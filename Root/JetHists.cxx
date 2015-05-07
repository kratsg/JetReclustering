#include <Reclustering/JetHists.h>

// jet definition
#include <fastjet/JetDefinition.hh>

// subjet finding
#include "JetSubStructureUtils/SubjetFinder.h"

Reclustering::JetHists::JetHists (std::string name) :
  HistogramManager(name, "")
{
}

Reclustering::JetHists::~JetHists () {}

EL::StatusCode Reclustering::JetHists::initialize() {
  m_massOverPt  = book(m_name, "MOverPt", "#frac{m^{jet}}{p_{T}}", 100, 0, 5);

  //topology
  m_dEta        = book(m_name, "j1j2/dEta", "|#Delta#eta(j^{1}, j^{2})|", 80, 0, 8);
  m_dPhi        = book(m_name, "j1j2/dPhi", "|#Delta#phi(j^{1}, j^{2})|", 120, 0, 2*TMath::Pi() );
  m_asymmetry   = book(m_name, "j1j2/asymmetry", "#frac{p_{T}^{1} - p_{T}^{2}}{p_{T}^{1} + p_{T}^{2}}", 100, 0, 1);
  m_ptRatio     = book(m_name, "j1j3/ptRatio", "#frac{p_{T}^{3}}{p_{T}^{1}}", 100, 0, 1);
  m_totalMass   = book(m_name, "sumJetM", "#Sigma m^{jet}", 120, 0, 3000.);

  //substructure
    //subjettiness
  m_tau21                     = book(m_name, "tau21", "#Tau_{21}", 120, 0, 1.2);
  m_tau32                     = book(m_name, "tau32", "#Tau_{32}", 120, 0, 1.2);
    //subjet info
  m_subjet_multiplicity       = book(m_name, "subjets/multiplicity", "N_{subjet}", 21, -0.5, 20.5);
  m_subjet_ptFrac             = book(m_name, "subjets/ptFrac", "p_{T}^{subjet}/p_{T}^{jet}", 100, 0, 1);
    //constituent info
  m_constituents_multiplicity = book(m_name, "constituents/multiplicity", "N_{constituents}", 61, -0.5, 60.5);
  m_constituents_width        = book(m_name, "width", "#frac{1}{p_{T}^{jet}} #Sigma_{i}^{constit} p_{T,i}^{constit} #cdot #Delta R(constit, jet)", 100, 0, 2);
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Reclustering::JetHists::execute( const xAOD::JetContainer* jets, float eventWeight ) {

  float totalMass(0);
  for(const auto jet: *jets){
    totalMass += jet->m();
    this->execute(jet, eventWeight);
  }

  const xAOD::Jet* j1(nullptr);
  const xAOD::Jet* j2(nullptr);
  const xAOD::Jet* j3(nullptr);

  if(jets->size() > 0){
    j1 = jets->at(0);
    if(jets->size() > 1){
      j2 = jets->at(1);
      m_dEta->      Fill( fabs(j1->eta() - j2->eta()),                  eventWeight );
      m_dPhi->      Fill( fabs(j1->p4().DeltaPhi( j2->p4() )),          eventWeight );
      m_asymmetry-> Fill( (j1->pt() - j2->pt())/(j1->pt() + j2->pt()),  eventWeight );
      if(jets->size() > 2){
        j3 = jets->at(2);
        m_ptRatio->  Fill( j3->pt()/j1->pt() );
      }
    }
  }
  m_totalMass->     Fill( totalMass/1000., eventWeight );

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Reclustering::JetHists::execute( const xAOD::Jet* jet, float eventWeight ){
  static SG::AuxElement::ConstAccessor<float> Tau1("Tau1");
  static SG::AuxElement::ConstAccessor<float> Tau2("Tau2");
  static SG::AuxElement::ConstAccessor<float> Tau3("Tau3");
  static SG::AuxElement::ConstAccessor<float> Width("Width");

  m_massOverPt->Fill( jet->m()/jet->pt(), eventWeight );

  // subjettiness
  if(Tau1.isAvailable(*jet) && Tau2.isAvailable(*jet)) m_tau21->Fill( Tau2(*jet)/Tau1(*jet), eventWeight);
  if(Tau2.isAvailable(*jet) && Tau3.isAvailable(*jet)) m_tau32->Fill( Tau3(*jet)/Tau2(*jet), eventWeight);

  // default to using the kt_algorithm and 0.2 radius
  fastjet::JetAlgorithm subjet_clustering(fastjet::kt_algorithm);
  float subjet_radius(0.2);
  // if it is a transformed jet, deal with that
  static SG::AuxElement::ConstAccessor<int> TransformType("TransformType");
  if(TransformType.isAvailable(*jet)){
    static SG::AuxElement::ConstAccessor<float> RClus("RClus");
    switch(TransformType(*jet)){
      case xAOD::JetTransform::Trim:
        subjet_clustering = fastjet::kt_algorithm;
        subjet_radius     = RClus(*jet);
      break;
      case xAOD::JetTransform::MassDrop:
        subjet_clustering = fastjet::cambridge_algorithm;
        subjet_radius     = RClus(*jet);
      break;
      default:
        //case xAOD::JetTransform::Prune:
        //use defaults
      break;
    }
  }
  // else, use defaults

  JetSubStructureUtils::SubjetFinder subjetFinder(subjet_clustering, subjet_radius, 0.0);
  std::vector<fastjet::PseudoJet> subjets = subjetFinder.result(*jet);
  for(auto subjet: subjets) m_subjet_ptFrac->Fill( subjet.pt()/jet->pt(), eventWeight);
  m_subjet_multiplicity->Fill( subjets.size(), eventWeight);

  m_constituents_multiplicity->Fill( jet->numConstituents(), eventWeight);
  if(Width.isAvailable(*jet)) m_constituents_width->Fill( Width(*jet), eventWeight);

  return EL::StatusCode::SUCCESS;
}
