if(NOT DEFINED stduuid_DIR)
    set(stduuid_sub_DIR lib/cmake/stduuid CACHE INTERNAL "")
    set(stduuid_DIR ${CMAKE_INSTALL_PREFIX}/${stduuid_sub_DIR})
endif()
