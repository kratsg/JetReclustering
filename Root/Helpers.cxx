#include "xAODJetReclustering/Helpers.h"
#include "xAODJetReclustering/tools/Check.h"

// jet reclustering
#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include "JetInterface/IJetModifier.h"
#include "JetRec/PseudoJetGetter.h"
#include "JetRec/JetFromPseudojet.h"
#include "JetRec/JetFinder.h"
#include "JetRec/JetFilterTool.h"


// tools for adding information to jet
#include "JetSubStructureMomentTools/JetChargeTool.h"
#include "JetSubStructureMomentTools/JetPullTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorRatiosTool.h"
#include "JetSubStructureMomentTools/KTSplittingScaleTool.h"
#include "JetSubStructureMomentTools/DipolarityTool.h"
#include "JetSubStructureMomentTools/CenterOfMassShapesTool.h"

#include "JetMomentTools/JetWidthTool.h"

JetRecTool* xAODJetReclustering::JetFiltering(const std::string inputJetContainer, const std::string outputJetContainer, float ptMin){
  std::string uniqueName = "_"+inputJetContainer+"_"+outputJetContainer;

  ToolHandleArray<IJetModifier> modArray;
  JetFilterTool *jetFilterTool = new JetFilterTool("JetFilterTool"+uniqueName);
  CHECK("xAODJetReclustering", jetFilterTool->setProperty("PtMin", ptMin));
  modArray.push_back( ToolHandle<IJetModifier>( jetFilterTool ) );

  JetRecTool* inputJetFilterTool = new JetRecTool("InputJetFilteringTool"+uniqueName);
  CHECK("xAODJetReclustering", inputJetFilterTool->setProperty("InputContainer", inputJetContainer));
  CHECK("xAODJetReclustering", inputJetFilterTool->setProperty("OutputContainer", outputJetContainer));
  CHECK("xAODJetReclustering", inputJetFilterTool->setProperty("JetModifiers", modArray));
  CHECK("xAODJetReclustering", inputJetFilterTool->initialize());

  return inputJetFilterTool;
}

JetRecTool* xAODJetReclustering::JetReclusteringTool(const std::string inputJetContainer, const std::string outputJetContainer, float radius, fastjet::JetAlgorithm rc_alg, float ptMin){
  std::string uniqueName = "_"+inputJetContainer+"_"+outputJetContainer;

  ToolHandleArray<IJetExecuteTool> handleExec;

  ToolHandleArray<IPseudoJetGetter> getterArray;
  // Create a PseudoJet builder.
  PseudoJetGetter* lcgetter = new PseudoJetGetter("lcget"+uniqueName);
  //ToolStore::put(lcgetter);
  CHECK("xAODJetReclustering", lcgetter->setProperty("InputContainer", inputJetContainer));
  CHECK("xAODJetReclustering", lcgetter->setProperty("OutputContainer", "PseudoJets_"+uniqueName));
  CHECK("xAODJetReclustering", lcgetter->setProperty("Label", "LCTopo"));
  CHECK("xAODJetReclustering", lcgetter->setProperty("SkipNegativeEnergy", true));
  CHECK("xAODJetReclustering", lcgetter->setProperty("GhostScale", 0.0));
  CHECK("xAODJetReclustering", lcgetter->initialize());

  //ToolHandle<IPseudoJetGetter> hlcget(lcgetter);
  getterArray.push_back( ToolHandle<IPseudoJetGetter>(lcgetter) );

  JetFromPseudojet* jetFromPJ = new JetFromPseudojet("jetbuild"+uniqueName);
  //ToolStore::put(jetFromPJ);
  //std::vector<std::string> areatts = {"ActiveArea", "ActiveAreaFourVector"};
  //jetFromPJ->setProperty("Attributes", areatts);
  //jetFromPJ->msg().setLevel(MSG::ERROR);
  //jetFromPJ->msg().setLevel(MSG::VERBOSE);
  CHECK("xAODJetReclustering", jetFromPJ->initialize());

  std::map<fastjet::JetAlgorithm, std::string> algToAlgName = {{fastjet::kt_algorithm, "Kt"}, {fastjet::cambridge_algorithm, "CamKt"}, {fastjet::antikt_algorithm, "AntiKt"}};

  JetFinder* finder = new JetFinder("JetFinder"+uniqueName);
  //ToolStore::put(finder);
  CHECK("xAODJetReclustering", finder->setProperty("JetAlgorithm", algToAlgName.at(rc_alg)));
  CHECK("xAODJetReclustering", finder->setProperty("JetRadius", radius));
  CHECK("xAODJetReclustering", finder->setProperty("PtMin", ptMin));
  CHECK("xAODJetReclustering", finder->setProperty("GhostArea", 0.0));
  CHECK("xAODJetReclustering", finder->setProperty("RandomOption", 1));
  CHECK("xAODJetReclustering", finder->setProperty("JetBuilder", ToolHandle<IJetFromPseudojet>(jetFromPJ)));
  //finder->msg().setLevel(MSG::ERROR);
  CHECK("xAODJetReclustering", finder->initialize());

  // Create list of modifiers.
  ToolHandleArray<IJetModifier> modArray;
  modArray.push_back( ToolHandle<IJetModifier>( new JetChargeTool("JetChargeTool"+uniqueName) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new JetPullTool("JetPullTool"+uniqueName) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new EnergyCorrelatorTool("EnergyCorrelatorTool"+uniqueName) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new EnergyCorrelatorRatiosTool("EnergyCorrelatorRatiosTool"+uniqueName) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new KTSplittingScaleTool("KTSplittingScaleTool"+uniqueName) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new DipolarityTool("DipolarityTool"+uniqueName) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new CenterOfMassShapesTool("CenterOfMassShapesTool"+uniqueName) ) );

  modArray.push_back( ToolHandle<IJetModifier>( new JetWidthTool("JetWidthTool"+uniqueName) ) );

  JetRecTool* fullJetTool = new JetRecTool("FullJetRecTool"+uniqueName);
  CHECK("xAODJetReclustering", fullJetTool->setProperty("OutputContainer", outputJetContainer));
  CHECK("xAODJetReclustering", fullJetTool->setProperty("PseudoJetGetters", getterArray));
  CHECK("xAODJetReclustering", fullJetTool->setProperty("JetFinder", ToolHandle<IJetFinder>(finder)));
  CHECK("xAODJetReclustering", fullJetTool->setProperty("JetModifiers", modArray));

  //fullJetTool->msg().setLevel(MSG::DEBUG);
  CHECK("xAODJetReclustering", fullJetTool->initialize());

  // const xAOD::JetContainer* newjets = jetrectool->build();
  /*int status = fullJetTool->execute();*/

  return fullJetTool;
}
