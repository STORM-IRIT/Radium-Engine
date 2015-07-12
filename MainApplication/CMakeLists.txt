set(TARGET radium)

set(CURRENT_DIR .)
#set(CURRENT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
#message("In dir ${CURRENT_DIR}")

# Find Qt5 stuff
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL  REQUIRED)

# Find OpenGL headers
find_package(OpenGL     REQUIRED)

set(HEADER_PATH .)
set(UI_PATH Gui/ui)

set(QT_LIBS ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5OpenGL_LIBRARIES})
set(LIBS radiumEngine radiumCore ${QT_LIBS} ${OPENGL_LIBRARIES})

file(GLOB_RECURSE SOURCE_FILES *.cpp)

file(GLOB_RECURSE HEADER_FILES ${HEADER_PATH}/*.h ${HEADER_PATH}/*.hpp)

file(GLOB_RECURSE UI_FILES ${UI_PATH}/*.ui)
qt5_wrap_ui(UI_HEADER ${UI_FILES})

# Qt5
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5OpenGL_DEFINITIONS})

include_directories(
    ${HEADER_PATH}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/Core/include
    ${CMAKE_SOURCE_DIR}/Engine/include
    ${EIGEN_INC}
)

add_executable(
    ${TARGET}
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${UI_HEADER}
)

target_link_libraries(
    ${TARGET}
    ${LIBS}
)
