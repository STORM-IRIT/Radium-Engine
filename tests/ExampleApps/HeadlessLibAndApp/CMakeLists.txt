cmake_minimum_required(VERSION 3.13)
if(APPLE)
    cmake_policy(SET CMP0042 NEW)
endif(APPLE)

if(IN_RADIUM_BUILD_TREE)
    project(HeadlessDemo VERSION ${Radium_VERSION})
else()
    project(HeadlessDemo VERSION 1.0.1)
endif()
# To prevent installation ordering issue and make the library installed before the application, so
# that fixup_bundle will success on windows, we have to define libraries and executable in different
# subdirectories. see https://gitlab.kitware.com/cmake/cmake/-/issues/17989

# configure, build and install the Radium headless library
add_subdirectory(libHeadless)

# configure, build and install the demo application for Radium headless
add_subdirectory(Application)
