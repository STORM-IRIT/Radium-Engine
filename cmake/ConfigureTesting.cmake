add_custom_target(buildtests)
add_custom_target(check COMMAND "ctest")
add_dependencies(check buildtests)

# Usefull helper to add tests in radium
# call example:
#   radium_add_test ( TARGET test1
#                     SRC test1.cpp
#                     LIBS radiumCore radiumIO )
# Tests are NOT generated in Radium Bundle, but in ${CMAKE_CURRENT_BINARY_DIR}/tests/...
function (radium_add_test)
    set(_options "")
    set(_oneValueArgs TARGET)
    set(_multiValueArgs SRC LIBS)
    cmake_parse_arguments(_option "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN})

    add_executable(test_${_option_TARGET} ${_option_SRC} ${testing_SRCS})
    set_property(TARGET test_${_option_TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    add_dependencies(buildtests test_${_option_TARGET})
    add_test(NAME test_${_option_TARGET}
             #CONFIGURATIONS Release
             COMMAND test_${_option_TARGET})
    target_link_libraries(test_${_option_TARGET} ${_option_LIBS})
    target_include_directories(test_${_option_TARGET} PUBLIC ${testing_include_dir} ${RADIUM_SRC_DIR})
endfunction()

# CMake/Ctest does not allow us to change the build command,
# so we have to workaround by directly editing the generated DartConfiguration.tcl file
# save CMAKE_MAKE_PROGRAM
set(CMAKE_MAKE_PROGRAM_SAVE ${CMAKE_MAKE_PROGRAM})
# and set a fake one
set(CMAKE_MAKE_PROGRAM "@RADIUM_MAKECOMMAND_PLACEHOLDER@")

# This call activates testing and generates the DartConfiguration.tcl
# This adds another build target, which is test for Makefile generators,
# or RUN_TESTS for integrated development environments (like Visual Studio)
include(CTest)

set(RADIUM_TEST_BUILD_FLAGS "" CACHE STRING "Options passed to the build command of unit tests")

# overwrite default DartConfiguration.tcl
# The worarounds are different for each version of the MSVC IDE
set(RADIUM_TEST_TARGET buildtests)
if(MSVC_IDE)
  if(CMAKE_MAKE_PROGRAM_SAVE MATCHES "devenv") # devenv
    set(RADIUM_BUILD_COMMAND "${CMAKE_MAKE_PROGRAM_SAVE} RADIUM.sln /build Release /project ${RADIUM_TEST_TARGET}")
  else() # msbuild
    set(RADIUM_BUILD_COMMAND "${CMAKE_MAKE_PROGRAM_SAVE} ${RADIUM_TEST_TARGET}.vcxproj /p:Configuration=\${CTEST_CONFIGURATION_TYPE}")
  endif()

  # append the build flags if provided
  if(NOT "${RADIUM_TEST_BUILD_FLAGS}" MATCHES "^[ \t]*$")
    set(RADIUM_BUILD_COMMAND "${RADIUM_BUILD_COMMAND} ${RADIUM_TEST_BUILD_FLAGS}")
  endif()

  # apply the dartconfig hack ...
  set(RADIUM_MAKECOMMAND_PLACEHOLDER "${RADIUM_BUILD_COMMAND}\n#")
else()
  # for make and nmake
  set(RADIUM_BUILD_COMMAND "${CMAKE_MAKE_PROGRAM_SAVE} ${RADIUM_TEST_TARGET} ${RADIUM_TEST_BUILD_FLAGS}")
  set(RADIUM_MAKECOMMAND_PLACEHOLDER "${RADIUM_BUILD_COMMAND}")
endif()

configure_file(${CMAKE_BINARY_DIR}/DartConfiguration.tcl ${CMAKE_BINARY_DIR}/DartConfiguration.tcl)

# restore default CMAKE_MAKE_PROGRAM
set(CMAKE_MAKE_PROGRAM ${CMAKE_MAKE_PROGRAM_SAVE})

# un-set temporary variables so that it is like they never existed
unset(CMAKE_MAKE_PROGRAM_SAVE)
unset(RADIUM_MAKECOMMAND_PLACEHOLDER)


## Configure coverage
#if(CMAKE_COMPILER_IS_GNUCXX)
#  option(RADIUM_COVERAGE_TESTING "Enable/disable gcov" ON)

#  if(RADIUM_COVERAGE_TESTING)
#    set(COVERAGE_FLAGS "-fprofile-arcs -ftest-coverage")
#  else(RADIUM_COVERAGE_TESTING)
#    set(COVERAGE_FLAGS "")
#  endif(RADIUM_COVERAGE_TESTING)

#  if(RADIUM_TEST_C++0x)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
#  endif(RADIUM_TEST_C++0x)

#  if(CMAKE_SYSTEM_NAME MATCHES Linux)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_FLAGS} -g2")
#    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${COVERAGE_FLAGS} -O2 -g2")
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${COVERAGE_FLAGS} -fno-inline-functions")
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${COVERAGE_FLAGS} -O0 -g3")
#  endif(CMAKE_SYSTEM_NAME MATCHES Linux)

#elseif(MSVC)

#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_CRT_SECURE_NO_WARNINGS /D_SCL_SECURE_NO_WARNINGS")

#endif(CMAKE_COMPILER_IS_GNUCXX)

