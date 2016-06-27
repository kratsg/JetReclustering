
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "JetReclustering/JetReclusteringTool.h"
#include "JetReclustering/EffectiveRTool.h"

DECLARE_TOOL_FACTORY( JetReclusteringTool )
DECLARE_TOOL_FACTORY( EffectiveRTool )


#include "../AthJetReclusteringAlgo.h"
DECLARE_ALGORITHM_FACTORY( AthJetReclusteringAlgo )

DECLARE_FACTORY_ENTRIES( JetReclustering )
{
  DECLARE_ALGORITHM( AthJetReclusteringAlgo );
  DECLARE_TOOL( JetReclusteringTool );
  DECLARE_TOOL( EffectiveRTool );
}
