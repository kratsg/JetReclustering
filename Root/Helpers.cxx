#include "Reclustering/Helpers.h"

// for templating
#include "xAODBase/IParticleContainer.h"

// jet reclustering
#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>

// use this to set up constituents
#include <JetRec/JetFromPseudojet.h>

Reclustering :: Helpers :: Helpers ()
{
}


struct Reclustering :: Helpers :: sort_by_pt
{

    inline bool operator() (const TLorentzVector& lhs, const TLorentzVector& rhs)
    {
      return (lhs.Pt() > rhs.Pt());
    }

    inline bool operator() (const TLorentzVector* lhs, const TLorentzVector* rhs)
    {
      return (lhs->Pt() > rhs->Pt());
    }

    inline bool operator() (const xAOD::IParticle& lhs, const xAOD::IParticle& rhs)
    {
      return (lhs.pt() > rhs.pt());
    }

    inline bool operator() (const xAOD::IParticle* lhs, const xAOD::IParticle* rhs)
    {
      return (lhs->pt() > rhs->pt());
    }
};

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*\
|                                                                              |
|   Author  : Giordon Stark                                                    |
|   Email   : gstark@cern.ch                                                   |
|   Thanks to Ben Nachman for inspiration                                      |
|                                                                              |
|   jet_reclustering():                                                        |
|       Takes a set of small-R jets and reclusters to large-R jets             |
|                                                                              |
|       @jets   : jet container to recluster and trim                          |
|       @radius : radius of large-R jet                                        |
|       @fcut   : trimming cut to apply                                        |
|       @rc_alg : clustering algorithm                                         |
|                                                                              |
|                                                                              |
\*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
void Reclustering::Helpers::jet_to_pj(std::vector<fastjet::PseudoJet>& out_pj, const xAOD::JetContainer* in_jets){
  for(auto it = in_jets->begin(); it != in_jets->end(); ++it){
    const TLorentzVector jet_p4 = (*it)->p4();
    fastjet::PseudoJet pj(jet_p4.Px(),
                          jet_p4.Py(),
                          jet_p4.Pz(),
                          jet_p4.E ());
    pj.set_user_index(std::distance(in_jets->begin(), it));
    out_pj.push_back(pj);
  }
  return;
}

void Reclustering::Helpers::jet_reclustering(xAOD::JetContainer& out_jets, const xAOD::JetContainer* in_jets, double radius, fastjet::JetAlgorithm rc_alg){

  std::map<fastjet::JetAlgorithm, std::string> algToAlgName = {{fastjet::kt_algorithm, "kt_algorithm"}, {fastjet::cambridge_algorithm, "cambridge_algorithm"}, {fastjet::antikt_algorithm, "antikt_algorithm"}};

  //1. Need to convert the vector of jets to a vector of pseudojets; only need p4() since we're using them as inputs
  std::vector<fastjet::PseudoJet> input_jets;
  Reclustering::Helpers::jet_to_pj(input_jets, in_jets);

  //2. Build up the new jet definitions using input configurations: jet algorithm, radius
  fastjet::JetDefinition jet_def(rc_alg, radius);

  //3. Run the Cluster Sequence on pseudojets with the right jet definition above; cs = clustersequence
  fastjet::ClusterSequence cs(input_jets, jet_def);

  // 4. Grab the reclustered jets, sorted by pt(); rc_jets == reclustered jets
  std::vector<fastjet::PseudoJet> rc_jets = fastjet::sorted_by_pt(cs.inclusive_jets());

  JetFromPseudojet* pj2j_tool = new JetFromPseudojet("JetFromPseudoJetTool");
  //pj2j_tool->msg().setLevel( MSG::DEBUG );

  // figure out the correct attributes
  const xAOD::JetInput::Type input_type = in_jets->at(0)->getInputType();
  xAOD::JetTransform::Type transform_type(xAOD::JetTransform::UnknownTransform);
  switch(rc_alg){
    case fastjet::antikt_algorithm:
      transform_type = xAOD::JetTransform::AntiKtRecluster;
    break;
    case fastjet::kt_algorithm:
      transform_type = xAOD::JetTransform::KtRecluster;
    break;
    case fastjet::cambridge_algorithm:
      transform_type = xAOD::JetTransform::CamKtRecluster;
    break;
    default:
      transform_type = xAOD::JetTransform::UnknownTransform;
    break;
  }

  // set our attributes on the jets
  for(auto rc_jet: rc_jets){
    xAOD::Jet* jet_from_pj = pj2j_tool->add(rc_jet, out_jets, nullptr);
    jet_from_pj->setInputType(input_type); // LCTopo, EMTopo, ... based on small-R jets
    jet_from_pj->setAlgorithmType(xAOD::JetAlgorithmType::undefined_jet_algorithm); // not sure...
    jet_from_pj->setSizeParameter(radius); // large-R jet size
    jet_from_pj->auxdecor<int>("TransformType") = transform_type; // basically how we re-clustered

    // calculate substructure values using constituents
    float width(0.0);
    for(auto con: rc_jet.constituents()){
      const xAOD::Jet* constitJet = in_jets->at(con.user_index());
      jet_from_pj->addConstituent(constitJet);
      width += constitJet->pt() * jet_from_pj->p4().DeltaR(constitJet->p4());
    }
    jet_from_pj->auxdecor<float>("Width") = width/jet_from_pj->pt();
  }

  delete pj2j_tool;

  return;
}
