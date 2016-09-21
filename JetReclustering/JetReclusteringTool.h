#ifndef JetReclustering_JetReclusteringTool_H
#define JetReclustering_JetReclusteringTool_H

// making it more like a tool
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"
#include "JetInterface/IJetExecuteTool.h"

#include <map>
#include <memory>

/*
  Author  : Giordon Stark
  Email   : gstark@cern.ch
  Thanks to Ben Nachman for inspiration, P-A for the help, Jon Burr for
  assisting in dual-use conversion.

  Takes a set of small-R jets and reclusters to large-R jets
*/

class IJetModifier;
//class IJetExecuteTool;
class IJetFromPseudojet;
class IJetFinder;
class IJetGroomer;
class IPseudoJetGetter;
class IJetPseudojetRetriever;

class JetReclusteringTool : public asg::AsgTool, virtual public IJetExecuteTool {
  public:
    ASG_TOOL_CLASS(JetReclusteringTool, IJetExecuteTool)
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
    // tool for the jpjr for m_jetTrimmingTool
    asg::AnaToolHandle<IJetPseudojetRetriever> m_jetTrimmingTool_JPJR;
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
