#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <Reclustering/StandardKinematics.h>

// Infrastructure includes
#include "xAODRootAccess/Init.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

// xAH includes
#include "xAODAnaHelpers/HelperFunctions.h"
#include "xAODAnaHelpers/tools/ReturnCheck.h"

namespace HF = HelperFunctions;

// this is needed to distribute the algorithm to the workers
ClassImp(StandardKinematics)

StandardKinematics :: StandardKinematics () {}

EL::StatusCode StandardKinematics :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init("StandardKinematics").ignore();

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode StandardKinematics :: histInitialize () {
  // initialize all histograms here

  m_jetPlots[m_inputJets+"/jets"]          = new TheAccountant::IParticleKinematicHists( m_inputJets+"/jets/" );
  m_jetHistsPlots[m_inputJets+"/jets"]          = new TheAccountant::JetHists( m_inputJets+"/jets/" );

  // enable jet counting for jet plots above, set type to jet
  for(auto jetPlot: m_jetPlots){
    jetPlot.second->m_countParticles = true;
    jetPlot.second->m_particleType   = "jet";
  }

  // NLeadingJets
  for(int i=1; i <= m_numLeadingJets; ++i){
    //all/jetX
    m_jetPlots[m_inputJets+"/jet"+std::to_string(i)] = new TheAccountant::IParticleKinematicHists( m_inputJets+"/jet"+std::to_string(i)+"/" );
  }

  for(auto jetPlot: m_jetPlots){
    jetPlot.second->initialize();
    jetPlot.second->record( wk() );
  }

  for(auto jetHistsPlot: m_jetHistsPlots){
    jetHistsPlot.second->initialize();
    jetHistsPlot.second->record( wk() );
  }

  return EL::StatusCode::SUCCESS;
}
EL::StatusCode StandardKinematics :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode StandardKinematics :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode StandardKinematics :: initialize ()
{
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode StandardKinematics :: execute ()
{
  const xAOD::EventInfo*                eventInfo   (nullptr);
  const xAOD::JetContainer*             in_jets     (nullptr);

  // start grabbing all the containers that we can
  RETURN_CHECK("Preselect::execute()", HF::retrieve(eventInfo,    m_eventInfo,        m_event, m_store, m_debug), "Could not get the EventInfo container.");
  RETURN_CHECK("Preselect::execute()", HF::retrieve(in_jets,      m_inputJets,        m_event, m_store, m_debug), "Could not get the inputJets container.");

  static SG::AuxElement::Accessor< float > decor_eventWeight("eventWeight");

  float eventWeight(1);
  if( decor_eventWeight.isAvailable(*eventInfo) ) eventWeight = decor_eventWeight(*eventInfo);

  // standard all jets and all bjets
  if(m_jetPlots[m_inputJets+"/jets"]->execute(in_jets, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
  if(m_jetHistsPlots[m_inputJets+"/jets"]->execute(in_jets, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;

  //all/jetX
  for(int i=1; i <= std::min<int>( m_numLeadingJets, in_jets->size() ); ++i ){
    if(m_jetPlots[m_inputJets+"/jet"+std::to_string(i)]->execute(in_jets->at(i-1), eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode StandardKinematics :: postExecute () { return EL::StatusCode::SUCCESS; }

EL::StatusCode StandardKinematics :: finalize () {
  for( auto jetPlot : m_jetPlots ) {
    if(jetPlot.second) delete jetPlot.second;
  }

  for( auto jetHistsPlot: m_jetHistsPlots ){
    if(jetHistsPlot.second) delete jetHistsPlot.second;
  }
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode StandardKinematics :: histFinalize () { return EL::StatusCode::SUCCESS; }
