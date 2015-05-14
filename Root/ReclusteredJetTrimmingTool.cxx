#include "xAODJetReclustering/ReclusteredJetTrimmingTool.h"

#include "xAODJet/JetConstituentVector.h"
#include "xAODJet/JetContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "AthLinks/ElementLink.h"

ReclusteredJetTrimmingTool::ReclusteredJetTrimmingTool(std::string myname):
  JetModifierBase(myname)
{
  declareProperty("PtFrac", m_ptFrac = 0.05);
}

int ReclusteredJetTrimmingTool::modifyJet(xAOD::Jet& jet) const {

  // in this case we can access directly the raw
  // constituents (i.e. small R jets)
  std::vector<const xAOD::Jet*> filteredConstits;

  for(auto constit: jet.getConstituents())
    if(constit->pt() / jet.pt() > m_ptFrac)
      filteredConstits.push_back(static_cast<const xAOD::Jet*>(constit->rawConstituent()));

  // Access directly the internal links and clear it
  static SG::AuxElement::Accessor< std::vector< ElementLink< xAOD::IParticleContainer > > >  constituentAcc( "constituentLinks" );
  constituentAcc( jet ).clear();

  xAOD::JetFourMom_t totalJetP4;
  // re-add the filtered constituents with weight=1.0
  for( auto constit: filteredConstits){
    jet.addConstituent(constit, 1.0);
    totalJetP4 += constit->jetP4();
  }

  jet.setJetP4( totalJetP4 );
  return 0;
}
