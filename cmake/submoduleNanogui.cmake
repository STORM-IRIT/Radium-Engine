
# We include this guy only if GLFW is installed on the system
find_package(glfw QUIET)

if (GLFW_FOUND)
    message(STATUS "GLFW found, enabling nanogui compilation")
    # here is defined the way we want to compile nanogui
    ExternalProject_Add(
            nanogui

            # where the source will live
            SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/nanogui"

            # override default behaviours
            UPDATE_COMMAND ""

            # set the installatin to root
            # INSTALL_COMMAND cmake -E echo "Skipping install step."
            INSTALL_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}"
            CMAKE_ARGS
            -DNANOGUI_BUILD_EXAMPLE=OFF
            -DNANOGUI_BUILD_SHARED=ON
            -DNANOGUI_BUILD_PYTHON=OFF
            -DNANOGUI_INSTALL=ON
            -DNANOGUI_USE_GLAD=OFF
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_BUILD_TYPE=${RADIUM_SUBMODULES_BUILD_TYPE}
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            STEP_TARGETS install
            EXCLUDE_FROM_ALL TRUE
    )

    add_custom_target(nanogui_lib
            DEPENDS nanogui
            )
    # --------------------------------------------------------------------------

    set(NANOGUI_INCLUDE_DIR ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include/)
    if( APPLE )
        set(NANOGUI_LIBRARY "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libnanogui.dylib")
    elseif ( UNIX )
        set(NANOGUI_LIBRARY "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libnanogui.so")
    elseif (MINGW)
        set(NANOGUI_LIBRARY "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libnanogui.dll.a")
    elseif( MSVC )
        # fixme: test on windows
        set(NANOGUI_LIBRARY "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libnanogui.lib")
    endif()

    set(NANOGUI_FOUND TRUE)
endif()
