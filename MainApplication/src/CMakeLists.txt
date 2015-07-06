set(TARGET radium)

find_package(OpenGL     REQUIRED)
find_package(Eigen3     REQUIRED)

set(HEADER_PATH .)

set(LIBS radiumEngine ${OPENGL_LIBRARIES})

file(GLOB SOURCE_FILES *.cpp)
set(SOURCE ${SOURCE_FILES})

file(GLOB HEADER_FILES ${HEADER_PATH}/*.h ${HEADER_PATH}/*.hpp)
set(HEADER ${HEADER_FILES})

include_directories(
    ${HEADER_PATH}
    ${CMAKE_SOURCE_DIR}/Engine/include
    ${EIGEN3_INCLUDE_DIR}
)

add_executable(
    ${TARGET}
    ${SOURCE}
    ${HEADER}
)

target_link_libraries(
    ${TARGET}
    ${LIBS}
)

