/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJETRECLUSTERING_ATHJETRECLUSTERINGALGO_H
#define XAODJETRECLUSTERING_ATHJETRECLUSTERINGALGO_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!

class IJetExecuteTool;

class AthJetReclusteringAlgo: public ::AthAlgorithm {
 public:
  AthJetReclusteringAlgo( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~AthJetReclusteringAlgo();

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private:
  ToolHandle<IJetExecuteTool> m_jetRecTool;
};

#endif //> !XAODJETRECLUSTERING_ATHJETRECLUSTERINGALGO_H
