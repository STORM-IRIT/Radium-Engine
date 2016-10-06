# have ExternalProject available
include(ExternalProject)

#set( ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE )
#set( ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE )
#set( ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE )

# here is defined the way we want to import assimp
ExternalProject_Add(
        assimp

        # where the source will live
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/Assimp"

        # override default behaviours
        UPDATE_COMMAND ""

        # set the installatin to root
	# INSTALL_COMMAND ""
        INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DASSIMP_BUILD_ASSIMP_TOOLS=False -DASSIMP_BUILD_SAMPLES=False -DASSIMP_BUILD_TESTS=False
)
