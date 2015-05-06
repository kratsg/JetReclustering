#ifndef Reclustering_IParticleKinematicHists_H
#define Reclustering_IParticleKinematicHists_H

#include "xAODAnaHelpers/HistogramManager.h"
//#include "xAODAnaHelpers/HelperClasses.h"
#include "xAODBase/IParticleContainer.h"

namespace Reclustering
{
  class IParticleKinematicHists : public HistogramManager
  {
    public:
      IParticleKinematicHists(std::string name);
      virtual ~IParticleKinematicHists() ;
      EL::StatusCode initialize();
      EL::StatusCode execute( const xAOD::IParticleContainer* particles, float eventWeight);
      EL::StatusCode execute( const xAOD::IParticle* particle, float eventWeight);
      using HistogramManager::book; // make other overloaded version of book() to show up in subclass
      using HistogramManager::execute; // overload

      // enable if you don't use the execute(IParticleContainer, float) call since it fills m_jetMultiplicity
      bool m_countParticles = false; // whether to count the jets in event or not
      std::string m_particleType = "particle"; // is it a jet, electron, etc...

    private:
      //basic
      TH1F* m_pt;                        //!
      TH1F* m_eta;                       //!
      TH1F* m_phi;                       //!
      TH1F* m_mass;                      //!
      TH1F* m_energy;                    //!
      TH1F* m_rapidity;                  //!

      // counting particles per event
      TH1F* m_multiplicity;              //!

  };
}
#endif
