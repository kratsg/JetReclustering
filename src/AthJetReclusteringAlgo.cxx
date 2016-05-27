// xAODJetReclustering includes
#include "AthJetReclusteringAlgo.h"

#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

#include "xAODJetReclustering/IJetReclusteringTool.h"

AthJetReclusteringAlgo::AthJetReclusteringAlgo( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAlgorithm( name, pSvcLocator ),
    m_jetRecTool("JetReclusteringTool/"+name+"JetRecTool", this)
{
  declareProperty( "JetRecTool", m_jetRecTool );
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
    ATH_MSG_ERROR( "JetRecTool failed in execution with code: " << retCode );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


