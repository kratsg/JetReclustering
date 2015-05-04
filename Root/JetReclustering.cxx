#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

#include <Reclustering/JetReclustering.h>
#include <Reclustering/Helpers.h>

#include "xAODEventInfo/EventInfo.h"

// for making new containers
#include "xAODJet/JetAuxContainer.h"

#include "xAODAnaHelpers/tools/ReturnCheck.h"
#include "xAODAnaHelpers/HelperFunctions.h"

// in case we want to write the output reclustered jet collections
#include <EventLoop/OutputStream.h>

// this is needed to distribute the algorithm to the workers
ClassImp(JetReclustering)

JetReclustering :: JetReclustering () {}

EL::StatusCode JetReclustering :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init(("JetReclustering_"+m_inputJetName).c_str()).ignore(); // call before opening first file

  if(!m_outputXAODName.empty()){
    // write an output xAOD
    EL::OutputStream output_xAOD(m_outputXAODName);
    job.outputAdd(output_xAOD);
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetReclustering :: histInitialize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode JetReclustering :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode JetReclustering :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode JetReclustering :: initialize ()
{
  Info("initialize()", "JetReclustering_%s", m_inputJetName.c_str() );
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  std::map<std::string, fastjet::JetAlgorithm> algNameToAlg = {{"kt_algorithm", fastjet::kt_algorithm}, {"cambridge_algorithm", fastjet::cambridge_algorithm}, {"antikt_algorithm", fastjet::antikt_algorithm}};
  if(!m_clusteringAlgorithmName.empty()){
    if(!algNameToAlg.count(m_clusteringAlgorithmName)){
      Error("setupJob()", "Only `kt_algorithm`, `cambridge_algorithm`, and `antikt_algorithm` are supported!");
      return EL::StatusCode::FAILURE;
    }
    m_clusteringAlgorithm = algNameToAlg.at(m_clusteringAlgorithmName);
  } else {
    Info("setupJob()", "m_clusteringAlgorithmName is empty. Setting to `antikt_algorithm` by default.");
    m_clusteringAlgorithm = algNameToAlg.at("antikt_algorithm");
  }

  if(!m_outputXAODName.empty()){
    TFile *file = wk()->getOutputFile(m_outputXAODName);
    RETURN_CHECK("JetReclustering::execute()", m_event->writeTo(file), "");
  }


  return EL::StatusCode::SUCCESS;
}


EL::StatusCode JetReclustering :: execute ()
{
  const xAOD::EventInfo* eventInfo(nullptr);
  RETURN_CHECK("JetReclustering::execute()", HelperFunctions::retrieve( eventInfo, "EventInfo", m_event, 0, false), "Could not retrieve EventInfo object.");

  // grab the small radius jets
  const xAOD::JetContainer* smallRjets(nullptr);
  RETURN_CHECK("JetReclustering::execute()", HelperFunctions::retrieve( smallRjets, m_inputJetName, m_event, m_store, false), ("Could not retrieve smallRjet object: "+m_inputJetName).c_str());

  // set up the helpers
  static Reclustering::Helpers helpers;

  // set up the new containers to record the new jets
  xAOD::JetContainer* reclusteredJets = new xAOD::JetContainer();
  xAOD::JetAuxContainer* reclusteredJetsAux = new xAOD::JetAuxContainer;
  reclusteredJets->setStore(reclusteredJetsAux);

  // recluster 0.4 jets into 1.0 jets
  helpers.jet_reclustering(*reclusteredJets, smallRjets, m_radius, m_clusteringAlgorithm);

  // print debugging information if needed
  if(m_debug){
    std::string printStr = "\tPt: %0.2f\tMass: %0.2f\tEta: %0.2f\tPhi: %0.2f\tNum Subjets: %zu";
    Info("execute()", "%zu small-R jets", smallRjets->size());
    for(const auto jet: *smallRjets)
      Info("execute()", printStr.c_str(), jet->pt()/1000., jet->m()/1000., jet->eta(), jet->phi(), jet->numConstituents());

    Info("execute()", "%zu reclustered jets", reclusteredJets->size());
    for(const auto jet: *reclusteredJets)
      Info("execute()", printStr.c_str(), jet->pt()/1000., jet->m()/1000., jet->eta(), jet->phi(), jet->numConstituents());
  }

  // record in the store
  RETURN_CHECK("JetReclustering::execute()", m_store->record( reclusteredJets, m_outputJetName ),               ("Could not record container to store: "+ m_outputJetName).c_str());
  RETURN_CHECK("JetReclustering::execute()", m_store->record( reclusteredJetsAux, m_outputJetName + "Aux."),    ("Could not record aux container to store: "+ m_outputJetName+"Aux.").c_str());

  if(!m_outputXAODName.empty()){
    RETURN_CHECK("JetReclustering::execute()", m_event->record( reclusteredJets, m_outputJetName ),             ("Could not record container to event: "+ m_outputJetName).c_str());
    RETURN_CHECK("JetReclustering::execute()", m_event->record( reclusteredJetsAux, m_outputJetName + "Aux."),  ("Could not record aux container to event: "+ m_outputJetName+"Aux.").c_str());

    // fill the file
    m_event->fill();
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetReclustering :: postExecute () { return EL::StatusCode::SUCCESS; }

EL::StatusCode JetReclustering :: finalize () {
  if(!m_outputXAODName.empty()){
    TFile *file = wk()->getOutputFile(m_outputXAODName);
    RETURN_CHECK("JetReclustering::finalize()", m_event->finishWritingTo( file ), "Could not finish writing to file.");
  }
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetReclustering :: histFinalize () { return EL::StatusCode::SUCCESS; }
