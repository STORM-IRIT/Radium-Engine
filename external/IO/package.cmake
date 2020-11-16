if(NOT DEFINED assimp_DIR)
    set(assimp_sub_DIR lib/cmake/assimp-5.0/ CACHE INTERNAL "")
    set(assimp_DIR ${CMAKE_INSTALL_PREFIX}/${assimp_sub_DIR})
endif()

if(NOT DEFINED tinyply_DIR)
    set(tinyply_sub_DIR lib/cmake/tinyply/ CACHE INTERNAL "")
    set(tinyply_DIR ${CMAKE_INSTALL_PREFIX}/${tinyply_sub_DIR})
endif()
