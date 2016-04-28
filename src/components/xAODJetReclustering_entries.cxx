
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "xAODJetReclustering/JetReclusteringTool.h"

DECLARE_TOOL_FACTORY( JetReclusteringTool )

DECLARE_FACTORY_ENTRIES( xAODJetReclustering ) 
{
  DECLARE_TOOL( JetReclusteringTool );
}
