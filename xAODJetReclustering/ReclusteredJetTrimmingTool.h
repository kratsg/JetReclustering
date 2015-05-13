#ifndef xAODJetReclustering_ReclusteredJetTrimmingTool_H
#define xAODJetReclustering_ReclusteredJetTrimmingTool_H

/// Giordon Stark \n
/// May 2015
///
/// Tool to trim reclustered jets

#include "JetRec/JetModifierBase.h"

class ReclusteredJetTrimmingTool : public JetModifierBase {
  ASG_TOOL_CLASS(ReclusteredJetTrimmingTool, IJetModifier);

  public:
    // Constructor from tool name.
    ReclusteredJetTrimmingTool(std::string myname);

    // Inherited method to modify a jet.
    // Calls width and puts the result on the jet.
    virtual int modifyJet(xAOD::Jet& jet) const;

  private:
    float m_ptFrac;
};

#endif
