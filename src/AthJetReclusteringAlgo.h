#ifndef XAODJETRECLUSTERING_ATHJETRECLUSTERINGALGO_H
#define XAODJETRECLUSTERING_ATHJETRECLUSTERINGALGO_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!

class IJetReclusteringTool;

class AthJetReclusteringAlgo: public ::AthAlgorithm { 
 public: 
  AthJetReclusteringAlgo( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~AthJetReclusteringAlgo(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 
  ToolHandle<IJetReclusteringTool> m_jetRecTool;
}; 

#endif //> !XAODJETRECLUSTERING_ATHJETRECLUSTERINGALGO_H
