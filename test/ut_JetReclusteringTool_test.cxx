#include <xAODRootAccess/Init.h>
#include "AsgTools/AnaToolHandle.h"
#include "JetInterface/IJetExecuteTool.h"
#include "xAODJet/JetContainer.h"

#ifndef XAOD_STANDALONE
#include "POOLRootAccess/TEvent.h"
#endif

using namespace asg::msgUserCode;

int main() {
  ANA_CHECK_SET_TYPE (int); //makes ANA_CHECK return ints if exiting function
#ifdef ROOTCORE
  StatusCode::enableFailure();
  ANA_CHECK (xAOD::Init ());
#endif

#ifndef XAOD_STANDALONE

  //here's an example for AthAnalysisBase

  //start just by loading the first event of the test MC file
  POOL::TEvent evt(POOL::TEvent::kAthenaAccess);
  evt.readFrom("$ASG_TEST_FILE_MC");
  evt.getEntry(0);

  //example of configuring the tool
   asg::AnaToolHandle<IJetExecuteTool> myTool("JetReclusteringTool/testRCJets");
   ANA_CHECK(myTool.setProperty("InputJetContainer",  "AntiKt4LCTopoJets"));
   ANA_CHECK(myTool.setProperty("OutputJetContainer", "MyNewJets"));
   ANA_CHECK(myTool.setProperty("ReclusterRadius",    1.0));
   ANA_CHECK(myTool.setProperty("ReclusterAlgorithm", "AntiKt"));
   ANA_CHECK(myTool.setProperty("VariableRMinRadius", -1.0));
   ANA_CHECK(myTool.setProperty("VariableRMassScale", -1.0));
   ANA_CHECK(myTool.setProperty("InputJetPtMin",      25.0)); //GeV
   ANA_CHECK(myTool.setProperty("RCJetPtMin",         50.0)); //GeV
   ANA_CHECK(myTool.setProperty("RCJetPtFrac",        0.05));
   ANA_CHECK(myTool.setProperty("RCJetSubjetRadius",  0.4));
   ANA_CHECK(myTool.setProperty("DoArea",             false));
   ANA_CHECK(myTool.setProperty("AreaAttributes",     ""));
   ANA_CHECK(myTool.initialize());

   //actually run the tool: call its execute method, and check that the return value is 0
   if( myTool->execute()!=0 ) {
     ANA_MSG_INFO("Failured to recluster");
     return -1;
   }

   //if it worked, the reclustered jets will be accessible from the storegate:
   const xAOD::JetContainer* newJets = 0;
   ANA_CHECK( evt.retrieve( newJets, "MyNewJets" ) );



#endif

   return 0; //zero = success
}
