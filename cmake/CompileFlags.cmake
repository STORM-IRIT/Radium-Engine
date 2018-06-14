# This file define common compile flags for all Radium projects.


# Set build configurations ====================================================
# Debug by default: Do it at the beginning to ensure proper configuration
if ( NOT MSVC )
    set( VALID_CMAKE_BUILD_TYPES "Debug Release RelWithDebInfo" )
    if ( NOT CMAKE_BUILD_TYPE )
        set( CMAKE_BUILD_TYPE Debug )
    elseif ( NOT "${VALID_CMAKE_BUILD_TYPES}" MATCHES ${CMAKE_BUILD_TYPE} )
        set( CMAKE_BUILD_TYPE Debug )
    endif()
endif()

set(UNIX_DEFAULT_CXX_FLAGS                "-Wall -Wextra  -pthread -msse3 -Wno-sign-compare -Wno-unused-parameter")
set(UNIX_DEFAULT_CXX_FLAGS_DEBUG          "-D_DEBUG -DCORE_DEBUG -g3 -ggdb")
set(UNIX_DEFAULT_CXX_FLAGS_RELEASE        "-DNDEBUG -O3")
set(UNIX_DEFAULT_CXX_FLAGS_RELWITHDEBINFO "-g3")

set(CMAKE_CXX_STANDARD 17)

# Compilation flag for each platforms =========================================

if (APPLE)
  #    message(STATUS "${PROJECT_NAME} : Compiling on Apple with compiler " ${CMAKE_CXX_COMPILER_ID})

    set(MATH_FLAG "-mfpmath=sse")
    if(RADIUM_FAST_MATH)
        if ( (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU") )
            set(MATH_FLAG "${MATH_FLAG} -ffast-math")
        endif()
    endif()

    set(CMAKE_CXX_FLAGS                "${UNIX_DEFAULT_CXX_FLAGS}                ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG          "${UNIX_DEFAULT_CXX_FLAGS_DEBUG}          ${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE        "${UNIX_DEFAULT_CXX_FLAGS_RELEASE}        ${MATH_FLAG}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${UNIX_DEFAULT_CXX_FLAGS_RELWITHDEBINFO} ${CMAKE_CXX_FLAGS_RELEASE}")

    #add_definitions( -Wno-deprecated-declarations ) # Do not warn for eigen bind being deprecated
elseif (UNIX OR MINGW)
    set(MATH_FLAG "-mfpmath=sse")
    if(RADIUM_FAST_MATH)
        if ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
            set(MATH_FLAG "${MATH_FLAG} -ffast-math")
        endif()
    endif()

    if( MINGW )
        set( EIGEN_ALIGNMENT_FLAG "-mincoming-stack-boundary=2" )
        add_definitions( -static-libgcc -static-libstdc++) # Compile with static libs
    else()
        set( EIGEN_ALIGNMENT_FLAG "" )
    endif()

    set(CMAKE_CXX_FLAGS                "${UNIX_DEFAULT_CXX_FLAGS}                ${EIGEN_ALIGNMENT_FLAG} ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG          "${UNIX_DEFAULT_CXX_FLAGS_DEBUG}          ${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE        "${UNIX_DEFAULT_CXX_FLAGS_RELEASE}        ${MATH_FLAG}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${UNIX_DEFAULT_CXX_FLAGS_RELWITHDEBINFO} -ggdb ${CMAKE_CXX_FLAGS_RELEASE}")

    # Prevent Eigen from spitting thousands of warnings with gcc 6+
    # add_definitions(-Wno-deprecated-declarations)
    if( NOT(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.4))
        add_definitions(-Wno-ignored-attributes -Wno-misleading-indentation)
    endif()
elseif (MSVC)
    # Visual studio flags breakdown
    # /GR- : no rtti ;
    # /Od  : disable optimization
    # /Ox :  maximum optimization
    # /GL : enable link time optimization
    # /Zi  : generate debug info
    # /wd4251 : -> disable warning: 'identifier': class 'type' needs to have dll-interface to be used by clients of class 'type2'
    #              See https://github.com/cginternals/glbinding/blob/master/cmake/CompileOptions.cmake and
    #              and https://github.com/cginternals/glbinding/issues/141#issuecomment-174511579
    # /EHsc  : enable exceptions
        # /bigobj: fixes C1128

    # disable secure CRT warnings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
    add_definitions(-D_USE_MATH_DEFINES)

    string (REGEX REPLACE "/GR" ""     CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    # remove library compilation flags (MT, MD, MTd, MDd
    string( REGEX REPLACE "/M(T|D)(d)*" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string( REGEX REPLACE "/M(T|D)(d)*" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string( REGEX REPLACE "/M(T|D)(d)*" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

    set(CMAKE_CXX_FLAGS                "/arch:AVX2 /MP /wd4251 /EHsc /bigobj ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG          "/D_DEBUG /DCORE_DEBUG /Od /Zi ${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE        "/DNDEBUG /Ox /fp:fast ${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi ${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_SHARED_LINKER_FLAGS      "/LTCG ${CMAKE_SHARED_LINKER_FLAGS}")

    # Problem with Qt linking
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_COMPILING_QSTRING_COMPAT_CPP")

endif()

# Additional flags depending on build options =================================

if (${RADIUM_WITH_OMP})
    find_package(OpenMP QUIET)

    if(OPENMP_FOUND)
        message(STATUS "${PROJECT_NAME} : Using OpenMP")
        add_definitions(-DCORE_USE_OMP)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    endif(OPENMP_FOUND)
else (${RADIUM_WITH_OMP})
    message(STATUS "${PROJECT_NAME} : OpenMP disabled")
    if ( (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU") )
        add_definitions( -Wno-unknown-pragmas )  # gcc/mingw prints a lot of warnings due to open mp pragmas
    endif()
endif()

if ("${CMAKE_BUILD_TYPE}"  STREQUAL "Release" )
    add_definitions(-DNO_DEBUG_INFO)
endif()

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

if (${RADIUM_ASSIMP_SUPPORT})
    add_definitions(-DIO_USE_ASSIMP)
    message(STATUS "${PROJECT_NAME} : Using Assimp loader")
else()
    message(STATUS "${PROJECT_NAME} : Assimp loader disabled")
endif()


if (${RADIUM_TINYPLY_SUPPORT})
    add_definitions(-DIO_USE_TINYPLY)
    message(STATUS "${PROJECT_NAME} : Using TinyPly loader")
else()
    message(STATUS "${PROJECT_NAME} : TinyPly loader disabled")
endif()



# Additional flags depending on system        =================================

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "${PROJECT_NAME} : 64 bits build")
else()
    message(STATUS "${PROJECT_NAME} : 32 bits build")
endif()


