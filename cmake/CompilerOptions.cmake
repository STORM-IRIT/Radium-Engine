#inspired from https://github.com/cginternals/glbinding/blob/master/cmake/CompileOptions.cmake

#
# Platform and architecture setup
#

# Get upper case system name
string(TOUPPER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_UPPER)

# Determine architecture (32/64 bit)
set(X64 OFF)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(X64 ON)
endif()

#
# Project options
#

set(RA_DEFAULT_PROJECT_OPTIONS
    DEBUG_POSTFIX
    "d"
    POSITION_INDEPENDENT_CODE
    ON
    CXX_EXTENSIONS
    Off
    CXX_STANDARD
    17
    LINKER_LANGUAGE
    "CXX"
    VERSION
    ${RADIUM_VERSION}
)

#
# Include directories
#

set(RA_DEFAULT_INCLUDE_DIRECTORIES)

#
# Libraries
#

set(RA_DEFAULT_LIBRARIES)

#
# Compile definitions
#

set(RA_DEFAULT_COMPILE_DEFINITIONS SYSTEM_${SYSTEM_NAME_UPPER})

# MSVC compiler options
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(RA_DEFAULT_COMPILE_DEFINITIONS
        ${RA_DEFAULT_COMPILE_DEFINITIONS}
        _SCL_SECURE_NO_WARNINGS # Calling any one of the potentially unsafe methods in the Standard
                                # C++ Library
        _CRT_SECURE_NO_WARNINGS # Calling any one of the potentially unsafe methods in the CRT
                                # Library
    )
endif()

#
# Compile options
#

set(RA_DEFAULT_COMPILE_OPTIONS)
set(RA_GLBINDINGS_COMPILE_OPTIONS)

# MSVC compiler options
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(RA_GLBINDINGS_COMPILE_OPTIONS
        ${RA_DEFAULT_COMPILE_OPTIONS}
        PRIVATE
        /MP # -> build with multiple processes
        /W4 # -> warning level 4
        # /WX         # -> treat warnings as errors
        /wd4251 # -> disable warning: 'identifier': class 'type' needs to have dll-interface to be
                # used by clients of class 'type2'
        /wd4592 # -> disable warning: 'identifier': symbol will be dynamically initialized
                # (implementation limitation)
        # /wd4201     # -> disable warning: nonstandard extension used: nameless struct/union
        # (caused by GLM)
        /wd4127 # -> disable warning: conditional expression is constant (caused by Qt)
        # /Zm114      # -> Memory size for precompiled headers (insufficient for msvc 2013)
        /Zm200 # -> Memory size for precompiled headers
        # $<$<CONFIG:Debug>: /RTCc         # -> value is assigned to a smaller data type and results
        # in a data loss >
        $<$<CONFIG:Release>:
        /Gw # -> whole program global optimization
        /GS- # -> buffer security check: no
        /GL # -> whole program optimization: enable link-time code generation (disables Zi)
        /GF # -> enable string pooling
        >
        # No manual c++11 enable for MSVC as all supported MSVC versions for cmake-init have C++11
        # implicitly enabled (MSVC >=2013)
        PUBLIC
    )
endif()

# GCC and Clang compiler options
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(RA_GLBINDINGS_COMPILE_OPTIONS
        ${RA_DEFAULT_COMPILE_OPTIONS}
        PRIVATE
        # -fno-exceptions # since we use stl and stl is intended to use exceptions, exceptions
        # should not be disabled
        -Wall
        -Wextra
        -Wunused
        -Wreorder
        -Wignored-qualifiers
        -Wmissing-braces
        -Wreturn-type
        -Wswitch
        -Wswitch-default
        -Wuninitialized
        -Wmissing-field-initializers
        $<$<CXX_COMPILER_ID:GNU>:
        -Wmaybe-uninitialized
        -Wno-unknown-pragmas
        $<$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,4.8>:
        -Wpedantic
        -Wreturn-local-addr
        >
        >
        $<$<CXX_COMPILER_ID:Clang>:
        -Wpedantic
        # -Wreturn-stack-address # gives false positives
        >
        PUBLIC
        $<$<PLATFORM_ID:Darwin>:
        -pthread
        >
        # Required for CMake < 3.1; should be removed if minimum required CMake version is raised.
        $<$<VERSION_LESS:${CMAKE_VERSION},3.1>:
        -std=c++11
        >
    )
endif()

#
# Linker options
#

set(RA_DEFAULT_LINKER_OPTIONS)

# Use pthreads on mingw and linux
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    set(RA_DEFAULT_LINKER_OPTIONS PUBLIC -pthread)
endif()

if(NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(RA_DEFAULT_LINKER_OPTIONS ${RA_DEFAULT_LINKER_OPTIONS} PUBLIC ${CMAKE_DL_LIBS})

endif()
