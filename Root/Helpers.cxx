#include "xAODJetReclustering/Helpers.h"

// for templating
#include "xAODBase/IParticleContainer.h"

// jet reclustering
#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include "JetInterface/IJetModifier.h"
#include "JetRec/PseudoJetGetter.h"
#include "JetRec/JetFromPseudojet.h"
#include "JetRec/JetFinder.h"

// tools for adding information to jet
#include "JetSubStructureMomentTools/NSubjettinessTool.h"
//#include "JetSubStructureMomentTools/NSubjettinessRatiosTool.h" // doesn't work
#include "JetSubStructureMomentTools/JetChargeTool.h"
#include "JetSubStructureMomentTools/JetPullTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorRatiosTool.h"
#include "JetSubStructureMomentTools/KTSplittingScaleTool.h"
#include "JetSubStructureMomentTools/DipolarityTool.h"
#include "JetSubStructureMomentTools/CenterOfMassShapesTool.h"

#include "JetMomentTools/JetWidthTool.h"

xAODJetReclustering :: Helpers :: Helpers ()
{
}


struct xAODJetReclustering :: Helpers :: sort_by_pt
{

    inline bool operator() (const TLorentzVector& lhs, const TLorentzVector& rhs)
    {
      return (lhs.Pt() > rhs.Pt());
    }

    inline bool operator() (const TLorentzVector* lhs, const TLorentzVector* rhs)
    {
      return (lhs->Pt() > rhs->Pt());
    }

    inline bool operator() (const xAOD::IParticle& lhs, const xAOD::IParticle& rhs)
    {
      return (lhs.pt() > rhs.pt());
    }

    inline bool operator() (const xAOD::IParticle* lhs, const xAOD::IParticle* rhs)
    {
      return (lhs->pt() > rhs->pt());
    }
};

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*\
|                                                                               |
|   Author  : Giordon Stark                                                     |
|   Email   : gstark@cern.ch                                                    |
|   Thanks to Ben Nachman for inspiration, P-A for the help                     |
|                                                                               |
|   jet_reclustering():                                                         |
|       Takes a set of small-R jets and reclusters to large-R jets              |
|                                                                               |
|       @inputJetContainer  : name of small-R jet container                     |
|       @outputJetContainer : name of new jet container to record in TStore     |
|       @radius             : radius of large-R jets                            |
|       @rc_alg             : reclustering algorithm to use (AntiKt, Kt, CamKt) |
|       @ptMin              : minimum Pt cut on reclustered jets                |
|                                                                               |
|                                                                               |
|                                                                               |
|                                                                               |
\*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
JetRecTool* xAODJetReclustering::Helpers::JetxAODJetReclusteringTool(const std::string inputJetContainer, const std::string outputJetContainer, double radius, fastjet::JetAlgorithm rc_alg, float ptMin){
  std::string uniqueName = "_"+inputJetContainer+"_"+outputJetContainer;

  ToolHandleArray<IJetExecuteTool> handleExec;

  ToolHandleArray<IPseudoJetGetter> getterArray;
  // Create a PseudoJet builder.
  PseudoJetGetter* lcgetter = new PseudoJetGetter("lcget"+uniqueName);
  //ToolStore::put(lcgetter);
  lcgetter->setProperty("InputContainer", inputJetContainer);
  lcgetter->setProperty("OutputContainer", "PseudoJets_"+uniqueName);
  lcgetter->setProperty("Label", "LCTopo");
  lcgetter->setProperty("SkipNegativeEnergy", true);
  lcgetter->setProperty("GhostScale", 0.0);
  lcgetter->initialize();

  //ToolHandle<IPseudoJetGetter> hlcget(lcgetter);
  getterArray.push_back( ToolHandle<IPseudoJetGetter>(lcgetter) );

  JetFromPseudojet* jetFromPJ = new JetFromPseudojet("jetbuild"+uniqueName);
  //ToolStore::put(jetFromPJ);
  //std::vector<std::string> areatts = {"ActiveArea", "ActiveAreaFourVector"};
  //jetFromPJ->setProperty("Attributes", areatts);
  //jetFromPJ->msg().setLevel(MSG::ERROR);
  //jetFromPJ->msg().setLevel(MSG::VERBOSE);
  jetFromPJ->initialize();

  std::map<fastjet::JetAlgorithm, std::string> algToAlgName = {{fastjet::kt_algorithm, "Kt"}, {fastjet::cambridge_algorithm, "CamKt"}, {fastjet::antikt_algorithm, "AntiKt"}};

  JetFinder* finder = new JetFinder("JetFinder"+uniqueName);
  //ToolStore::put(finder);
  finder->setProperty("JetAlgorithm", algToAlgName.at(rc_alg));
  finder->setProperty("JetRadius", radius);
  finder->setProperty("PtMin", ptMin);
  finder->setProperty("GhostArea", 0.0);
  finder->setProperty("RandomOption", 1);
  finder->setProperty("JetBuilder", ToolHandle<IJetFromPseudojet>(jetFromPJ));
  //finder->msg().setLevel(MSG::ERROR);
  finder->initialize();

  // Create list of modifiers.
  ToolHandleArray<IJetModifier> modArray;
  modArray.push_back( ToolHandle<IJetModifier>( new NSubjettinessTool("NSubjettinessTool"+uniqueName) ) );
  //modArray.push_back( ToolHandle<IJetModifier>( new NSubjettinessRatiosTool("NSubjettinessRatiosTool") ) );
  modArray.push_back( ToolHandle<IJetModifier>( new JetChargeTool("JetChargeTool"+uniqueName) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new JetPullTool("JetPullTool"+uniqueName) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new EnergyCorrelatorTool("EnergyCorrelatorTool"+uniqueName) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new EnergyCorrelatorRatiosTool("EnergyCorrelatorRatiosTool"+uniqueName) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new KTSplittingScaleTool("KTSplittingScaleTool"+uniqueName) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new DipolarityTool("DipolarityTool"+uniqueName) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new CenterOfMassShapesTool("CenterOfMassShapesTool"+uniqueName) ) );

  modArray.push_back( ToolHandle<IJetModifier>( new JetWidthTool("JetWidthTool"+uniqueName) ) );

  JetRecTool* fullJetTool = new JetRecTool("FullJetRecTool"+uniqueName);
  fullJetTool->setProperty("OutputContainer", outputJetContainer);
  fullJetTool->setProperty("PseudoJetGetters", getterArray);
  fullJetTool->setProperty("JetFinder", ToolHandle<IJetFinder>(finder));
  fullJetTool->setProperty("JetModifiers", modArray);

  //fullJetTool->msg().setLevel(MSG::DEBUG);
  fullJetTool->initialize();

  // const xAOD::JetContainer* newjets = jetrectool->build();
  /*int status = fullJetTool->execute();*/

  return fullJetTool;
}
