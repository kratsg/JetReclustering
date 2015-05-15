#ifndef xAODJetReclustering_EffectiveRTool_H
#define xAODJetReclustering_EffectiveRTool_H

/// Giordon Stark \n
/// May 2015
///
/// Tool to calculate EffectiveR for VariableR reclustering

#include "JetRec/JetModifierBase.h"

class EffectiveRTool : public JetModifierBase {
  ASG_TOOL_CLASS(EffectiveRTool, IJetModifier)

  public:
    // Constructor from tool name.
    EffectiveRTool(std::string myname);

    // Inherited method to modify a jet.
    // Calls width and puts the result on the jet.
    virtual int modifyJet(xAOD::Jet& jet) const;

  private:
};

#endif
