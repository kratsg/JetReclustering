#include <Reclustering/IParticleKinematicHists.h>


Reclustering::IParticleKinematicHists::IParticleKinematicHists (std::string name) :
  HistogramManager(name, "")
{
}

Reclustering::IParticleKinematicHists::~IParticleKinematicHists () {}

EL::StatusCode Reclustering::IParticleKinematicHists::initialize() {
  m_pt           = book(m_name, "pt",       m_particleType + " p_{T} [GeV]", 120, 0, 3000.);
  m_eta          = book(m_name, "eta",      m_particleType + " #eta",         80, -4, 4);
  m_phi          = book(m_name, "phi",      m_particleType + " #phi",120, -TMath::Pi(), TMath::Pi() );
  m_mass         = book(m_name, "mass",     m_particleType + " mass [GeV]",120, 0, 400);
  m_energy       = book(m_name, "energy",   m_particleType + " energy [GeV]",120, 0, 4000.);
  m_rapidity     = book(m_name, "rapidity", m_particleType + " rapidity",120, -10, 10);

  if(m_countParticles) m_multiplicity = book(m_name, "multiplicity_particles", "number of " + m_particleType + "s/event", 21, -0.5, 20);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Reclustering::IParticleKinematicHists::execute( const xAOD::IParticleContainer* particles, float eventWeight ) {
  for( const auto particle : *particles ) {
    this->execute( particle, eventWeight );
  }

  if(m_countParticles) m_multiplicity->Fill( particles->size(), eventWeight);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Reclustering::IParticleKinematicHists::execute( const xAOD::IParticle* particle, float eventWeight ) {
  //basic
  m_pt ->      Fill( particle->pt()/1e3,    eventWeight );
  m_eta->      Fill( particle->eta(),       eventWeight );
  m_phi->      Fill( particle->phi(),       eventWeight );
  m_mass->     Fill( particle->m()/1e3,     eventWeight );
  m_energy->   Fill( particle->e()/1e3,     eventWeight );
  m_rapidity-> Fill( particle->rapidity(),  eventWeight );

  return EL::StatusCode::SUCCESS;
}
