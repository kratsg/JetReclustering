# Jet Reclustering

*This is currently in AnalysisBase releases 2.4.17+.*

This tool allows you to recluster small-R xAOD jets into large-R xAOD jets. It provides configurable filtering of the small-R jets, reclustering using standard or variable-R algorithms, configurable trimming of the large-R jets, and jet moment & jet substructure moment calculations.

If you would like to get involved, see the twiki for [the JetMET working group for jet reclustering](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetReclustering). The [pre-recommendations](https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/PreRec) twiki contains the guidelines for your analyses.

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**  *generated with [DocToc](https://github.com/thlorenz/doctoc)*

- [Installing](#installing)
- [Configurations for](#configurations-for)
  - [`JetReclusteringTool` tool](#jetreclusteringtool-tool)
  - [`JetReclusteringAlgo` algorithm](#jetreclusteringalgo-algorithm)
  - [`AthJetReclusteringAlgo` Athena algorithm](#athjetreclusteringalgo-athena-algorithm)
- [Using Jet Reclustering](#using-jet-reclustering)
  - [Input Jet Filtering](#input-jet-filtering)
  - [Output Reclustered Jet Trimming](#output-reclustered-jet-trimming)
  - [Variable-R Jet Finding](#variable-r-jet-finding)
  - [Area Calculations](#area-calculations)
  - [Incorporating in existing code](#incorporating-in-existing-code)
    - [RootCore](#rootcore)
    - [Athena](#athena)
  - [Incorporating in algorithm chain](#incorporating-in-algorithm-chain)
    - [RootCore](#rootcore-1)
    - [Athena](#athena-1)
- [Studies and Example Usage](#studies-and-example-usage)
  - [Accessing the subjets from constituents](#accessing-the-subjets-from-constituents)
  - [Accessing various jet moments](#accessing-various-jet-moments)
- [Authors](#authors)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## Installing

This works in AB 2.3.X and 2.4.Y on ROOT6 releases. As long as [JetRec](http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Reconstruction/Jet/JetRec/JetRec/) works, this will be ok.

```bash
rcSetup Base,2.4.Y # or 2.3.X
git clone https://github.com/kratsg/JetReclustering.git
rc find_packages
rc compile
```

*Warning*: use `X >= 45` since I am using `ANA_CHECK` which only works with `EventLoop-00-01-35` or better. Use `Y >= 17` as it is part of the atlas software releases.

## Configurations for

### `JetReclusteringTool` tool

 Property           | Type                      | Default                   | Description
:-------------------|:-------------------------:|--------------------------:|:-------------------------------------------------------------------------------------
InputJetContainer   | string                    |                           | name of the input jet container for reclustering
OutputJetContainer  | string                    |                           | name of the output jet container holding reclustered jets
InputJetPtMin       | float                     | 25.0                      | filter input jets by requiring a minimum pt cut [GeV]
ReclusterAlgorithm  | string                    | AntiKt                    | name of algorithm for clustering large-R jets {AntiKt, Kt, CamKt}
ReclusterRadius     | float                     | 1.0                       | radius of large-R reclustered jets or maximum radius of variable-R jet finding
RCJetPtMin          | float                     | 50.0                      | filter reclustered jets by requiring a minimum pt cut [GeV]
RCJetPtFrac         | float                     | 0.05                      | trim the reclustered jets with a PtFrac on its constituents (eg: small-R input jets)
RCJetSubjetRadius   | float                     | 0.0                       | radius parameter for kt-clustering to form subjets (R=0.0 should not do any clustering)
VariableRMinRadius  | float                     | -1.0                      | minimum radius for variable-R jet finding
VariableRMassScale  | float                     | -1.0                      | mass scale [GeV] for variable-R jet finding
DoArea              | bool                      | false                     | turn on ghost area calculations (set ghost area scale to 0.01)
AreaAttributes      | string                    | ActiveArea ActiveArea4vec | space-delimited list of attributes to transfer over from fastjet

### `JetReclusteringAlgo` algorithm

As well as the provided above configurations for the `JetReclusteringTool`, we also provide a `m_debug` configuration for extra verbose output and an `m_outputXAODName` to create an output xAOD containing the reclustered jets (note: experimental)

Variable            | Type      | Default                   | Description
:-------------------|:---------:|--------------------------:|:-------------------------------------------------------------------------------------
m_inputJetContainer | string    |                           | see above
m_outputJetContainer| string    |                           | see above
m_ptMin_input       | float     | 25.0                      | see above
m_rc_alg            | string    | AntiKt                    | see above
m_radius            | float     | 1.0                       | see above
m_ptMin_rc          | float     | 50.0                      | see above
m_ptFrac            | float     | 0.05                      | see above
m_subjet_radius     | float     | 0.0                       | see above
m_varR_minR         | float     | -1.0                      | see above
m_varR_mass         | float     | -1.0                      | see above
m_doArea            | bool      | false                     | see above
m_areaAttributes    | string    | ActiveArea ActiveArea4vec | see above
m_outputXAODName    | string    |                           | if defined, put the reclustered jets in an output xAOD file of the given name
m_debug             | bool      | false                     | enable verbose debugging information, such as printing the tool configurations

### `AthJetReclusteringAlgo` Athena algorithm

 Property           | Type                      | Default                   | Description
:-------------------|:-------------------------:|--------------------------:|:-------------------------------------------------------------------------------------
JetReclusteringTool          | ToolHandle                |                           | The JetReclusteringTool to use. All configurables should be set on this.

## Using Jet Reclustering

### Input Jet Filtering

The input jets can be filtered using the `InputJetPtMin` or `m_ptMin_input` options. If these are set to 0 (or less), this will turn the jet filtering tool off (essentially skipping the step).

### Output Reclustered Jet Trimming

The output jets can be trimmed using the `RCJetPtFrac` or `m_ptFrac` options. If these are set to 0 (or less), this will turn the reclustered jet trimming tool off (essentially skipping the step). Note that by default, we will not kt-cluster the subjets as `RCJetSubjetRadius` / `m_subjet_radius` are set to 0.0 by default, however one can set them to a non-zero radius value if you want to form subjets (if you somehow passed in `xAOD::Jet` objects that represent clusters rather than small-R jets).

### Variable-R Jet Finding

Variable-R jet finding is performed if `VariableRMinRadius >= 0` and `VariableRMassScale >= 0`. For more information on these variables, see the [Jets with Variable R](http://arxiv.org/pdf/0903.0392v1.pdf) paper. If you choose variable-R jet finding, the maximum jet radius will be specified by `ReclusterRadius`. The relevant properties are listed in the following table

 Property           | Type                      | Default                   | Description
:-------------------|:-------------------------:|--------------------------:|:-------------------------------------------------------------------------------------
ReclusterRadius     | float                     | 1.0                       | maximum radius of variable-R jet finding
VariableRMinRadius  | float                     | -1.0                      | minimum radius for variable-R jet finding
VariableRMassScale  | float                     | -1.0                      | mass scale [GeV] for variable-R jet finding

When a new jet is formed using variable-R jet finding, it will have some extra attributes as mentioned on the [Run 2 - Jet Moments](https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/Run2JetMoments) twiki. We will also decorate with an `EffectiveR` attribute as well which reflects the effective radius of the reclustered jet using its untrimmed transverse momentum. To summarize the translations

 Property           | Type                      | Jet Attribute
:-------------------|:-------------------------:|--------------------
ReclusterRadius     | float                     | SizeParameter
VariableRMinRadius  | float                     | VariableRMinRadius
VariableRMassScale  | float                     | VariableRMassScale
                    | float                     | EffectiveR

### Area Calculations

Areas can be calculated and added to the jets. Fastjet does the area calculation and these values can be transferred over using the `JetFromPseudojet` tool. To make this happen, simply enable `m_doArea` (`DoArea`) which will set the ghost area size to `0.01` which is a reasonable default for most use cases. The attributes that get transferred over are defined in `m_areaAttributes` (`AreaAttributes`). As of the time of writing this section of the README, there were only two allowed values which would get decorated:

- `ActiveArea` (most people use this one)
- `ActiveArea4vec`

### Incorporating in existing code

#### RootCore

If you wish to incorporate `JetReclustering` directly into your code, add this package as a dependency in `cmt/Makefile.RootCore` and then a header

```c++
#include <AsgTools/AnaToolHandle.h>
#include <JetInterface/IJetExecuteTool.h>

class MyAlgo : public EL::Algorithm {
  // ...

  asg::AnaToolHandle<IJetExecuteTool> m_jetReclusteringTool; //!
}
```

to get started. In the source, you need to add the tool header

```c++
#include <JetReclustering/JetReclusteringTool.h>
```

then make sure the AsgTool tool store sets up the tool correctly in the constructor

```c++
MyAlgo :: MyAlgo () :
  m_jetReclusteringTool("JetReclusteringTool/ANameForTheTool")
  {}
```

At this point, you can set up your standard tool in the `initialize()` portion of your algorithm as a tool handle

```c++
ANA_CHECK_SET_TYPE (EL::StatusCode);
ANA_CHECK(ASG_MAKE_ANA_TOOL(m_jetReclusteringTool, JetReclusteringTool));
ANA_CHECK(m_jetReclusteringTool.setProperty("InputJetContainer",  m_inputJetContainer));
ANA_CHECK(m_jetReclusteringTool.setProperty("OutputJetContainer", m_outputJetContainer));
ANA_CHECK(m_jetReclusteringTool.setProperty("ReclusterRadius",    m_radius));
ANA_CHECK(m_jetReclusteringTool.setProperty("ReclusterAlgorithm", m_rc_alg));
ANA_CHECK(m_jetReclusteringTool.setProperty("VariableRMinRadius", m_varR_minR));
ANA_CHECK(m_jetReclusteringTool.setProperty("VariableRMassScale", m_varR_mass));
ANA_CHECK(m_jetReclusteringTool.setProperty("InputJetPtMin",      m_ptMin_input));
ANA_CHECK(m_jetReclusteringTool.setProperty("RCJetPtMin",         m_ptMin_rc));
ANA_CHECK(m_jetReclusteringTool.setProperty("RCJetPtFrac",        m_ptFrac));
ANA_CHECK(m_jetReclusteringTool.setProperty("RCJetSubjetRadius",  m_subjet_radius));
ANA_CHECK(m_jetReclusteringTool.setProperty("DoArea",             m_doArea));
ANA_CHECK(m_jetReclusteringTool.setProperty("AreaAttributes",     m_areaAttributes));
ANA_CHECK(m_jetReclusteringTool.retrieve());
```

and then simply call `m_jetReclusteringTool->execute()` in the `execute()` portion of your algorithm to fill the TStore with the appropriate container(s). Note that you use a pointer on the second portion when calling `execute()` to access the underlying pointer to the tool itself. The functions `setProperty()` and `initialize()` have a return type `StatusCode` which needs to be checked.

#### Athena

The methods for incorporating the code into an Athena algorithm are very similar to the method for RootCore.
The package must be included with a 'use' statement in `cmt/requirements`. Then in your header:
```c++
#include <AsgTools/ToolHandle.h> // Can use AnaToolHandle instead if you want
#include <JetInterface/IJetExecuteTool.h>

class MyAlgo : public ::AthAnalysisAlgorithm { // Or any of the other Athena algorithm types
  // ...

  ToolHandle<IJetExecuteTool> m_jetReclusteringTool;
};
```

As with other Athena algorithms it's recommended to configure the tool in your job options and then pass it into your algorithm as a property. To do this, in the constructor you need to initialize the handle and declare it as a property

```c++

MyAlgo::MyAlgo( const std::string& name, ISvcLocator* pSvcLocator )
: AthAnalysisAlgorithm( name, pSvcLocator ), // Or other base class
  m_jetReclusteringTool("") // Can also initialize with a default type/name if you wish
{
  // Other properties...
  declareProperty( "JetReclusteringTool", m_jetReclusteringTool );
}
```

Then in the `initialize()` member function of your algorithm `retrieve` the tool

```c++
  ATH_CHECK( m_jetReclusteringTool.retrieve() );
```

To use it in your algorithm you can call (in your `execute()` function). Make sure you do this **before** anyone tries to use the output...
```c++
  int retCode = m_jetReclusteringTool->execute();
  if (retCode != 0) {
    ATH_MSG_ERROR( "JetReclusteringTool failed in execution with code: " << retCode );
    return StatusCode::FAILURE;
  }
```

Then you need to create and pass in your tool in your joboptions (see the section below for more instructions, replace `AthJetReclusteringAlgo` with `MyAlgo` or whatever you called your algorithm).
It is also possible to set the properties and initialize your tool in the cxx code but this isn't really recommended. If you want to do this either use AnaToolHandle and setProperty (analogously to in RootCore) or use ToolHandle and [AthAnalysisHelper](http://acode-browser2.usatlas.bnl.gov/lxr-AthAna/source/atlas/Control/AthAnalysisBaseComps/AthAnalysisBaseComps/AthAnalysisHelper.h#0053).

### Incorporating in algorithm chain

#### RootCore

This is the least destructive option since it requires **no change** to your existing code. All you need to do is create a new `JetReclusteringAlgo` algorithm and add it to the job before other algorithms downstream that want access to the reclustered jets. It is highly configurable. In your runner macro, add the header

```c++
#include <JetReclustering/JetReclusteringAlgo.h>
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
jetReclusterer->m_ptFrac = 0.05; // GeV

// ...
// ...
// ...

// add it to your job sometime later
job.algsAdd(jetReclusterer);
```

#### Athena

As with RootCore this is the best way to use the tool (Athena is designed to execute a series of algorithms after all...).
All you need to do is create a `JetReclusteringTool` in the `ToolSvc`, add an `AthJetReclusteringAlgo` to your `AlgSequence` and connect them

```python
ToolSvc += CfgMgr.JetReclusteringTool("MyJetReclusteringTool", InputJetContainer = "AntiKt4EMTopoJets", OutputJetContainer = "AntiKt10EMTopoJets_RC") # Set up properties here
ToolSvc.MyJetReclusteringTool.InputJetPtMin = 10 #Can also set properties like this

algseq = CfgMgr.AthSequencer("AthAlgSeq")                #gets the main AthSequencer
algseq += CfgMgr.AthJetReclusteringAlgo("JetRecAlgo", JetReclusteringTool = ToolSvc.MyJetReclusteringTool)
```

Now any algorithm downstream of this in algseq will have access to the `AntiKt10EMPTopoJets_RC` container.
An example (and a test you can use) is in `share/AthJetReclusteringAlgoJobOptions.py`

## Studies and Example Usage

See [kratsg/ReclusteringStudies](https://github.com/kratsg/ReclusteringStudies) for studies and example usage.

### Accessing the subjets from constituents

The reclustered jets have constituents which are your input small-R jets. These can be re-inflated, so to speak. As an example, I wanted to get the btagging information of my subjets as well as their constituents (eg: the topological calorimeter clusters, `TopoCaloClusters`)

```c++
for(auto jet: *in_jets){
  const xAOD::Jet* subjet(nullptr);
  const xAOD::BTagging* btag(nullptr);
  for(auto constit: jet->getConstituents()){
    if (subjet->type() != xAOD::Type::Jet) {
      // You need to handle this somehow
      continue;
    }
    subjet = static_cast<const xAOD::Jet*>(constit->rawConstituent());
    btag = subjet->btagging();
    if(btag)
      Info("execute()", "btagging: %0.2f", btag->MV1_discriminant());

    for(auto subjet_constit: subjet->getConstituents())
      Info("execute()", "\tconstituent pt: %0.2f", subjet_constit->pt());
  }
}
```

where we explicitly `static_cast<>` our raw pointer from the `rawConstituent()` call. See [xAODJet/JetConstituentVector.h](http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODJet/xAODJet/JetConstituentVector.h) for more information about what is available. As a raw pointer, we already know that the input to the constituents were small-R jets (since we re-clustered ourselves) so this type of casting is safe.

### Accessing various jet moments

We try to use the standard `JetModifier` tools that are available ATLAS-wide. In those cases, you can find a lot more information on the [Run 2 - Jet Moments](https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/Run2JetMoments) twiki. There are two ways to figure out what information is stored on the reclustered jet.

1. Store the reclustered jets in an output xAOD. Use [kratsg/xAODDumper](https://github.com/kratsg/xAODDumper) to dump the properties and attributes of the jet containers associated with the reclustered jets.
2. Go to [Root/JetReclusteringTool.cxx](Root/JetReclusteringTool.cxx#L101) around line 100, find the `modArray.push_back()` calls, and look at all the tools being added. For a given tool, you can look it up in the [Run 2 - Jet Moments](https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/Run2JetMoments) twiki to figure out the corresponding property names.

As an example of the second way, I see that `m_ktSplittingScaleTool` is added. On the twiki, I see an entry for `KTSplittingScaleTool` which lists 6 variables associated with it: `Split12, Split23, Split34, ZCut12, ZCut23, ZCut34` and all are of a `float` type, so I can write

```c++
static SG::AuxElement::ConstAccessor<float> Split12("Split12");
static SG::AuxElement::ConstAccessor<float> Split23("Split23");
static SG::AuxElement::ConstAccessor<float> Split34("Split34");
static SG::AuxElement::ConstAccessor<float> ZCut12("ZCut12");
static SG::AuxElement::ConstAccessor<float> ZCut23("ZCut23");
static SG::AuxElement::ConstAccessor<float> ZCut34("ZCut34");
```

and with these, I can quickly access it on my jet (protecting myself against when it doesn't exist for some reason)

```c++
for(auto jet: *in_jets){
  if(Split12.isAvailable(*jet))
    Info("execute()", "\tSplit12: %0.2f", Split12(*jet));
  if(Split23.isAvailable(*jet))
    Info("execute()", "\tSplit23: %0.2f", Split23(*jet));
  if(Split34.isAvailable(*jet))
    Info("execute()", "\tSplit34: %0.2f", Split34(*jet));
  if(ZCut12.isAvailable(*jet))
    Info("execute()", "\tZCut12: %0.2f", ZCut12(*jet));
  if(ZCut23.isAvailable(*jet))
    Info("execute()", "\tZCut23: %0.2f", ZCut23(*jet));
  if(ZCut34.isAvailable(*jet))
    Info("execute()", "\tZCut34: %0.2f", ZCut34(*jet));
}

```

## Authors
- [Giordon Stark](https://github.com/kratsg)
- [Jon Burr](https://github.com/j0nburr)
