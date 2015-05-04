# Reclustering - A RootCore Package

## Installing
The last stable analysis base used is **2.1.30**. To install,
```bash
rcSetup Base,2.1.30
git clone https://github.com/kratsg/Reclustering.git
git clone https://github.com/kratsg/TheAccountant.git
rc find_packages
rc compile
```

See https://github.com/kratsg/TheAccountant#installing for more information about installing the rest of the dependencies.

### Jet Reclustering

Using `xAODAnaHelpers`, we provide an easy way for you to enable jet reclustering in a job. In particular, given a `json` file

```json
[
  {
    "class": "JetReclustering",
    "configs": {
      "m_inputJetName": "AntiKt4LCTopoJets",
      "m_outputJetName": "AntiKt10LCTopoJetsRCAntiKt4LCTopoJets",
      "m_radius": 1.0,
      "m_debug": false,
      "m_outputXAODName": "output"
    }
  }
]
```

you can run the reclustering code and produce a basic output file (by setting `m_outputXAODName`) and running `xAH_run.py input.root  --config config.json`. This is a minimal working example. You can include this in an algorithm chain.

#### Authors
- [Giordon Stark](https://github.com/kratsg)
