# The name of the package:
atlas_subdir( JetReclustering )

# Dependency helper variable:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess
      PhysicsAnalysis/D3PDTools/EventLoop PRIVATE Event/xAOD/xAODEventInfo
      Reconstruction/Jet/JetSubStructureMomentTools )
else()
   set( extra_deps PRIVATE Control/xAODRootAccess Control/AthenaBaseComps
      PhysicsAnalysis/POOLRootAccess GaudiKernel )
endif()

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Reconstruction/Jet/JetInterface
   Reconstruction/Jet/JetRec
   ${extra_deps}
   Event/xAODJet )

# Libraries in the package:
if( XAOD_STANDALONE )
   atlas_add_root_dictionary( JetReclusteringLib JetReclusteringCintDict
      ROOT_HEADERS JetReclustering/JetReclusteringAlgo.h Root/LinkDef.h
      EXTERNAL_PACKAGES ROOT )

   atlas_add_library( JetReclusteringLib
      JetReclustering/*.h Root/*.cxx ${JetReclusteringCintDict}
      PUBLIC_HEADERS JetReclustering
      LINK_LIBRARIES AsgTools JetInterface JetRecLib xAODRootAccess EventLoop
      PRIVATE_LINK_LIBRARIES xAODJet xAODEventInfo
      JetSubStructureMomentToolsLib )
else()
   atlas_add_library( JetReclusteringLib
      JetReclustering/*.h Root/*.cxx
      PUBLIC_HEADERS JetReclustering
      LINK_LIBRARIES AsgTools JetInterface JetRecLib
      PRIVATE_LINK_LIBRARIES xAODJet xAODRootAccess )

   atlas_add_component( JetReclustering
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps GaudiKernel JetInterface
      JetReclusteringLib )
endif()

atlas_add_dictionary( JetReclusteringDict
   JetReclustering/JetReclusteringDict.h
   JetReclustering/selection.xml
   LINK_LIBRARIES JetReclusteringLib )

# Test(s) in the package:
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs POOLRootAccessLib )
endif()
atlas_add_test( ut_JetReclusteringTool_test
                SOURCES test/ut_JetReclusteringTool_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess AsgTools JetInterface xAODJet
                ${extra_libs} )

# Install files from the package:
atlas_install_joboptions( share/*.py )
