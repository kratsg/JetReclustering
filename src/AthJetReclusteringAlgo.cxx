/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetReclustering includes
#include "AthJetReclusteringAlgo.h"

#include "JetInterface/IJetExecuteTool.h"

AthJetReclusteringAlgo::AthJetReclusteringAlgo( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAlgorithm( name, pSvcLocator ),
    m_jetRecTool("JetReclusteringTool/"+this->name()+"JetReclusteringTool")
{
  declareProperty( "JetReclusteringTool", m_jetRecTool );
}


AthJetReclusteringAlgo::~AthJetReclusteringAlgo() {}


StatusCode AthJetReclusteringAlgo::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  ATH_CHECK( m_jetRecTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode AthJetReclusteringAlgo::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode AthJetReclusteringAlgo::execute() {
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  int retCode = m_jetRecTool->execute();
  if (retCode != 0) {
    ATH_MSG_ERROR( "JetReclusteringTool failed in execution with code: " << retCode );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


