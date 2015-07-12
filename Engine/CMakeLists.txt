set(TARGET radiumEngine)

set(LIBRARY_VERSION "0.1.0")
set(LIBRARY_SOVERSION "0.1")

find_package(Qt5OpenGL  REQUIRED)
find_package(OpenGL     REQUIRED)

set(CURRENT_DIR .)
set(HEADER_PATH include)

set(LIBS ${Qt5OpenGL_LIBRARIES} ${OPENGL_LIBRARIES})

file(GLOB_RECURSE SOURCE_FILES *.cpp)
set(SOURCE ${SOURCE_FILES})

file(GLOB_RECURSE HEADER_FILES	${HEADER_PATH}/*.h 
								${HEADER_PATH}/*.hpp 
								${HEADER_PATH}/*.inl)

add_definitions(${Qt5OpenGL_DEFINITIONS})

set(HEADER ${HEADER_FILES})

include_directories(
  ${HEADER_PATH}
  ${CMAKE_SOURCE_DIR}/Core/include
  ${EIGEN_INC}
)

add_library(
 ${TARGET} STATIC
 ${SOURCE}
 ${HEADER}
)

SET_TARGET_PROPERTIES(
  ${TARGET}
  PROPERTIES
  VERSION ${LIBRARY_VERSION}
  SOVERSION ${LIBRARY_SOVERSION}
)

target_link_libraries(
  ${TARGET}
  ${LIBS}
)

INSTALL(
  TARGETS ${TARGET}
  DESTINATION ${LIB_INSTALL_DIR}
)
