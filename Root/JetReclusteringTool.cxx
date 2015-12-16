#include "xAODJetReclustering/JetReclusteringTool.h"
#include "xAODJetReclustering/tools/Check.h"

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
#include "JetInterface/IJetModifier.h"

// make unique pointers
#include<CxxUtils/make_unique.h>

#include <sstream>

JetReclusteringTool::JetReclusteringTool(std::string name) :
  AsgTool(name),
  m_name(name),
  m_jetFilterTool               (CxxUtils::make_unique<JetFilterTool>("JetFilterTool_"+name)),
  m_inputJetFilterTool          (CxxUtils::make_unique<JetRecTool>("JetRec_InputJetFilterTool_"+name)),
  m_pseudoJetGetterTool         (CxxUtils::make_unique<PseudoJetGetter>("PseudoJetGetterTool_"+name)),
  m_jetFromPseudoJetTool        (CxxUtils::make_unique<JetFromPseudojet>("JetFromPseudoJetTool_"+name)),
  m_jetFinderTool               (CxxUtils::make_unique<JetFinder>("JetFinderTool_"+name)),
  m_reclusterJetTool            (CxxUtils::make_unique<JetRecTool>("JetRec_JetReclusterTool_"+name)),
  m_effectiveRTool              (CxxUtils::make_unique<EffectiveRTool>("EffectiveRTool_"+name)),
  m_jetTrimmingTool             (CxxUtils::make_unique<JetTrimmer>("JetTrimmerTool_"+name)),
  m_jetChargeTool               (CxxUtils::make_unique<JetChargeTool>("JetChargeTool_"+name)),
  m_jetPullTool                 (CxxUtils::make_unique<JetPullTool>("JetPullTool_"+name)),
  m_energyCorrelatorTool        (CxxUtils::make_unique<EnergyCorrelatorTool>("EnergyCorrelatorTool_"+name)),
  m_energyCorrelatorRatiosTool  (CxxUtils::make_unique<EnergyCorrelatorRatiosTool>("EnergyCorrelatorRatiosTool_"+name)),
  m_ktSplittingScaleTool        (CxxUtils::make_unique<KTSplittingScaleTool>("KTSplittingScaleTool_"+name)),
  m_dipolarityTool              (CxxUtils::make_unique<DipolarityTool>("DipolarityTool_"+name)),
  m_centerOfMassShapesTool      (CxxUtils::make_unique<CenterOfMassShapesTool>("CenterOfMassShapesTool_"+name)),
  m_jetWidthTool                (CxxUtils::make_unique<JetWidthTool>("JetWidthTool_"+name)),
  m_nSubjettinessTool           (CxxUtils::make_unique<NSubjettinessTool>("NSubjettinessTool_"+name))
{
  declareProperty("InputJetContainer",  m_inputJetContainer = "");
  declareProperty("OutputJetContainer", m_outputJetContainer = "");
  declareProperty("ReclusterRadius",    m_radius = 1.0);
  declareProperty("ReclusterAlgorithm", m_rc_alg = fastjet::antikt_algorithm);
  declareProperty("VariableRMinRadius", m_varR_minR =-1.0);
  declareProperty("VariableRMassScale", m_varR_mass =-1.0);
  declareProperty("InputJetPtMin",      m_ptMin_input = 25.0);
  declareProperty("RCJetPtMin",         m_ptMin_rc = 50.0);
  declareProperty("RCJetPtFrac",        m_ptFrac = 0.05);
  declareProperty("RCJetSubjetRadius",  m_subjet_radius = 0.0);
  declareProperty("DoArea",             m_doArea = false);
  declareProperty("AreaAttributes",     m_areaAttributes = "ActiveArea ActiveArea4vec");
}

StatusCode JetReclusteringTool::initialize(){
  if(m_isInitialized){
    std::cout << "Warning: " << m_APP_NAME << " already initialized." << std::endl;
    return StatusCode::FAILURE;
  }
  std::cout << "Info: " << m_APP_NAME << " initializing" << std::endl;
  // set to true, we're calling it now
  m_isInitialized = true;

  // set up some stuff to use within the initialize()
  ToolHandleArray<IJetModifier> modArray;
  ToolHandleArray<IPseudoJetGetter> getterArray;

  // this is for intermediate output containers used between tools
  std::string filteredInputJetContainer(m_inputJetContainer);
  if(m_ptMin_input > 0)
    filteredInputJetContainer = m_inputJetContainer+"_"+m_name+"_FilteredPtMin_"+std::to_string(static_cast<int>(m_ptMin_input));

  std::string filteredInputPseudoJetsContainer = "PseudoJets_"+filteredInputJetContainer;

  // for the CHECK() statements
  const char* prettyFuncName = (m_APP_NAME+"::initialize()").c_str();

  if(m_ptMin_input > 0){
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
  }

  // only compute area if ptFrac = 0.0 and m_areaAttributes is specified
  float ghostArea(0.0);
  std::vector<std::string> areaAttributes;
  if(m_doArea){
    ghostArea = 0.01;
    // split up the m_areaAttributes string specifying which attributes to record
    std::string token;
    std::istringstream ss(m_areaAttributes);
    while(std::getline(ss, token, ' '))
      areaAttributes.push_back(token);
  }

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
  CHECK(prettyFuncName, m_jetFromPseudoJetTool->setProperty("Attributes", areaAttributes));
  CHECK(prettyFuncName, m_jetFromPseudoJetTool->initialize());
  //    - create a ClusterSequence Tool
  CHECK(prettyFuncName, m_jetFinderTool->setProperty("JetAlgorithm", algToAlgName.at(m_rc_alg)));
  CHECK(prettyFuncName, m_jetFinderTool->setProperty("JetRadius", m_radius));
  CHECK(prettyFuncName, m_jetFinderTool->setProperty("VariableRMinRadius", m_varR_minR));
  CHECK(prettyFuncName, m_jetFinderTool->setProperty("VariableRMassScale", m_varR_mass*1.e3));
  CHECK(prettyFuncName, m_jetFinderTool->setProperty("PtMin", m_ptMin_rc*1.e3));
  // set ghost area, ignore if trimming is being applied to reclustered jets
  CHECK(prettyFuncName, m_jetFinderTool->setProperty("GhostArea", ghostArea));
  CHECK(prettyFuncName, m_jetFinderTool->setProperty("RandomOption", 1));
  CHECK(prettyFuncName, m_jetFinderTool->setProperty("JetBuilder", ToolHandle<IJetFromPseudojet>(m_jetFromPseudoJetTool.get())));
  CHECK(prettyFuncName, m_jetFinderTool->initialize());
  //    - create list of modifiers.
  modArray.clear();
  //        we need to calculate effectiveR before trimming, if we are doing variableR
  modArray.push_back( ToolHandle<IJetModifier>( m_effectiveRTool.get() ) );
  if(m_ptFrac > 0){
    //        then trim the reclustered jets
    CHECK(prettyFuncName, m_jetTrimmingTool->setProperty("PtFrac", m_ptFrac));
    CHECK(prettyFuncName, m_jetTrimmingTool->setProperty("RClus", m_subjet_radius));
    CHECK(prettyFuncName, m_jetTrimmingTool->setProperty("JetBuilder", ToolHandle<IJetFromPseudojet>(m_jetFromPseudoJetTool.get())));
    // add to recluster jet tool
    CHECK(prettyFuncName, m_reclusterJetTool->setProperty("JetGroomer", ToolHandle<IJetGroomer>( m_jetTrimmingTool.get() ) ));
  }
  //        and then apply all other modifiers based on the trimmed reclustered jets
  modArray.push_back( ToolHandle<IJetModifier>( m_jetChargeTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_jetPullTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_energyCorrelatorTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_energyCorrelatorRatiosTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_ktSplittingScaleTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_dipolarityTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_centerOfMassShapesTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_jetWidthTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_nSubjettinessTool.get() ) );
  //    - create our master reclustering tool
  CHECK(prettyFuncName, m_reclusterJetTool->setProperty("OutputContainer", m_outputJetContainer));
  CHECK(prettyFuncName, m_reclusterJetTool->setProperty("PseudoJetGetters", getterArray));
  CHECK(prettyFuncName, m_reclusterJetTool->setProperty("JetFinder", ToolHandle<IJetFinder>(m_jetFinderTool.get())));
  CHECK(prettyFuncName, m_reclusterJetTool->setProperty("JetModifiers", modArray));
  CHECK(prettyFuncName, m_reclusterJetTool->initialize());

  return StatusCode::SUCCESS;
}

void JetReclusteringTool::execute() const {
  if(m_ptMin_input > 0)
    m_inputJetFilterTool->execute();
  m_reclusterJetTool->execute();
}

void JetReclusteringTool::print() const {
  std::cout << "Properties for " << m_APP_NAME << "(" << m_name << ")" << std::endl
            << "    InputJetContainer:      " << m_inputJetContainer << std::endl
            << "    OutputJetContainer:     " << m_outputJetContainer << std::endl
            << "    Radius:                 " << m_radius << std::endl
            << "    ReclusteringAlgorithm:  " << algToAlgName.at(m_rc_alg) << " (" << m_rc_alg << ")" << std::endl
            << "    VariableRMinRadius:     " << m_varR_minR << std::endl
            << "    VariableRMassScale:     " << m_varR_mass << " GeV" << std::endl
            << "    InputJetPtCut:          " << m_ptMin_input << " GeV" << std::endl
            << "    ReclusteredJetPtCut:    " << m_ptMin_rc << " GeV" << std::endl
            << "    ReclusteredJetPtFrac:   " << m_ptFrac << std::endl
            << "    ReclusteredJetSubjetR:  " << m_subjet_radius << std::endl;


  if(m_isInitialized){
    m_inputJetFilterTool->print();
    m_reclusterJetTool->print();
  } else {
    std::cout << m_APP_NAME << " has not been initialized yet" << std::endl;
  }

  return;
}
