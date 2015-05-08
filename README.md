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

### Using xAOD Jet Reclustering

#### Incorporating in existing code

If you wish to incorporate `JetReclustering` directly into your code, add this package as a dependency in `cmt/Makefile.RootCore` and then a header

```c++
#include <xAODJetReclustering/Helpers.h>
```

to get started. At this point, you can set up your standard tool in the `initialize()` portion of your algorithm as a pointer

```c++
JetRecTool* m_jetReclusteringTool = xAODJetReclustering::JetReclusteringTool(m_inputJetName, m_outputJetName, m_radius, m_clusteringAlgorithm);
```

and then simply call `m_jetReclusteringTool->execute()` in the `execute()` portion of your algorithm to fill the TStore with the appropriate container(s).

#### Incorporating in algorithm chain

This is the least destructive option since it requires **no change** to your existing code. All you need to do is create a new `JetReclustering` algorithm and add it to the job before other algorithms downstream that want access to the reclustered jets. It is highly configurable. In your runner macro, add the header

```c++
#include <xAODJetReclustering/JetReclustering.h>
```

and then simply set up your algorithm like so

```c++
// initialize and set it up
JetReclustering* jetReclusterer = new JetReclustering();
jetReclusterer->m_inputJetName = "AntiKt4LCTopoJets";
jetReclusterer->m_outputJetName = "AntiKt10LCTopoJetsRCAntiKt4LCTopoJets";

// ...
// ...
// ...

// add it to your job sometime later
job.algsAdd(jetReclusterer);
```

#### Authors
- [Giordon Stark](https://github.com/kratsg)
