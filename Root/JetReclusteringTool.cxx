#include "xAODJetReclustering/JetReclusteringTool.h"
#include "AsgTools/Check.h"

#include "JetInterface/IJetModifier.h"
#include "JetInterface/IJetFromPseudojet.h"
#include "JetInterface/IJetGroomer.h"
#include "JetInterface/IPseudoJetGetter.h"
#include "JetInterface/IJetFinder.h"
#include "JetInterface/IJetExecuteTool.h"

// make unique pointers
//#include<CxxUtils/make_unique.h>

#include <sstream>

JetReclusteringTool::JetReclusteringTool(std::string name) :
  AsgTool(name),
  m_name(name),
  m_jetFilterTool("JetFilterTool/JetFilterTool", this),
  m_inputJetFilterTool("JetRecTool/JetRec_InputJetFilterTool", this),
  m_pseudoJetGetterTool("PseudoJetGetter/PseudoJetGetterTool", this),
  m_jetFromPseudoJetTool("JetFromPseudojet/JetFromPseudoJetTool", this),
  m_jetFinderTool("JetFinder/JetFinderTool", this),
  m_reclusterJetTool("JetRecTool/JetRec_JetReclusterTool", this),
  m_trimJetTool("JetRecTool/JetRec_JetTrimTool", this),
  m_effectiveRTool("EffectiveRTool/EffectiveRTool", this),
  m_jetTrimmingTool("JetTrimmer/JetTrimmerTool", this),
  m_jetChargeTool("JetChargeTool/JetChargeTool", this),
  m_jetPullTool("JetPullTool/JetPullTool", this),
  m_energyCorrelatorTool("EnergyCorrelatorTool/EnergyCorrelatorTool", this),
  m_energyCorrelatorRatiosTool("EnergyCorrelatorRatiosTool/EnergyCorrelatorRatiosTool", this),
  m_ktSplittingScaleTool("KTSplittingScaleTool/KTSplittingScaleTool", this),
  m_dipolarityTool("DipolarityTool/DipolarityTool", this),
  m_centerOfMassShapesTool("CenterOfMassShapesTool/CenterOfMassShapesTool", this),
  m_nSubjettinessTool("NSubjettinessTool/NSubjettinessTool", this)
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
    ATH_MSG_WARNING(m_APP_NAME << " already initialized.");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(m_APP_NAME << " initializing");

  ATH_MSG_INFO( "With properties:" );
  ATH_MSG_INFO( "InputJetContainer: " << m_inputJetContainer );
  ATH_MSG_INFO( "OutputJetContainer: " << m_outputJetContainer );
  ATH_MSG_INFO( "ReclusterRadius: " << m_radius );

  // set to true, we're calling it now
  m_isInitialized = true;

  // set up some stuff to use within the initialize()
  ToolHandleArray<IJetModifier> modArray;
  ToolHandleArray<IPseudoJetGetter> getterArray;

  // this is for intermediate output containers used between tools
  std::string filteredInputJetContainer(m_inputJetContainer);
  if(m_ptMin_input > 0) filteredInputJetContainer = "Filtered_"+name();
  std::string filteredInputPseudoJetsContainer = "PseudoJets_"+name();
  std::string reclusteredJetsContainer = "ReclusteredJets_"+name();

  if(m_ptMin_input > 0){
    /* initialize input jet filtering */
    //    - create a tool that will filter jets
    ASG_CHECK(m_jetFilterTool.setProperty("PtMin", m_ptMin_input*1.e3));
    ASG_CHECK(m_jetFilterTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_jetFilterTool.retrieve() );
    modArray.clear();
    //modArray.push_back( m_jetFilterTool.getHandle() );
    modArray.push_back( m_jetFilterTool.name() );

    //    - create the master tool to filter the input jets
    ASG_CHECK(m_inputJetFilterTool.setProperty("InputContainer", m_inputJetContainer));
    ASG_CHECK(m_inputJetFilterTool.setProperty("OutputContainer", filteredInputJetContainer));
    ASG_CHECK(m_inputJetFilterTool.setProperty("JetModifiers", modArray));
    /* note: we cannot use shallow copies since we are removing elements from a
     * container, we need a deep copy as linking will break */
    ASG_CHECK(m_inputJetFilterTool.setProperty("ShallowCopy", false));
    ASG_CHECK(m_inputJetFilterTool.setProperty("OutputLevel", msg().level() ) );
    ASG_CHECK(m_inputJetFilterTool.retrieve());
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
  ASG_CHECK(m_pseudoJetGetterTool.setProperty("InputContainer", filteredInputJetContainer));
  ASG_CHECK(m_pseudoJetGetterTool.setProperty("OutputContainer", filteredInputPseudoJetsContainer));
  ASG_CHECK(m_pseudoJetGetterTool.setProperty("Label", "LCTopo"));
  ASG_CHECK(m_pseudoJetGetterTool.setProperty("SkipNegativeEnergy", true));
  ASG_CHECK(m_pseudoJetGetterTool.setProperty("GhostScale", 0.0));
  ASG_CHECK(m_pseudoJetGetterTool.setProperty("OutputLevel", msg().level() ) );
  ASG_CHECK(m_pseudoJetGetterTool.retrieve());
  getterArray.push_back( m_pseudoJetGetterTool.getHandle() );
  //    - create a Jet builder
  ASG_CHECK(m_jetFromPseudoJetTool.setProperty("Attributes", areaAttributes));
  ASG_CHECK(m_jetFromPseudoJetTool.setProperty("OutputLevel", msg().level() ) );
  ASG_CHECK(m_jetFromPseudoJetTool.retrieve());
  //    - create a ClusterSequence Tool
  ASG_CHECK(m_jetFinderTool.setProperty("JetAlgorithm", m_rc_alg));
  ASG_CHECK(m_jetFinderTool.setProperty("JetRadius", m_radius));
  ASG_CHECK(m_jetFinderTool.setProperty("VariableRMinRadius", m_varR_minR));
  ASG_CHECK(m_jetFinderTool.setProperty("VariableRMassScale", m_varR_mass*1.e3));
  ASG_CHECK(m_jetFinderTool.setProperty("PtMin", m_ptMin_rc*1.e3));
  // set ghost area, ignore if trimming is being applied to reclustered jets
  ASG_CHECK(m_jetFinderTool.setProperty("GhostArea", ghostArea));
  ASG_CHECK(m_jetFinderTool.setProperty("RandomOption", 1));
  ASG_CHECK(m_jetFinderTool.setProperty("JetBuilder", m_jetFromPseudoJetTool));
  ASG_CHECK(m_jetFinderTool.setProperty("OutputLevel", msg().level() ) );
  ASG_CHECK(m_jetFinderTool.retrieve());
  //    - create list of modifiers.
  modArray.clear();
  //        we need to calculate effectiveR before trimming, if we are doing variableR
  modArray.push_back( m_effectiveRTool.getHandle() );
  //    - create our master reclustering tool
  ASG_CHECK(m_reclusterJetTool.setProperty("OutputContainer", reclusteredJetsContainer));
  ASG_CHECK(m_reclusterJetTool.setProperty("PseudoJetGetters", getterArray));
  ASG_CHECK(m_reclusterJetTool.setProperty("JetFinder", m_jetFinderTool));
  ASG_CHECK(m_reclusterJetTool.setProperty("JetModifiers", modArray));
  ASG_CHECK(m_reclusterJetTool.setProperty("OutputLevel", msg().level() ) );
  ASG_CHECK(m_reclusterJetTool.retrieve());

  // clear modArray again
  modArray.clear();
  //        then trim the reclustered jets
  ASG_CHECK(m_jetTrimmingTool.setProperty("PtFrac", m_ptFrac));
  ASG_CHECK(m_jetTrimmingTool.setProperty("RClus", m_subjet_radius));
  ASG_CHECK(m_jetTrimmingTool.setProperty("JetBuilder", m_jetFromPseudoJetTool));
  ASG_CHECK(m_jetTrimmingTool.setProperty("OutputLevel", msg().level() ) );
  ASG_CHECK(m_jetTrimmingTool.retrieve() );
  //        and then apply all other modifiers based on the trimmed reclustered jets
  modArray.push_back(  m_jetChargeTool.getHandle() );
  modArray.push_back(  m_jetPullTool.getHandle() );
  modArray.push_back(  m_energyCorrelatorTool.getHandle() );
  modArray.push_back(  m_energyCorrelatorRatiosTool.getHandle() );
  modArray.push_back(  m_ktSplittingScaleTool.getHandle() );
  modArray.push_back(  m_dipolarityTool.getHandle() );
  modArray.push_back(  m_centerOfMassShapesTool.getHandle() );
  modArray.push_back(  m_nSubjettinessTool.getHandle() );
  // finish up the rest of the tool
  ASG_CHECK(m_trimJetTool.setProperty("InputContainer", reclusteredJetsContainer));
  ASG_CHECK(m_trimJetTool.setProperty("OutputContainer", m_outputJetContainer));
  ASG_CHECK(m_trimJetTool.setProperty("JetModifiers", modArray));
  ASG_CHECK(m_trimJetTool.setProperty("JetGroomer",  m_jetTrimmingTool ));
  ASG_CHECK(m_trimJetTool.setProperty("OutputLevel", msg().level() ) );
  ASG_CHECK(m_trimJetTool.retrieve());

  //m_reclusterJetTool->msg().setLevel(MSG::VERBOSE);
  //m_trimJetTool->msg().setLevel(MSG::VERBOSE);

  return StatusCode::SUCCESS;
}

int JetReclusteringTool::execute() const {
  int retCode = 0;
  if(m_ptMin_input > 0) {
    if ( (retCode = m_inputJetFilterTool->execute() ) != 0) {
      ATH_MSG_ERROR( "Failure in inputJetFilterTool with return code: " << retCode );
      return retCode;
    }
  }
  if ( (retCode = m_reclusterJetTool->execute() ) != 0) {
    ATH_MSG_ERROR( "Failure in reclusterJetTool with return code: " << retCode );
    return retCode;
  }
  if ( (retCode = m_trimJetTool->execute() ) != 0) {
    ATH_MSG_ERROR( "Failure in trimJetTool with return code: " << retCode );
    return retCode;
  }
  return 0;
}

void JetReclusteringTool::print() const {
  ATH_MSG_INFO("Properties for " << m_APP_NAME << "(" << m_name << ")");
  ATH_MSG_INFO("    InputJetContainer:      " << m_inputJetContainer );
  ATH_MSG_INFO("    OutputJetContainer:     " << m_outputJetContainer );
  ATH_MSG_INFO("    Radius:                 " << m_radius );
  ATH_MSG_INFO("    ReclusteringAlgorithm:  " << m_rc_alg << " (" << m_rc_alg << ")" );
  ATH_MSG_INFO("    VariableRMinRadius:     " << m_varR_minR );
  ATH_MSG_INFO("    VariableRMassScale:     " << m_varR_mass << " GeV" );
  ATH_MSG_INFO("    InputJetPtCut:          " << m_ptMin_input << " GeV" );
  ATH_MSG_INFO("    ReclusteredJetPtCut:    " << m_ptMin_rc << " GeV" );
  ATH_MSG_INFO("    ReclusteredJetPtFrac:   " << m_ptFrac );
  ATH_MSG_INFO("    ReclusteredJetSubjetR:  " << m_subjet_radius );

  if(m_isInitialized){
    m_inputJetFilterTool->print();
    m_reclusterJetTool->print();
    m_trimJetTool->print();
  } else {
    ATH_MSG_INFO(m_APP_NAME << " has not been initialized yet");
  }

  return;
}
