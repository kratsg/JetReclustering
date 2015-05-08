#ifndef xAODJetReclustering_Helpers_H
#define xAODJetReclustering_Helpers_H


// jet reclustering
#include <fastjet/JetDefinition.hh>
#include "JetRec/JetRecTool.h"

namespace xAODJetReclustering {

  /*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*\
  |                                                                               |
  |   Author  : Giordon Stark                                                     |
  |   Email   : gstark@cern.ch                                                    |
  |   Thanks to Ben Nachman for inspiration, P-A for the help                     |
  |                                                                               |
  |   jet_reclustering():                                                         |
  |       Takes a set of small-R jets and reclusters to large-R jets              |
  |                                                                               |
  |       @inputJetContainer  : name of small-R jet container                     |
  |       @outputJetContainer : name of new jet container to record in TStore     |
  |       @radius             : radius of large-R jets                            |
  |       @rc_alg             : reclustering algorithm to use (AntiKt, Kt, CamKt) |
  |       @ptMin              : minimum Pt cut on reclustered jets                |
  |                                                                               |
  |                                                                               |
  |                                                                               |
  |                                                                               |
  \*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
    JetRecTool* JetReclusteringTool(const std::string inputJetContainer, const std::string outputJetContainer, double radius = 1.0, fastjet::JetAlgorithm rc_alg = fastjet::antikt_algorithm, float ptMin = 50000.);

}
#endif
