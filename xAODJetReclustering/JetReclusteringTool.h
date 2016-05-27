#ifndef xAODJetReclustering_JetReclusteringTool_H
#define xAODJetReclustering_JetReclusteringTool_H

// making it more like a tool
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"
#include "xAODJetReclustering/IJetReclusteringTool.h"

#include <map>
#include <memory>

/*
  Author  : Giordon Stark
  Email   : gstark@cern.ch
  Thanks to Ben Nachman for inspiration, P-A for the help

  Takes a set of small-R jets and reclusters to large-R jets

  @inputJetContainer  : name of small-R jet container
  @outputJetContainer : name of new jet container to record in TStore
  @radius             : radius of large-R jets
  @rc_alg             : reclustering algorithm to use (AntiKt, Kt, CamKt)
  @ptMin              : minimum Pt cut on reclustered jets
*/
// all general tools used
//#include "JetRec/PseudoJetGetter.h"
//#include "JetRec/JetFromPseudojet.h"
//#include "JetRec/JetFinder.h"
//#include "JetRec/JetFilterTool.h"
//#include "JetRec/JetRecTool.h"
//#include "JetRec/JetTrimmer.h"
// calculate EffectiveR
//#include "xAODJetReclustering/EffectiveRTool.h"
// all jet modifier tools
//#include "JetSubStructureMomentTools/JetChargeTool.h"
//#include "JetSubStructureMomentTools/JetPullTool.h"
//#include "JetSubStructureMomentTools/EnergyCorrelatorTool.h"
//#include "JetSubStructureMomentTools/EnergyCorrelatorRatiosTool.h"
//#include "JetSubStructureMomentTools/KTSplittingScaleTool.h"
//#include "JetSubStructureMomentTools/DipolarityTool.h"
//#include "JetSubStructureMomentTools/CenterOfMassShapesTool.h"
//#include "JetSubStructureMomentTools/NSubjettinessTool.h"

class IJetModifier;
class IJetExecuteTool;
class IJetFromPseudojet;
class IJetFinder;
class IJetGroomer;
class IPseudoJetGetter;

class JetReclusteringTool : public asg::AsgTool, virtual public IJetReclusteringTool {
  public:
    ASG_TOOL_CLASS(JetReclusteringTool, IJetReclusteringTool)
    JetReclusteringTool(std::string myname);

    // initialization - set up everything
    virtual StatusCode initialize() override;

    // execute - build jets
    virtual int execute() const override;

    // display the configuration
    virtual void print() const override;

  private:
    // hold the class name
    std::string m_APP_NAME = "JetReclusteringTool";

    // unique name for all tools, makes our life fucking easier
    std::string m_name;

  /* Properties */
    // input jet container to use as a constituent proxy
    std::string m_inputJetContainer;
    // output jet container to store reclustered jets
    std::string m_outputJetContainer;
    // radius of the reclustered jets
    float m_radius;
    // reclustering algorithm to use
    std::string m_rc_alg;
  /* variable R reclustering */
    // minimum radius
    float m_varR_minR;
    // mass scale (GeV)
    float m_varR_mass;
  /* end variable R reclustering */
    // minimum pt of the constituents (GeV)
    float m_ptMin_input;
    // minimum pt of the reclustered jets (GeV)
    float m_ptMin_rc;
    // trimming to apply to reclustered jets
    float m_ptFrac;
    float m_subjet_radius;
    // enable to add area attributes form
    bool m_doArea;
    std::string m_areaAttributes;

    // make sure someone only calls a function once
    bool m_isInitialized = false;
#if 0
  /* all tools we use */
    // this is for filtering input jets
    std::unique_ptr<JetFilterTool> m_jetFilterTool;
    std::unique_ptr<JetRecTool> m_inputJetFilterTool;
    // this is for reclustering using filtered input jets
    std::unique_ptr<PseudoJetGetter> m_pseudoJetGetterTool;
    std::unique_ptr<JetFromPseudojet> m_jetFromPseudoJetTool;
    std::unique_ptr<JetFinder> m_jetFinderTool;
    std::unique_ptr<JetRecTool> m_reclusterJetTool;
    std::unique_ptr<JetRecTool> m_trimJetTool;

    // tool for calculating effectiveR
    std::unique_ptr<EffectiveRTool> m_effectiveRTool;
    // tool for trimming reclustered jet
    std::unique_ptr<JetTrimmer> m_jetTrimmingTool;
    // modifier tools for the reclustered jets
    std::unique_ptr<JetChargeTool>              m_jetChargeTool;
    std::unique_ptr<JetPullTool>                m_jetPullTool;
    std::unique_ptr<EnergyCorrelatorTool>       m_energyCorrelatorTool;
    std::unique_ptr<EnergyCorrelatorRatiosTool> m_energyCorrelatorRatiosTool;
    std::unique_ptr<KTSplittingScaleTool>       m_ktSplittingScaleTool;
    std::unique_ptr<DipolarityTool>             m_dipolarityTool;
    std::unique_ptr<CenterOfMassShapesTool>     m_centerOfMassShapesTool;
    std::unique_ptr<NSubjettinessTool>          m_nSubjettinessTool;
#endif
    // this is for filtering input jets
    asg::AnaToolHandle<IJetModifier> m_jetFilterTool;
    asg::AnaToolHandle<IJetExecuteTool> m_inputJetFilterTool;
    // this is for reclustering using filtered input jets
    asg::AnaToolHandle<IPseudoJetGetter> m_pseudoJetGetterTool;
    asg::AnaToolHandle<IJetFromPseudojet> m_jetFromPseudoJetTool;
    asg::AnaToolHandle<IJetFinder> m_jetFinderTool;
    asg::AnaToolHandle<IJetExecuteTool> m_reclusterJetTool;
    asg::AnaToolHandle<IJetExecuteTool> m_trimJetTool;

    // tool for calculating effectiveR
    asg::AnaToolHandle<IJetModifier> m_effectiveRTool;
    // tool for trimming reclustered jet
    asg::AnaToolHandle<IJetGroomer> m_jetTrimmingTool;
    // modifier tools for the reclustered jets
    asg::AnaToolHandle<IJetModifier> m_jetChargeTool;
    asg::AnaToolHandle<IJetModifier> m_jetPullTool;
    asg::AnaToolHandle<IJetModifier> m_energyCorrelatorTool;
    asg::AnaToolHandle<IJetModifier> m_energyCorrelatorRatiosTool;
    asg::AnaToolHandle<IJetModifier> m_ktSplittingScaleTool;
    asg::AnaToolHandle<IJetModifier> m_dipolarityTool;
    asg::AnaToolHandle<IJetModifier> m_centerOfMassShapesTool;
    asg::AnaToolHandle<IJetModifier> m_nSubjettinessTool;
};  

#endif
