#ifndef xAODJetReclustering_Helpers_H
#define xAODJetReclustering_Helpers_H

#include "xAODJet/JetContainer.h"

// jet reclustering
#include <fastjet/JetDefinition.hh>
#include "JetRec/JetRecTool.h"

namespace xAODJetReclustering {

  class Helpers {

  public:
    Helpers();

    struct sort_by_pt;

    // jet reclustering
    void jet_to_pj(std::vector<fastjet::PseudoJet>& out_pj, const xAOD::JetContainer* in_jets);
    static JetRecTool* JetxAODJetReclusteringTool(const std::string inputJetContainer, const std::string outputJetContainer, double radius = 1.0, fastjet::JetAlgorithm rc_alg = fastjet::antikt_algorithm, float ptMin = 50000.);


    template<typename T>
    T sort_container_pt(T* inCont){
      T sortedCont(SG::VIEW_ELEMENTS);
      for(auto el : *inCont) sortedCont.push_back( el );

      std::sort(sortedCont.begin(), sortedCont.end(), xAODJetReclustering::Helpers::sort_by_pt());
      return sortedCont;
    }

    template<typename T>
    const T sort_container_pt(const T* inCont){
      ConstDataVector<T> sortedCont(SG::VIEW_ELEMENTS);

      for(auto el : *inCont) sortedCont.push_back( el );
      std::sort(sortedCont.begin(), sortedCont.end(), xAODJetReclustering::Helpers::sort_by_pt());
      return *sortedCont.asDataVector();
    }

  private:

  };
}
#endif
