# This file define common compile flags for all Radium projects.

# Compilation flag for each platforms =========================================

if (APPLE)
#    message(STATUS "${PROJECT_NAME} : Compiling on Apple with compiler " ${CMAKE_CXX_COMPILER_ID})

    if ( (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU") )
        set(OMP_FLAG "-fopenmp -ftree-vectorize")
        set(MATH_FLAG "-mfpmath=sse -ffast-math")
    else()
        set(MATH_FLAG "-mfpmath=sse")
        set(OMP_FLAG "")
    endif()

    if (NOT ${RADIUM_WITH_OMP})
        set (OMP_FLAG "")
        add_definitions( -Wno-unknown-pragmas )  # gcc/mingw prints a lot of warnings due to open mp pragmas
    else()
        set(RADIUM_WITH_OMP ON)
    endif()

    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_FLAGS                "-Wall -Wextra  -pthread -msse3 -Wno-sign-compare -Wno-unused-parameter -fno-exceptions -fPIC ${OMP_FLAG} ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG          "-D_DEBUG -DCORE_DEBUG -g3 -ggdb ${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE        "-DNDEBUG -O3 ${MATH_FLAG}")
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

    if (NOT ${RADIUM_WITH_OMP})
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
    # disable secure CRT warnings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
    string (REGEX REPLACE "/EHsc *" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string (REGEX REPLACE "/GR" ""     CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    # remove library compilation flags (MT, MD, MTd, MDd
    string( REGEX REPLACE "/M(T|D)(d)*" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string( REGEX REPLACE "/M(T|D)(d)*" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string( REGEX REPLACE "/M(T|D)(d)*" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

    if ("${RADIUM_WITH_OMP}" STREQUAL "True")
        set (OMP_FLAG "/openmp")
    else()
        set (OMP_FLAG "")
    endif()

    set(CMAKE_CXX_FLAGS                "/arch:AVX2 /GR- /EHs-c- /MP ${OMP_FLAG} ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG          "/D_DEBUG /DCORE_DEBUG /Od /Zi ${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE        "/DNDEBUG /Ox /fp:fast ${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi ${CMAKE_CXX_FLAGS_RELEASE}")

    # Problem with Qt linking
    # FIXME(Charly): Not sure if this should be done on Linux
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_COMPILING_QSTRING_COMPAT_CPP")

endif()

# Additional flags depending on build options =================================

if (${RADIUM_WITH_DOUBLE_PRECISION})
  add_definitions(-DCORE_USE_DOUBLE)
  message(STATUS "${PROJECT_NAME} : Using double precision.")
else()
  message(STATUS "${PROJECT_NAME} : Using single precision.")
endif()

if (NOT ${RADIUM_WITH_FANCY_GL})
  add_definitions(-DNO_TRANSPARENCY)
  message(STATUS "${PROJECT_NAME} : Fancy OpenGL Effects are disabled")
endif()

if (${RADIUM_WITH_OMP})
    add_definitions(-DCORE_USE_OMP)
    message(STATUS "${PROJECT_NAME} : Using OpenMP")
else()
    message(STATUS "${PROJECT_NAME} : OpenMP disabled")
endif()

if (${RADIUM_WITH_TEXTURES})
    message(STATUS "${PROJECT_NAME} : Textures will be loaded")
    add_definitions(-DRADIUM_WITH_TEXTURES)
else()
    message(STATUS "${PROJECT_NAME} : Textures won't be loaded, use RADIUM_WITH_TEXTURES flag to enable them.")
endif()

if (${RADIUM_WITH_PROFILING})
    add_definitions(-DALLOW_PROFILING)
    message(STATUS "${PROJECT_NAME} : Profiling is enabled")
endif()

if (${RADIUM_WARNINGS_AS_ERRORS})
    message(STATUS "${PROJECT_NAME} : Enabling warnings as errors")
    if ( APPLE OR ( UNIX OR MINGW ) )
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    elseif (MSVC)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
    endif()
endif()

if (${RADIUM_FORCE_ASSERTS})
    message( STATUS "Enabling asserts")
    add_definitions(-DCORE_USE_ASSERT)
endif()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "${PROJECT_NAME} : 64 bits build")
else()
    message(STATUS "${PROJECT_NAME} : 32 bits build")
endif()


# Set build configurations ====================================================

# Debug by default

if ( NOT MSVC )
    set( VALID_CMAKE_BUILD_TYPES "Debug Release RelWithDebInfo" )
    if ( NOT CMAKE_BUILD_TYPE )
        set( CMAKE_BUILD_TYPE Debug )
    elseif ( NOT "${VALID_CMAKE_BUILD_TYPES}" MATCHES ${CMAKE_BUILD_TYPE} )
        set( CMAKE_BUILD_TYPE Debug )
    endif()
endif()
