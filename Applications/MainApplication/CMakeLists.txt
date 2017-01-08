# Build MainApplication
set(app_target main-app)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(OpenGL     REQUIRED)
find_package(Qt5Core    REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL  REQUIRED)

set( Qt5_LIBRARIES
     ${Qt5Core_LIBRARIES}
     ${Qt5Widgets_LIBRARIES}
     ${Qt5OpenGL_LIBRARIES} )


if(APPLE)
    FIND_LIBRARY(CARBON_LIBRARY Carbon)
    set( PLATFORM_LIBS ${CARBON_LIBRARY} )
elseif( UNIX )
    set( PLATFORM_LIBS X11 xcb )
else()
    set( PLATFORM_LIBS "" )
endif()


include(CompileFlags)
set(app_libs
    ${RA_GUIBASE_LIB}           # gui
    ${RA_ENGINE_LIB}            # engine
    ${RA_CORE_LIB}              # core
    ${Qt5_LIBRARIES}                # the Qt beast
    ${OPENGL_LIBRARIES}
    ${PLATFORM_LIBS}
    ${ASSIMP_LIB}
      )

file(GLOB_RECURSE app_sources *.cpp)
file(GLOB_RECURSE app_headers *.h *.hpp)
file(GLOB_RECURSE app_inlines *.inl)
file(GLOB_RECURSE app_uis     *.ui)
qt5_wrap_ui(app_uis_moc ${app_uis})

file(GLOB resources ${CMAKE_CURRENT_SOURCE_DIR}/../../*.qrc)

include_directories(
    .
    ${RADIUM_INCLUDE_DIR}
    ${EIGEN_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_DIR}

    ${CMAKE_CURRENT_BINARY_DIR} # Moc
    )

add_executable(
    ${app_target}
    ${app_sources}
    ${app_headers}
    ${app_inlines}
    ${app_uis_moc}
    ${resources}
    )

target_link_libraries(
    ${app_target}
    ${app_libs}
    )

#install( DIRECTORY . DESTINATION include FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl" PATTERN "*.h" )
#install( FILES ${engine_shaders} DESTINATION Shaders )
#install( DIRECTORY ${CMAKE_SOURCE_DIR}/Assets/. DESTINATION Assets )
#install( TARGETS ${app_target} ${core_target} ${engine_target} ${guibase_target}
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib )

if (MSVC)
    #This will work from CMake 3.6
    set_property( DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${app_target})
endif(MSVC)
