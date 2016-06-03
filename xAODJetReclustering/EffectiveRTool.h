#ifndef xAODJetReclustering_EffectiveRTool_H
#define xAODJetReclustering_EffectiveRTool_H

/// Giordon Stark \n
/// Created: May 2015
///
/// Tool to calculate EffectiveR for VariableR reclustering
/// Updated: May 2016
/// Make it compatible with AnaToolHandle

#include "AsgTools/AsgTool.h"
#include "JetInterface/ISingleJetModifier.h"
#include "JetInterface/IJetModifier.h"

class EffectiveRTool : public asg::AsgTool,
                       virtual public IJetModifier,
                       virtual public ISingleJetModifier {
  public:
    ASG_TOOL_CLASS(EffectiveRTool, ISingleJetModifier)
    // Constructor from tool name.
    EffectiveRTool(std::string myname);

    // Inherited method to modify a jet.
    // Calls width and puts the result on the jet.
    virtual int modifyJet(xAOD::Jet& jet) const;

  private:
};

#endif
