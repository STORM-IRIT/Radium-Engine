# have ExternalProject available
include(ExternalProject)

# here is defined the way we want to import assimp
ExternalProject_Add(
        assimp

        # where the source will live
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/Assimp"

        # override default behaviours
        UPDATE_COMMAND ""

        # set the installatin to root
        INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)
