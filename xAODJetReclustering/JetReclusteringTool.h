#ifndef xAODJetReclustering_JetReclusteringTool_H
#define xAODJetReclustering_JetReclusteringTool_H

#include <fastjet/JetDefinition.hh>
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
#include "JetRec/PseudoJetGetter.h"
#include "JetRec/JetFromPseudojet.h"
#include "JetRec/JetFinder.h"
#include "JetRec/JetFilterTool.h"
#include "JetRec/JetRecTool.h"
// all jet modifier tools
#include "JetSubStructureMomentTools/JetChargeTool.h"
#include "JetSubStructureMomentTools/JetPullTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorRatiosTool.h"
#include "JetSubStructureMomentTools/KTSplittingScaleTool.h"
#include "JetSubStructureMomentTools/DipolarityTool.h"
#include "JetSubStructureMomentTools/CenterOfMassShapesTool.h"
#include "JetMomentTools/JetWidthTool.h"

class JetReclusteringTool {
  public:
    JetReclusteringTool(std::string myname);

    // initialization - set up everything
    bool initialize();

    // execute - build jets
    void execute() const;

    // display the configuration
    void print() const;

  /* Properties */
    // input jet container to use as a constituent proxy
    std::string m_inputJetContainer;
    // output jet container to store reclustered jets
    std::string m_outputJetContainer;
    // radius of the reclustered jets
    float m_radius                  = 1.0;
    // reclustering algorithm to use
    fastjet::JetAlgorithm m_rc_alg  = fastjet::antikt_algorithm;
    // minimum pt of the constituents (GeV)
    float m_ptMin_input             = 25.0;
    // minimum pt of the reclustered jets (GeV)
    float m_ptMin_rc                = 50.0;
    // trimming to apply to reclustered jets
    float m_ptFrac                  = 0.05;

  private:
    // hold the class name
    std::string m_APP_NAME = "JetReclusteringTool";

    // unique name for all tools, makes our life fucking easier
    std::string m_name;

    // make sure someone only calls a function once
    bool m_isInitialized = false;

    // we have to convert the fastjet algorithm to a named algorithm because
    // the tools are fucking stupid
    std::map<fastjet::JetAlgorithm, std::string> algToAlgName = {
            {fastjet::kt_algorithm, "Kt"},
            {fastjet::cambridge_algorithm, "CamKt"},
            {fastjet::antikt_algorithm, "AntiKt"}
    };

  /* all tools we use */
    // this is for filtering input jets
    std::unique_ptr<JetFilterTool> m_jetFilterTool;
    std::unique_ptr<JetRecTool> m_inputJetFilterTool;
    // this is for reclustering using filtered input jets
    std::unique_ptr<PseudoJetGetter> m_pseudoJetGetterTool;
    std::unique_ptr<JetFromPseudojet> m_jetFromPseudoJetTool;
    std::unique_ptr<JetFinder> m_jetFinderTool;
    std::unique_ptr<JetRecTool> m_reclusterJetTool;

    // modifier tools for the reclustered jets
    std::unique_ptr<JetChargeTool>              m_jetChargeTool;
    std::unique_ptr<JetPullTool>                m_jetPullTool;
    std::unique_ptr<EnergyCorrelatorTool>       m_energyCorrelatorTool;
    std::unique_ptr<EnergyCorrelatorRatiosTool> m_energyCorrelatorRatiosTool;
    std::unique_ptr<KTSplittingScaleTool>       m_ktSplittingScaleTool;
    std::unique_ptr<DipolarityTool>             m_dipolarityTool;
    std::unique_ptr<CenterOfMassShapesTool>     m_centerOfMassShapesTool;
    std::unique_ptr<JetWidthTool>               m_jetWidthTool;

};

#endif
