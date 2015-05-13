#include "xAODJetReclustering/JetReclusteringTool.h"
#include "xAODJetReclustering/tools/Check.h"

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include "JetInterface/IJetModifier.h"

// all jet modifier tools
#include "JetSubStructureMomentTools/JetChargeTool.h"
#include "JetSubStructureMomentTools/JetPullTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorRatiosTool.h"
#include "JetSubStructureMomentTools/KTSplittingScaleTool.h"
#include "JetSubStructureMomentTools/DipolarityTool.h"
#include "JetSubStructureMomentTools/CenterOfMassShapesTool.h"
#include "JetMomentTools/JetWidthTool.h"

JetReclusteringTool::JetReclusteringTool(std::string name) :
  m_name(name),
  m_jetFilterTool(new JetFilterTool("JetFilterTool_"+name)),
  m_inputJetFilterTool(new JetRecTool("JetRec_InputJetFilterTool_"+name)),
  m_pseudoJetGetterTool(new PseudoJetGetter("PseudoJetGetterTool_"+name)),
  m_jetFromPseudoJetTool(new JetFromPseudojet("JetFromPseudoJetTool_"+name)),
  m_jetFinderTool(new JetFinder("JetFinderTool_"+name)),
  m_reclusterJetTool(new JetRecTool("JetRec_JetReclusterTool_"+name))
{}

bool JetReclusteringTool::initialize(){
  if(m_isInitialized){
    std::cout << "Warning: " << m_APP_NAME << " already initialized." << std::endl;
    return false;
  }
  std::cout << "Info: " << m_APP_NAME << " initializing" << std::endl;
  // set to true, we're calling it now
  m_isInitialized = true;

  // set up some stuff to use within the initialize()
  ToolHandleArray<IJetModifier> modArray;
  ToolHandleArray<IPseudoJetGetter> getterArray;

  // this is for intermediate output containers used between tools
  std::string filteredInputJetContainer = m_inputJetContainer+"_"+m_name+"_FilteredPtMin_"+std::to_string(static_cast<int>(m_ptMin_input));
  std::string filteredInputPseudoJetsContainer = "PseudoJets_"+filteredInputJetContainer;

  // for the CHECK() statements
  const char* prettyFuncName = (m_APP_NAME+"::initialize()").c_str();

  /* initialize input jet filtering */
  //    - create a tool that will filter jets
  CHECK(prettyFuncName, m_jetFilterTool->setProperty("PtMin", m_ptMin_input*1.e3));
  modArray.clear();
  modArray.push_back( ToolHandle<IJetModifier>( m_jetFilterTool.get() ) );
  //    - create the master tool to filter the input jets
  CHECK(prettyFuncName, m_inputJetFilterTool->setProperty("InputContainer", m_inputJetContainer));
  CHECK(prettyFuncName, m_inputJetFilterTool->setProperty("OutputContainer", filteredInputJetContainer));
  CHECK(prettyFuncName, m_inputJetFilterTool->setProperty("JetModifiers", modArray));
  /* note: we cannot use shallow copies since we are removing elements from a
   * container, we need a deep copy as linking will break */
  CHECK(prettyFuncName, m_inputJetFilterTool->setProperty("ShallowCopy", false));
  CHECK(prettyFuncName, m_inputJetFilterTool->initialize());

  /* initialize jet reclustering */
  //    - create a PseudoJet builder.
  CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("InputContainer", filteredInputJetContainer));
  CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("OutputContainer", filteredInputPseudoJetsContainer));
  CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("Label", "LCTopo"));
  CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("SkipNegativeEnergy", true));
  CHECK(prettyFuncName, m_pseudoJetGetterTool->setProperty("GhostScale", 0.0));
  CHECK(prettyFuncName, m_pseudoJetGetterTool->initialize());
  getterArray.push_back( ToolHandle<IPseudoJetGetter>(m_pseudoJetGetterTool.get()) );
  //    - create a Jet builder
  CHECK(prettyFuncName, m_jetFromPseudoJetTool->initialize());
  //    - create a ClusterSequence Tool
  CHECK(prettyFuncName, m_jetFinderTool->setProperty("JetAlgorithm", algToAlgName.at(m_rc_alg)));
  CHECK(prettyFuncName, m_jetFinderTool->setProperty("JetRadius", m_radius));
  CHECK(prettyFuncName, m_jetFinderTool->setProperty("PtMin", m_ptMin_rc*1.e3));
  CHECK(prettyFuncName, m_jetFinderTool->setProperty("GhostArea", 0.0));
  CHECK(prettyFuncName, m_jetFinderTool->setProperty("RandomOption", 1));
  CHECK(prettyFuncName, m_jetFinderTool->setProperty("JetBuilder", ToolHandle<IJetFromPseudojet>(m_jetFromPseudoJetTool.get())));
  CHECK(prettyFuncName, m_jetFinderTool->initialize());
  //    - create list of modifiers.
  modArray.clear();
  modArray.push_back( ToolHandle<IJetModifier>( new JetChargeTool("JetChargeTool_"+m_name) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new JetPullTool("JetPullTool_"+m_name) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new EnergyCorrelatorTool("EnergyCorrelatorTool_"+m_name) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new EnergyCorrelatorRatiosTool("EnergyCorrelatorRatiosTool_"+m_name) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new KTSplittingScaleTool("KTSplittingScaleTool_"+m_name) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new DipolarityTool("DipolarityTool_"+m_name) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new CenterOfMassShapesTool("CenterOfMassShapesTool_"+m_name) ) );
  modArray.push_back( ToolHandle<IJetModifier>( new JetWidthTool("JetWidthTool_"+m_name) ) );
  //    - create our master reclustering tool
  CHECK(prettyFuncName, m_reclusterJetTool->setProperty("OutputContainer", m_outputJetContainer));
  CHECK(prettyFuncName, m_reclusterJetTool->setProperty("PseudoJetGetters", getterArray));
  CHECK(prettyFuncName, m_reclusterJetTool->setProperty("JetFinder", ToolHandle<IJetFinder>(m_jetFinderTool.get())));
  CHECK(prettyFuncName, m_reclusterJetTool->setProperty("JetModifiers", modArray));
  CHECK(prettyFuncName, m_reclusterJetTool->initialize());

  return true;
}

void JetReclusteringTool::execute() const {
  m_inputJetFilterTool->execute();
  m_reclusterJetTool->execute();
}

void JetReclusteringTool::print() const {
  std::cout << "Properties for " << m_APP_NAME << "(" << m_name << ")" << std::endl
            << "    InputJetContainer:      " << m_inputJetContainer << std::endl
            << "    OutputJetContainer:     " << m_outputJetContainer << std::endl
            << "    Radius:                 " << m_radius << std::endl
            << "    ReclusteringAlgorithm:  " << algToAlgName.at(m_rc_alg) << " (" << m_rc_alg << ")" << std::endl
            << "    InputJetPtCut:          " << m_ptMin_input << " GeV" << std::endl
            << "    ReclusteredJetPtCut:    " << m_ptMin_rc << " GeV" << std::endl
            << "    ReclusteredJetPtFrac:   " << m_ptFrac << std::endl;
  if(m_isInitialized){
    m_inputJetFilterTool->print();
    m_reclusterJetTool->print();
  } else {
    std::cout << m_APP_NAME << " has not been initialized yet" << std::endl;
  }

  return;
}
