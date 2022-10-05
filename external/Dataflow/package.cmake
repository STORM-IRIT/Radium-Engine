if(NOT DEFINED stduuid_DIR)
    set(stduuid_sub_DIR lib/cmake/stduuid CACHE INTERNAL "")
    set(stduuid_DIR ${CMAKE_INSTALL_PREFIX}/${stduuid_sub_DIR})
endif()

if(NOT DEFINED RadiumNodeEditor_DIR)
    set(RadiumNodeEditor_sub_DIR lib/cmake/RadiumNodeEditor CACHE INTERNAL "")
    set(RadiumNodeEditor_DIR ${CMAKE_INSTALL_PREFIX}/${RadiumNodeEditor_sub_DIR})
endif()
