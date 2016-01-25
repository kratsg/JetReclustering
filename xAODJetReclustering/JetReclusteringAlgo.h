#ifndef xAODJetReclustering_JetReclusteringAlgo_H
#define xAODJetReclustering_JetReclusteringAlgo_H

#include <EventLoop/Algorithm.h>

#include <xAODRootAccess/Init.h>
#include <xAODRootAccess/TEvent.h>
#include <xAODRootAccess/TStore.h>

// jet definition
#include <fastjet/JetDefinition.hh>

// forward-declare
class JetReclusteringTool;

class JetReclusteringAlgo : public EL::Algorithm
{
public:
  std::string m_name,
              m_inputJetContainer,
              m_outputJetContainer,
              m_rc_algName,
              m_outputXAODName;
  float m_radius = 1.0;
  float m_ptMin_input = 25.0; // GeV
  float m_ptMin_rc = 50.0; // GeV
  float m_ptFrac = 0.05;
  float m_varR_minR = -1.0;
  float m_varR_mass = -1.0; // GeV
  std::string m_area = "";
  bool m_debug = false;

private:
  /* For counting and statistics */
  xAOD::TEvent *m_event; //!
  xAOD::TStore *m_store; //!

  fastjet::JetAlgorithm m_rc_alg; //!
  JetReclusteringTool* m_jetReclusteringTool; //!

public:
  // this is a standard constructor
  JetReclusteringAlgo ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  // this is needed to distribute the algorithm to the workers
  ClassDef(JetReclusteringAlgo, 1);
};

#endif
