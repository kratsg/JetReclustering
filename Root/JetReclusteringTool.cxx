#include "xAODJetReclustering/JetReclusteringTool.h"
#include "AsgTools/Check.h"

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
  m_trimJetTool                 (CxxUtils::make_unique<JetRecTool>("JetRec_JetTrimTool_"+name)),
  m_effectiveRTool              (CxxUtils::make_unique<EffectiveRTool>("EffectiveRTool_"+name)),
  m_jetTrimmingTool             (CxxUtils::make_unique<JetTrimmer>("JetTrimmerTool_"+name)),
  m_jetChargeTool               (CxxUtils::make_unique<JetChargeTool>("JetChargeTool_"+name)),
  m_jetPullTool                 (CxxUtils::make_unique<JetPullTool>("JetPullTool_"+name)),
  m_energyCorrelatorTool        (CxxUtils::make_unique<EnergyCorrelatorTool>("EnergyCorrelatorTool_"+name)),
  m_energyCorrelatorRatiosTool  (CxxUtils::make_unique<EnergyCorrelatorRatiosTool>("EnergyCorrelatorRatiosTool_"+name)),
  m_ktSplittingScaleTool        (CxxUtils::make_unique<KTSplittingScaleTool>("KTSplittingScaleTool_"+name)),
  m_dipolarityTool              (CxxUtils::make_unique<DipolarityTool>("DipolarityTool_"+name)),
  m_centerOfMassShapesTool      (CxxUtils::make_unique<CenterOfMassShapesTool>("CenterOfMassShapesTool_"+name)),
  m_nSubjettinessTool           (CxxUtils::make_unique<NSubjettinessTool>("NSubjettinessTool_"+name))
{
  declareProperty("InputJetContainer",  m_inputJetContainer = "");
  declareProperty("OutputJetContainer", m_outputJetContainer = "");
  declareProperty("ReclusterRadius",    m_radius = 1.0);
  declareProperty("ReclusterAlgorithm", m_rc_alg = "AntiKt");
  declareProperty("VariableRMinRadius", m_varR_minR =-1.0);
  declareProperty("VariableRMassScale", m_varR_mass =-1.0);
  declareProperty("InputJetPtMin",      m_ptMin_input = 25.0);
  declareProperty("RCJetPtMin",         m_ptMin_rc = 50.0);
  declareProperty("RCJetPtFrac",        m_ptFrac = 0.05);
  declareProperty("RCJetSubjetRadius",  m_subjet_radius = 0.2);
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
  if(m_ptMin_input > 0) filteredInputJetContainer = "Filtered_"+m_name;
  std::string filteredInputPseudoJetsContainer = "PseudoJets_"+m_name;
  std::string reclusteredJetsContainer = "ReclusteredJets_"+m_name;

  if(m_ptMin_input > 0){
    /* initialize input jet filtering */
    //    - create a tool that will filter jets
    ASG_CHECK(m_jetFilterTool->setProperty("PtMin", m_ptMin_input*1.e3));
    modArray.clear();
    modArray.push_back( ToolHandle<IJetModifier>( m_jetFilterTool.get() ) );
    //    - create the master tool to filter the input jets
    ASG_CHECK(m_inputJetFilterTool->setProperty("InputContainer", m_inputJetContainer));
    ASG_CHECK(m_inputJetFilterTool->setProperty("OutputContainer", filteredInputJetContainer));
    ASG_CHECK(m_inputJetFilterTool->setProperty("JetModifiers", modArray));
    /* note: we cannot use shallow copies since we are removing elements from a
     * container, we need a deep copy as linking will break */
    ASG_CHECK(m_inputJetFilterTool->setProperty("ShallowCopy", false));
    ASG_CHECK(m_inputJetFilterTool->initialize());
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
  ASG_CHECK(m_pseudoJetGetterTool->setProperty("InputContainer", filteredInputJetContainer));
  ASG_CHECK(m_pseudoJetGetterTool->setProperty("OutputContainer", filteredInputPseudoJetsContainer));
  ASG_CHECK(m_pseudoJetGetterTool->setProperty("Label", "LCTopo"));
  ASG_CHECK(m_pseudoJetGetterTool->setProperty("SkipNegativeEnergy", true));
  ASG_CHECK(m_pseudoJetGetterTool->setProperty("GhostScale", 0.0));
  ASG_CHECK(m_pseudoJetGetterTool->initialize());
  getterArray.push_back( ToolHandle<IPseudoJetGetter>(m_pseudoJetGetterTool.get()) );
  //    - create a Jet builder
  ASG_CHECK(m_jetFromPseudoJetTool->setProperty("Attributes", areaAttributes));
  ASG_CHECK(m_jetFromPseudoJetTool->initialize());
  //    - create a ClusterSequence Tool
  ASG_CHECK(m_jetFinderTool->setProperty("JetAlgorithm", m_rc_alg));
  ASG_CHECK(m_jetFinderTool->setProperty("JetRadius", m_radius));
  ASG_CHECK(m_jetFinderTool->setProperty("VariableRMinRadius", m_varR_minR));
  ASG_CHECK(m_jetFinderTool->setProperty("VariableRMassScale", m_varR_mass*1.e3));
  ASG_CHECK(m_jetFinderTool->setProperty("PtMin", m_ptMin_rc*1.e3));
  // set ghost area, ignore if trimming is being applied to reclustered jets
  ASG_CHECK(m_jetFinderTool->setProperty("GhostArea", ghostArea));
  ASG_CHECK(m_jetFinderTool->setProperty("RandomOption", 1));
  ASG_CHECK(m_jetFinderTool->setProperty("JetBuilder", ToolHandle<IJetFromPseudojet>(m_jetFromPseudoJetTool.get())));
  ASG_CHECK(m_jetFinderTool->initialize());
  //    - create list of modifiers.
  modArray.clear();
  //        we need to calculate effectiveR before trimming, if we are doing variableR
  modArray.push_back( ToolHandle<IJetModifier>( m_effectiveRTool.get() ) );
  //    - create our master reclustering tool
  ASG_CHECK(m_reclusterJetTool->setProperty("OutputContainer", reclusteredJetsContainer));
  ASG_CHECK(m_reclusterJetTool->setProperty("PseudoJetGetters", getterArray));
  ASG_CHECK(m_reclusterJetTool->setProperty("JetFinder", ToolHandle<IJetFinder>(m_jetFinderTool.get())));
  ASG_CHECK(m_reclusterJetTool->setProperty("JetModifiers", modArray));
  ASG_CHECK(m_reclusterJetTool->initialize());

  // clear modArray again
  modArray.clear();
  //        then trim the reclustered jets
  ASG_CHECK(m_jetTrimmingTool->setProperty("PtFrac", m_ptFrac));
  ASG_CHECK(m_jetTrimmingTool->setProperty("RClus", m_subjet_radius));
  ASG_CHECK(m_jetTrimmingTool->setProperty("JetBuilder", ToolHandle<IJetFromPseudojet>(m_jetFromPseudoJetTool.get())));
  //        and then apply all other modifiers based on the trimmed reclustered jets
  modArray.push_back( ToolHandle<IJetModifier>( m_jetChargeTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_jetPullTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_energyCorrelatorTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_energyCorrelatorRatiosTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_ktSplittingScaleTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_dipolarityTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_centerOfMassShapesTool.get() ) );
  modArray.push_back( ToolHandle<IJetModifier>( m_nSubjettinessTool.get() ) );
  // finish up the rest of the tool
  ASG_CHECK(m_trimJetTool->setProperty("InputContainer", reclusteredJetsContainer));
  ASG_CHECK(m_trimJetTool->setProperty("OutputContainer", m_outputJetContainer));
  ASG_CHECK(m_trimJetTool->setProperty("JetModifiers", modArray));
  ASG_CHECK(m_trimJetTool->setProperty("JetGroomer", ToolHandle<IJetGroomer>( m_jetTrimmingTool.get() ) ));
  ASG_CHECK(m_trimJetTool->initialize());

  //m_reclusterJetTool->msg().setLevel(MSG::VERBOSE);
  //m_trimJetTool->msg().setLevel(MSG::VERBOSE);

  return StatusCode::SUCCESS;
}

void JetReclusteringTool::execute() const {
  if(m_ptMin_input > 0)
    m_inputJetFilterTool->execute();
  m_reclusterJetTool->execute();
  m_trimJetTool->execute();
}

void JetReclusteringTool::print() const {
  std::cout << "Properties for " << m_APP_NAME << "(" << m_name << ")" << std::endl
            << "    InputJetContainer:      " << m_inputJetContainer << std::endl
            << "    OutputJetContainer:     " << m_outputJetContainer << std::endl
            << "    Radius:                 " << m_radius << std::endl
            << "    ReclusteringAlgorithm:  " << m_rc_alg << " (" << m_rc_alg << ")" << std::endl
            << "    VariableRMinRadius:     " << m_varR_minR << std::endl
            << "    VariableRMassScale:     " << m_varR_mass << " GeV" << std::endl
            << "    InputJetPtCut:          " << m_ptMin_input << " GeV" << std::endl
            << "    ReclusteredJetPtCut:    " << m_ptMin_rc << " GeV" << std::endl
            << "    ReclusteredJetPtFrac:   " << m_ptFrac << std::endl
            << "    ReclusteredJetSubjetR:  " << m_subjet_radius << std::endl;

  if(m_isInitialized){
    m_inputJetFilterTool->print();
    m_reclusterJetTool->print();
    m_trimJetTool->print();
  } else {
    std::cout << m_APP_NAME << " has not been initialized yet" << std::endl;
  }

  return;
}
