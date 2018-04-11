/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetReclustering/EffectiveRTool.h"

EffectiveRTool::EffectiveRTool(std::string myname):
  JetModifierBase(myname)
{}

int EffectiveRTool::modifyJet(xAOD::Jet& jet) const {

  // get the effective mass scale
  float rho(0.0);
  bool rhores = jet.getAttribute("VariableRMassScale", rho);
  // only set if we could retrieve the above attribute
  //    which is only set if JetFinder does VariableR reclustering
  if(rhores)
    jet.setAttribute("EffectiveR", rho/jet.pt());
  return 0;
}
