/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ROOTCORE
#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

#include <JetReclustering/JetReclusteringAlgo.h>
#include <JetReclustering/JetReclusteringTool.h>

#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"

// in case we want to write the output reclustered jet collections
#include <EventLoop/OutputStream.h>

// ANA_CHECK macro
#include <AsgTools/MessageCheck.h>

// this is needed to distribute the algorithm to the workers
ClassImp(JetReclusteringAlgo)

JetReclusteringAlgo :: JetReclusteringAlgo () :
  m_jetReclusteringTool("JetReclusteringTool/"+m_name)
{}

EL::StatusCode JetReclusteringAlgo :: setupJob (EL::Job& job)
{
  ANA_CHECK_SET_TYPE (EL::StatusCode);
  job.useXAOD();
  ANA_CHECK(xAOD::Init()); // call before opening first file

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
  ANA_CHECK_SET_TYPE (EL::StatusCode);
  Info("initialize()", "JetReclusteringAlgo_%s", m_inputJetContainer.c_str() );
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  if(m_rc_alg.empty()){
    Info("setupJob()", "m_rc_alg is empty. Setting to `AntiKt` by default.");
    m_rc_alg = "AntiKt";
  }

  if(!m_outputXAODName.empty()){
    TFile *file = wk()->getOutputFile(m_outputXAODName);
    if(!m_event->writeTo(file).isSuccess()){
      Error("initialize()", "Could not set up an output file to write xAODs to.");
      return EL::StatusCode::FAILURE;
    }
  }

  // m_name needs to be set and unique or we have conflicts with multiple tools
  if(m_name.empty()){
    Error("initialize()", "m_name needs to be set and unique.");
    return EL::StatusCode::FAILURE;
  }

  ANA_CHECK(ASG_MAKE_ANA_TOOL(m_jetReclusteringTool, JetReclusteringTool));
  ANA_CHECK(m_jetReclusteringTool.setProperty("InputJetContainer",  m_inputJetContainer));
  ANA_CHECK(m_jetReclusteringTool.setProperty("OutputJetContainer", m_outputJetContainer));
  ANA_CHECK(m_jetReclusteringTool.setProperty("ReclusterRadius",    m_radius));
  ANA_CHECK(m_jetReclusteringTool.setProperty("ReclusterAlgorithm", m_rc_alg));
  ANA_CHECK(m_jetReclusteringTool.setProperty("VariableRMinRadius", m_varR_minR));
  ANA_CHECK(m_jetReclusteringTool.setProperty("VariableRMassScale", m_varR_mass));
  ANA_CHECK(m_jetReclusteringTool.setProperty("InputJetPtMin",      m_ptMin_input));
  ANA_CHECK(m_jetReclusteringTool.setProperty("RCJetPtMin",         m_ptMin_rc));
  ANA_CHECK(m_jetReclusteringTool.setProperty("RCJetPtFrac",        m_ptFrac));
  ANA_CHECK(m_jetReclusteringTool.setProperty("RCJetSubjetRadius",  m_subjet_radius));
  ANA_CHECK(m_jetReclusteringTool.setProperty("DoArea",             m_doArea));
  ANA_CHECK(m_jetReclusteringTool.setProperty("AreaAttributes",     m_areaAttributes));
  ANA_CHECK(m_jetReclusteringTool.retrieve());

  if(m_debug) m_jetReclusteringTool->print();

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode JetReclusteringAlgo :: execute ()
{
  //ANA_CHECK_SET_TYPE (EL::StatusCode);
  m_jetReclusteringTool->execute();

  // print debugging information if needed
  if(m_debug){
    typedef xAOD::JetContainer jet_t;
    const xAOD::JetContainer* smallRjets(nullptr);
    const xAOD::JetContainer* reclusteredJets(nullptr);

    if(m_store->contains<jet_t>(m_inputJetContainer)){
      if(!m_store->retrieve( smallRjets, m_inputJetContainer ).isSuccess()) return EL::StatusCode::FAILURE;
    } else if(m_event->contains<jet_t>(m_inputJetContainer)){
      if(!m_event->retrieve( smallRjets, m_inputJetContainer ).isSuccess()) return EL::StatusCode::FAILURE;
    } else {
      Error("execute()", "Could not find the input jet container. That's really weird.");
      return EL::StatusCode::FAILURE;
    }


    if(m_store->contains<jet_t>(m_outputJetContainer)){
      if(!m_store->retrieve( reclusteredJets, m_outputJetContainer ).isSuccess()) return EL::StatusCode::FAILURE;
    } else if(m_event->contains<jet_t>(m_outputJetContainer)){
      if(!m_event->retrieve( reclusteredJets, m_outputJetContainer ).isSuccess()) return EL::StatusCode::FAILURE;
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
    ANA_CHECK(m_event->copy( reclusteredJets, m_outputJetContainer ));

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

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JetReclusteringAlgo :: histFinalize () { return EL::StatusCode::SUCCESS; }
#endif // ROOTCORE
