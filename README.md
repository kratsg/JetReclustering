# xAODJetReclustering - A RootCore Package

## Installing
The last stable analysis base used is **2.1.30**. To install,
```bash
git clone https://github.com/kratsg/xAODJetReclustering.git
rc checkout_pkg atlasoff/Reconstruction/Jet/JetSubStructureUtils/tags/JetSubStructureUtils-00-02-08
rc checkout_pkg atlasoff/Reconstruction/Jet/JetSubStructureMomentTools/tags/JetSubStructureMomentTools-00-01-14
rc find_packages
rc compile
```

## Configurations for `JetReclusteringAlgo` algorithm

Variable | Type | Default | Description
---------|------|---------|-------------
m_inputJetContainer | std::string | | name of the input jet container for reclustering
m_ptMin_input | float | 25.0 | filter input jets by requiring a minimum pt cut
m_outputJetContainer | std::string | | name of the output jet container holding reclustered jets
m_rc_algName | std::string | antikt_algorithm | how to recluster the input jets
m_radius | float | 1.0 | radius of large-R reclustered jets
m_ptMin_rc | float | 50.0 | filter reclustered jets by requiring a minimum pt cut
m_outputXAODName | std::string | | if defined, put the reclustered jets in an output xAOD file of the given name
m_debug | bool | false | enable verbose debugging information

## Using xAOD Jet Reclustering

### Incorporating in existing code

If you wish to incorporate `JetReclustering` directly into your code, add this package as a dependency in `cmt/Makefile.RootCore` and then a header

```c++
#include <xAODJetReclustering/JetReclusteringTool.h>
```

to get started. At this point, you can set up your standard tool in the `initialize()` portion of your algorithm as a pointer

```c++
m_jetReclusteringTool = new JetReclusteringTool(m_name);
m_jetReclusteringTool->m_inputJetContainer = "AntiKt4LCTopoJets";
m_jetReclusteringTool->m_outputJetContainer = "AntiKt10LCTopoJetsRCAntiKt4LCTopoJets";
m_jetReclusteringTool->m_radius = 1.0;
m_jetReclusteringTool->m_name = "R10"; // unique name for the tool
m_jetReclusteringTool->m_ptMin_input = 25.0; // GeV
m_jetReclusteringTool->m_ptMin_rc = 50.0; // GeV

if(!m_jetReclusteringTool->initialize()){
  Error("initialize()", "Could not initialize the JetReclusteringTool.");
  return EL::StatusCode::FAILURE;
}
```

and then simply call `m_jetReclusteringTool->execute()` in the `execute()` portion of your algorithm to fill the TStore with the appropriate container(s). Don't forget to delete the pointer when you're done

```c++
if(m_jetReclusteringTool) delete m_jetReclusteringTool;
```

### Incorporating in algorithm chain

This is the least destructive option since it requires **no change** to your existing code. All you need to do is create a new `JetReclustering` algorithm and add it to the job before other algorithms downstream that want access to the reclustered jets. It is highly configurable. In your runner macro, add the header

```c++
#include <xAODJetReclustering/JetReclusteringAlgo.h>
```

and then simply set up your algorithm like so

```c++
// initialize and set it up
JetReclustering* jetReclusterer = new JetReclusteringAlgo();
jetReclusterer->m_inputJetContainer = "AntiKt4LCTopoJets";
jetReclusterer->m_outputJetContainer = "AntiKt10LCTopoJetsRCAntiKt4LCTopoJets";
jetReclusterer->m_name = "R10"; // unique name for the tool
jetReclusterer->m_ptMin_input = 25.0; // GeV
jetReclusterer->m_ptMin_rc = 50.0; // GeV
// ...
// ...
// ...

// add it to your job sometime later
job.algsAdd(jetReclusterer);
```

### Studies and Example Usage

See [kratsg/ReclusteringStudies](https://github.com/kratsg/ReclusteringStudies) for studies and example usage.

### Accessing the subjets from constituents

The reclustered jets have constituents which are your input small-R jets. These can be re-inflated, so to speak. As an example, I wanted to get the btagging information of my subjets as well as their constituents (eg: the topological calorimeter clusters, TopoCaloClusters)

```c++
for(auto jet: *in_jets){
  const xAOD::Jet* subjet(nullptr);
  const xAOD::BTagging* btag(nullptr);
  for(auto constit: jet->getConstituents()){
    subjet = static_cast<const xAOD::Jet*>(constit->rawConstituent());
    btag = subjet->btagging();
    if(btag) Info("execute()", "btagging: %0.2f", btag->MV1_discriminant());

    for(auto subjet_constit: subjet->getConstituents()){
      Info("execute()", "\tconstituent pt: %0.2f", subjet_constit->pt());
    }
  }
}
```

where we explicitly `static_cast<>` our raw pointer from the `rawConstituent()` call. See [xAODJet/JetConstituentVector.h](http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODJet/xAODJet/JetConstituentVector.h) for more information about what is available. As a raw pointer, we already know that the input to the constituents were small-R jets (since we re-clustered ourselves) so this type of casting is safe.

#### Authors
- [Giordon Stark](https://github.com/kratsg)
