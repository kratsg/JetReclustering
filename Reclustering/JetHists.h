#ifndef Reclustering_JetHists_H
#define Reclustering_JetHists_H

#include "xAODAnaHelpers/HistogramManager.h"
#include "xAODJet/JetContainer.h"

namespace Reclustering
{
  class JetHists : public HistogramManager
  {
    public:
      JetHists(std::string name);
      virtual ~JetHists() ;
      EL::StatusCode initialize();
      EL::StatusCode execute( const xAOD::JetContainer* jets, float eventWeight);
      EL::StatusCode execute( const xAOD::Jet* jet, float eventWeight);
      using HistogramManager::book; // make other overloaded version of book() to show up in subclass
      using HistogramManager::execute; // overload

    private:
      TH1F* m_massOverPt;//!

      //topology
      TH1F* m_dEta;                     //!
      TH1F* m_dPhi;                     //!
      TH1F* m_asymmetry;                //!
      TH1F* m_ptRatio;                  //!
      TH1F* m_totalMass;                //!

      //substructure
      TH1F* m_tau21;                    //!
      TH1F* m_tau32;                    //!
      TH1F* m_subjet_multiplicity;      //!
      TH1F* m_subjet_ptFrac;            //!
      TH1F* m_constituents_multiplicity;//!
      TH1F* m_constituents_width;       //!
  };
}
#endif
