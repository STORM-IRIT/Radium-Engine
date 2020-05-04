
#------------------------------------------------------------------------------
# Unit tests via Catch framework
#
# For testing on the function/class level.

# -----------------------------------------------------------------------------


add_executable(unittests
    unittest.cpp
    unittestUtils.hpp
    Core/algebra.cpp
    Core/animation.cpp
    Core/color.cpp
    Core/containers.cpp
    Core/distance.cpp
    Core/indexmap.cpp
    Core/mesh.cpp
    Core/observer.cpp
    Core/obb.cpp
    Core/polyline.cpp
    Core/raycast.cpp
    Core/string.cpp
    Core/topomesh.cpp
    )
target_compile_definitions(unittests PRIVATE UNIT_TESTS) # add -DUNIT_TESTS define
target_link_libraries(unittests PRIVATE Catch2 Core)
add_dependencies(unittests Catch2 Core)
target_include_directories(unittests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# convenience target for running only the unit tests
add_custom_target(unit
    #this way we can use faux data from /test dir (if we have any):
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests
    COMMAND $<TARGET_FILE:unittests>
    DEPENDS unittests)
# Verbose printing of results
add_custom_target(unitall
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests
    COMMAND $<TARGET_FILE:unittests> -s -a -r=compact
    DEPENDS unittests)



