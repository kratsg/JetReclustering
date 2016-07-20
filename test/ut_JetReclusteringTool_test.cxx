#include <xAODRootAccess/Init.h>
#include "AsgTools/AnaToolHandle.h"
#include "JetReclustering/IJetExecute.h"

using namespace asg::msgUserCode;

int main() {
  ANA_CHECK_SET_TYPE (int); //makes ANA_CHECK return ints if exiting function
#ifdef ROOTCORE
  StatusCode::enableFailure();
  ANA_CHECK (xAOD::Init ());
#endif

   asg::AnaToolHandle<IJetExecute> myTool("JetReclusteringTool/testRCJets");
   //ANA_CHECK(myTool.setProperty("InputJetContainer",  m_inputJetContainer));
   //ANA_CHECK(myTool.setProperty("OutputJetContainer", m_outputJetContainer));
   ANA_CHECK(myTool.initialize());

   ANA_CHECK( myTool.initialize() );

   //myTool->isSelected(....); put test code here

   return 0; //zero = success
}
