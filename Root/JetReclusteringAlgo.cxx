#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

#include <xAODJetReclustering/JetReclusteringAlgo.h>
#include <xAODJetReclustering/Helpers.h>

#include "xAODEventInfo/EventInfo.h"

// in case we want to write the output reclustered jet collections
#include <EventLoop/OutputStream.h>

// this is needed to distribute the algorithm to the workers
ClassImp(JetReclusteringAlgo)

JetReclusteringAlgo :: JetReclusteringAlgo () {}

EL::StatusCode JetReclusteringAlgo :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init(("JetReclusteringAlgo_"+m_inputJetName).c_str()).ignore(); // call before opening first file

  if(!m_outputXAODName.empty()){
    // write an output xAOD
    EL::OutputStream output_xAOD(m_outputXAODName);
    job.outputAdd(output_xAOD);
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetReclusteringAlgo :: histInitialize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode JetReclusteringAlgo :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode JetReclusteringAlgo :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode JetReclusteringAlgo :: initialize ()
{
  Info("initialize()", "JetReclusteringAlgo_%s", m_inputJetName.c_str() );
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
    if(!m_event->writeTo(file).isSuccess()){
      Error("initialize()", "Could not set up an output file to write xAODs to.");
      return EL::StatusCode::FAILURE;
    }
  }

  // recluster 0.4 jets into 1.0 jets
  m_jetReclusteringTool = xAODJetReclustering::JetReclusteringTool(m_inputJetName, m_outputJetName, m_radius, m_clusteringAlgorithm);
  if(!m_jetReclusteringTool) return EL::StatusCode::FAILURE;

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode JetReclusteringAlgo :: execute ()
{

  m_jetReclusteringTool->execute();

  // print debugging information if needed
  if(m_debug){

    typedef xAOD::JetContainer jet_t;
    const xAOD::JetContainer* smallRjets(nullptr);
    const xAOD::JetContainer* reclusteredJets(nullptr);

    if(m_store->contains<jet_t>(m_inputJetName)){
      if(!m_store->retrieve( smallRjets, m_inputJetName ).isSuccess()) return EL::StatusCode::FAILURE;
    } else if(m_event->contains<jet_t>(m_inputJetName)){
      if(!m_event->retrieve( smallRjets, m_inputJetName ).isSuccess()) return EL::StatusCode::FAILURE;
    } else {
      Error("execute()", "Could not find the input jet container. That's fucking weird.");
      return EL::StatusCode::FAILURE;
    }


    if(m_store->contains<jet_t>(m_outputJetName)){
      if(!m_store->retrieve( reclusteredJets, m_outputJetName ).isSuccess()) return EL::StatusCode::FAILURE;
    } else if(m_event->contains<jet_t>(m_outputJetName)){
      if(!m_event->retrieve( reclusteredJets, m_outputJetName ).isSuccess()) return EL::StatusCode::FAILURE;
    } else {
      Error("execute()", "Could not find the output jet container. Did the tool execute properly? Maybe it was misconfigured.");
      return EL::StatusCode::FAILURE;
    }

    std::string printStr = "\tPt: %0.2f\tMass: %0.2f\tEta: %0.2f\tPhi: %0.2f\tNum Subjets: %zu";
    Info("execute()", "%zu small-R jets", smallRjets->size());
    for(const auto jet: *smallRjets)
      Info("execute()", printStr.c_str(), jet->pt()/1000., jet->m()/1000., jet->eta(), jet->phi(), jet->numConstituents());

    Info("execute()", "%zu reclustered jets", reclusteredJets->size());
    for(const auto jet: *reclusteredJets)
      Info("execute()", printStr.c_str(), jet->pt()/1000., jet->m()/1000., jet->eta(), jet->phi(), jet->numConstituents());
  }


  /* need to update later, must retrieve all objects include cluster sequence and pseudojets, and record them one at a time
  if(!m_outputXAODName.empty()){
    RETURN_CHECK("JetReclusteringAlgo::execute()", m_event->copy( reclusteredJets, m_outputJetName ),             ("Could not copy container to event: "+ m_outputJetName).c_str());

    // fill the file
    m_event->fill();
  }
  */

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetReclusteringAlgo :: postExecute () { return EL::StatusCode::SUCCESS; }

EL::StatusCode JetReclusteringAlgo :: finalize () {
  if(!m_outputXAODName.empty()){
    TFile *file = wk()->getOutputFile(m_outputXAODName);
    if(!m_event->finishWritingTo(file).isSuccess()){
      Error("finalize()", "Could not finish writing to file... shit.");
      return EL::StatusCode::FAILURE;
    }
  }

  // clear off the tool created
  if(!m_jetReclusteringTool) delete m_jetReclusteringTool;

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetReclusteringAlgo :: histFinalize () { return EL::StatusCode::SUCCESS; }
