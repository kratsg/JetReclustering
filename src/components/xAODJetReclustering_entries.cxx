
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "xAODJetReclustering/JetReclusteringTool.h"
#include "xAODJetReclustering/EffectiveRTool.h"

DECLARE_TOOL_FACTORY( JetReclusteringTool )
DECLARE_TOOL_FACTORY( EffectiveRTool )


#include "../AthJetReclusteringAlgo.h"
DECLARE_ALGORITHM_FACTORY( AthJetReclusteringAlgo )

DECLARE_FACTORY_ENTRIES( xAODJetReclustering ) 
{
  DECLARE_ALGORITHM( AthJetReclusteringAlgo );
  DECLARE_TOOL( JetReclusteringTool );
  DECLARE_TOOL( EffectiveRTool );
}
