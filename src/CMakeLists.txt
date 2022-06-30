# Adds convenience methods, see cmake/CleanCppExtensions.cmake
include(CleanCppExtensions)

# Creates options to turn on Sanitizers, see cmake/Sanitizers.cmake
include(Sanitizers)

# ------------------------------------------------------------------------------
# Compilation option
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/bigobj)
    endif()
else()
    add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wunused -pedantic)
endif()

# ------------------------------------------------------------------------------
# Libraries dependencies
find_package(OpenMP QUIET)

# -----------------------------------------------------------------------------------
function(cat IN_FILE OUT_FILE)
    file(READ ${IN_FILE} CONTENTS)
    file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()

# Prepare cmake package
include(CMakePackageConfigHelpers)
set(ConfigPackageLocation lib/cmake/Radium)

# -----------------------------------------------------------------------------------
# Include sources and declare components
set(PACKAGE_COMPONENTS "")

if(RADIUM_GENERATE_LIB_CORE)
    add_subdirectory(Core)
    add_dependencies(RadiumLibs Core)
endif()

if(RADIUM_GENERATE_LIB_ENGINE)
    add_subdirectory(Engine)
    add_dependencies(RadiumLibs Engine)
endif()

if(RADIUM_GENERATE_LIB_IO)
    add_subdirectory(IO)
    add_dependencies(RadiumLibs IO)
endif()

if(RADIUM_GENERATE_LIB_PLUGINBASE)
    add_subdirectory(PluginBase)
    add_dependencies(RadiumLibs PluginBase)
endif()

if(RADIUM_GENERATE_LIB_GUI)
    add_subdirectory(Gui)
    add_dependencies(RadiumLibs Gui)
endif()

if(RADIUM_GENERATE_LIB_HEADLESS)
    add_subdirectory(Headless)
    add_dependencies(RadiumLibs Headless)
endif()

# -----------------------------------------------------------------------------------
# Prepare the RadiumConfigTemplate.in file:
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/RadiumConfigTemplate.in "")

# Set the general header in RadiumConfigTemplate.in
cat(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/CompilerVersion.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/RadiumConfigTemplate.in
)
cat(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/RadiumConfigTemplate.in)

# Generate cmake package
configure_radium_package(
    NAME Radium PACKAGE_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/RadiumConfigTemplate.in
                               PACKAGE_VERSION ${Radium_VERSION}
)

# install general scripts
install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RadiumSetupFunctions.cmake"
          "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/QtFunctions.cmake"
          "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/FindFilesystem.cmake"
          "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RPath.cmake"
    DESTINATION ${ConfigPackageLocation}
)

# copy scripts in the buildtree to be available when searching package in the buildtree
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RadiumSetupFunctions.cmake"
         "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/QtFunctions.cmake"
         "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/FindFilesystem.cmake"
         "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/RPath.cmake"
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

if(MSVC OR MSVC_IDE OR MINGW)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Windeployqt.cmake"
            DESTINATION ${ConfigPackageLocation}
    )
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Windeployqt.cmake"
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

# propagate global to parent
set(LocalDependencies ${LocalDependencies} PARENT_SCOPE)

# -------------------------------------------------------------------------------
# Copy MINGW needed libraries for building on windows

if(MINGW)
    message(STATUS "MinGW detected")
    get_filename_component(GCC_PATH ${CMAKE_C_COMPILER} PATH)
    if(${GCC_PATH} MATCHES "mingw64/bin")
        set(libgcc "libgcc_s_seh-1.dll") # 64bit
        message(STATUS "  64bit dlls is building")
    else()
        set(libgcc "libgcc_s_dw2-1.dll") # 32bit
        message(STATUS "  32bit dlls is building")
    endif()

    install(FILES ${GCC_PATH}/${libgcc} ${GCC_PATH}/libstdc++-6.dll DESTINATION ./bin/)
endif(MINGW)

# ------------------------------------------------------------------------------
# Other MISC targets

# Adds misc targets: format, cppcheck, tidy, see cmake/CleanCppExtensions.cmake
set(FORMAT_STYLE_FILENAME ${RADIUM_SRC_DIR}/scripts/clang-format)
