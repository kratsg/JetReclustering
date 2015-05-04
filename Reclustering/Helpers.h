#ifndef Reclustering_Helpers_H
#define Reclustering_Helpers_H

#include "xAODJet/JetContainer.h"

// jet reclustering
#include <fastjet/JetDefinition.hh>

namespace Reclustering {

  class Helpers {

  public:
    Helpers();

    struct sort_by_pt;

    // jet reclustering and trimming
    void jet_to_pj(std::vector<fastjet::PseudoJet>& out_pj, const xAOD::JetContainer* in_jets);
    void jet_reclustering(xAOD::JetContainer& out_jets, const xAOD::JetContainer* in_jets, double radius = 1.0, fastjet::JetAlgorithm rc_alg = fastjet::antikt_algorithm);

    template<typename T>
    T sort_container_pt(T* inCont){
      T sortedCont(SG::VIEW_ELEMENTS);
      for(auto el : *inCont) sortedCont.push_back( el );

      std::sort(sortedCont.begin(), sortedCont.end(), Reclustering::Helpers::sort_by_pt());
      return sortedCont;
    }

    template<typename T>
    const T sort_container_pt(const T* inCont){
      ConstDataVector<T> sortedCont(SG::VIEW_ELEMENTS);

      for(auto el : *inCont) sortedCont.push_back( el );
      std::sort(sortedCont.begin(), sortedCont.end(), Reclustering::Helpers::sort_by_pt());
      return *sortedCont.asDataVector();
    }

  private:

  };
}
#endif
