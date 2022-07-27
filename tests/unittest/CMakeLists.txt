#------------------------------------------------------------------------------
# Unit tests via Catch framework
#
# For testing on the function/class level.

# -----------------------------------------------------------------------------
set(TEST_SRC
    unittest.cpp
    unittestUtils.hpp
    Core/algebra.cpp
    Core/animation.cpp
    Core/camera.cpp
    Core/color.cpp
    Core/containers.cpp
    Core/distance.cpp
    Core/indexmap.cpp
    Core/indexview.cpp
    Core/mapiterators.cpp
    Core/observer.cpp
    Core/obb.cpp
    Core/polyline.cpp
    Core/raycast.cpp
    Core/resources.cpp
    Core/string.cpp
    Core/topomesh.cpp
    Engine/environmentmap.cpp
    Engine/signalmanager.cpp
    Engine/renderparameters.cpp
    Gui/keymapping.cpp
    Core/geometryData.cpp
)

get_target_property(HAS_VOLUMES IO RADIUM_IO_HAS_VOLUMES)
if(${HAS_VOLUMES})
    message(STATUS "Compiling Volume loader unit test")
    list(APPEND TEST_SRC IO/volumeloader.cpp)
endif()

add_executable(unittests ${TEST_SRC})
target_compile_options(unittests PUBLIC ${RA_DEFAULT_COMPILE_OPTIONS})

target_compile_definitions(unittests PRIVATE UNIT_TESTS) # add -DUNIT_TESTS define
target_link_libraries(unittests PRIVATE Catch2::Catch2 Core Gui)

add_dependencies(unittests Catch2 Core Gui)
target_include_directories(unittests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(../external/Catch2/src/Catch2_download/contrib/Catch.cmake)

catch_discover_tests(unittests WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Filesystem COMPONENTS Final Experimental REQUIRED)
find_qt_package(COMPONENTS Core Widgets OpenGL Xml REQUIRED)

# Qt
set(QT_LIBRARIES Qt::Core Qt::Widgets Qt::OpenGL Qt::Xml)
target_link_libraries(unittests PRIVATE ${QT_LIBRARIES} std::filesystem)
target_compile_definitions(
    unittests
    PRIVATE -DCXX_FILESYSTEM_HAVE_FS
            -DCXX_FILESYSTEM_IS_EXPERIMENTAL=$<BOOL:${CXX_FILESYSTEM_IS_EXPERIMENTAL}>
            -DCXX_FILESYSTEM_NAMESPACE=${CXX_FILESYSTEM_NAMESPACE}
)

# convenience target for running only the unit tests
add_custom_target(
    run_unittests # this way we can use fake data from /tests/unittest/ dir (if we have any):
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND $<TARGET_FILE:unittests>
    DEPENDS unittests
    COMMENT "Run unittests only."
)

# Verbose printing of results
add_custom_target(
    run_unittests_verbose
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND $<TARGET_FILE:unittests> -s -a -r=compact
    DEPENDS unittests
    COMMENT "Run unittests only, verbose."
)
