# This file define common compile flags for all Radium projects.

# Compilation flag for each platforms =========================================

if (APPLE)
    # No openmp on MacosX Clang (TODO, find better compiler identification)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_FLAGS                "-Wall -Wextra -msse3 -Wno-sign-compare -Wno-unused-parameter -fno-exceptions ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG          "-D_DEBUG -DCORE_DEBUG -g3 -ggdb ${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE        "-DNDEBUG -O3 -ffast-math -mfpmath=sse")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 ${CMAKE_CXX_FLAGS_RELEASE}")

    add_definitions( -Wno-deprecated-declarations ) # Do not warn for eigen bind being deprecated
elseif (UNIX OR MINGW)
    if ((${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"))
        set(OMP_FLAG "-fopenmp=libiomp5")
        set(MATH_FLAG "-mfpmath=sse")
    else()
        set(OMP_FLAG "-fopenmp -ftree-vectorize")
        set(MATH_FLAG "-mfpmath=sse -ffast-math")
    endif()

    if ("${USE_OMP}" STREQUAL "False")
        set (OMP_FLAG "")
        add_definitions( -Wno-unknown-pragmas )  # gcc/mingw prints a lot of warnings due to open mp pragmas
    endif()

    if( MINGW )
        set( EIGEN_ALIGNMENT_FLAG "-mincoming-stack-boundary=2" )
    else()
        set( EIGEN_ALIGNMENT_FLAG "" )
    endif()

    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_FLAGS                "-Wall -Wextra  -pthread -msse3 -Wno-sign-compare -Wno-unused-parameter -fno-exceptions -fPIC ${OMP_FLAG} ${EIGEN_ALIGNMENT_FLAG} ${CMAKE_CXX_FLAGS}")
# fPIC ?    set(CMAKE_CXX_FLAGS                "-Wall -Wextra  -pthread -msse3 -Wno-sign-compare -Wno-unused-parameter -fno-exceptions ${OMP_FLAG} ${EIGEN_ALIGNMENT_FLAG} ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG          "-D_DEBUG -DCORE_DEBUG -g3 -ggdb ${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE        "-DNDEBUG -O3 ${MATH_FLAG}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -ggdb ${CMAKE_CXX_FLAGS_RELEASE}")

    # Prevent Eigen from spitting thousands of warnings with gcc 6+
    add_definitions(-Wno-deprecated-declarations)
    if( NOT(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.4))
        add_definitions(-Wno-ignored-attributes -Wno-misleading-indentation)
    endif()

  if (MINGW)
      add_definitions( -static-libgcc -static-libstdc++) # Compile with static libs
  endif()
elseif (MSVC)
    # Visual studio flags breakdown
    # /GR- : no rtti ; /Ehs-c- : no exceptions
    # /Od  : disable optimization
    # /Ox :  maximum optimization
    # /GL : enable link time optimization
    # /Zi  : generate debug info

    # remove exceptions from default args
    add_definitions(-D_HAS_EXCEPTIONS=0)
    string (REGEX REPLACE "/EHsc *" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string (REGEX REPLACE "/GR" ""     CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    # remove library compilation flags (MT, MD, MTd, MDd
    string( REGEX REPLACE "/M(T|D)(d)*" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string( REGEX REPLACE "/M(T|D)(d)*" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string( REGEX REPLACE "/M(T|D)(d)*" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

    if ("${USE_OMP}" STREQUAL "True")
        set (OMP_FLAG "/openmp")
    else()
        set (OMP_FLAG "")
    endif()

    set(CMAKE_CXX_FLAGS                "/arch:AVX2 /GR- /EHs-c- /MP ${OMP_FLAG} ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG          "/D_DEBUG /DCORE_DEBUG /Od /Zi ${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE        "/DNDEBUG /Ox /fp:fast ${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi ${CMAKE_CXX_FLAGS_RELEASE}")
endif()

# Additional flags depending on build options =================================

if ("${USE_DOUBLE}" STREQUAL "True")
  add_definitions(-DCORE_USE_DOUBLE)
  message("Using double precision.")
else()
  message("Using single precision.")
endif()

if ("${ENABLE_FANCY_GL}" STREQUAL "False")
  add_definitions(-DNO_TRANSPARENCY)
  message("Fancy OpenGL Effects are disabled")
endif()

if ("${USE_OMP}" STREQUAL "True")
    add_definitions(-DCORE_USE_OMP)
    message("Using OpenMP")
else()
    message("OpenMP disabled")
endif()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    message("64 bits build")
else()
    message("32 bits build")
endif()


# Set build configurations ====================================================

# Debug by default


set( VALID_CMAKE_BUILD_TYPES "Debug Release RelWithDebInfo" )
if ( NOT "${VALID_CMAKE_BUILD_TYPES}" MATCHES ${CMAKE_BUILD_TYPE} )
    set( CMAKE_BUILD_TYPE Debug )
endif()
