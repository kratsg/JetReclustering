#include <xAODRootAccess/Init.h>
#include "AsgTools/AnaToolHandle.h"
#include "JetInterface/IJetExecuteTool.h"

using namespace asg::msgUserCode;

int main() {
  ANA_CHECK_SET_TYPE (int); //makes ANA_CHECK return ints if exiting function
#ifdef ROOTCORE
  StatusCode::enableFailure();
  ANA_CHECK (xAOD::Init ());
#endif

   asg::AnaToolHandle<IJetExecuteTool> myTool("JetReclusteringTool/testRCJets");
   ANA_CHECK(myTool.setProperty("InputJetContainer",  "FakeInputContainer"));
   ANA_CHECK(myTool.setProperty("OutputJetContainer", "FakeOutputContainer"));
   ANA_CHECK(myTool.retrieve());

   //myTool->isSelected(....); put test code here

   return 0; //zero = success
}
