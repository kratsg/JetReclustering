#ifndef JetReclustering_EffectiveRTool_H
#define JetReclustering_EffectiveRTool_H

/// Giordon Stark \n
/// Created: May 2015
///
/// Tool to calculate EffectiveR for VariableR reclustering
/// Updated: May 2016
/// Make it compatible with AnaToolHandle

#include "JetRec/JetModifierBase.h"

class EffectiveRTool : public JetModifierBase {
  public:
    ASG_TOOL_CLASS(EffectiveRTool, IJetModifier)
    // Constructor from tool name.
    EffectiveRTool(std::string myname);

    // Inherited method to modify a jet.
    // Calls width and puts the result on the jet.
    virtual int modifyJet(xAOD::Jet& jet) const;

  private:
};

#endif
