#ifndef Reclustering_StandardKinematics_H
#define Reclustering_StandardKinematics_H

#include <EventLoop/Algorithm.h>

// Infrastructure include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

// plots to include
#include <Reclustering/IParticleKinematicHists.h>
#include <Reclustering/JetHists.h>

class StandardKinematics : public EL::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.
public:
  int m_numLeadingJets = 0;

  // standard across all algorithms for configuration
  bool m_debug = false;
  std::string m_eventInfo       = "EventInfo",
              m_inputJets       = "";

private:
  xAOD::TEvent *m_event;  //!
  xAOD::TStore *m_store;  //!

  std::map< std::string, Reclustering::IParticleKinematicHists* > m_jetPlots; //!
  std::map< std::string, Reclustering::JetHists* > m_jetHistsPlots; //!


public:
  // this is a standard constructor
  StandardKinematics ();
  StandardKinematics (std::string name, std::string configName);

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
  ClassDef(StandardKinematics, 1);
};

#endif
