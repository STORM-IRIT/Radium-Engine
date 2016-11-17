set(GL3W_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/gl3w)
set(GL3W_SRC ${GL3W_DIR}/src/gl3w.c)
set(GL3W_INC ${GL3W_DIR}/include)

if (NOT EXISTS ${GL3W_SRC})
	execute_process(COMMAND git submodule init
					COMMAND git submodule update)
	execute_process(COMMAND python gl3w_gen.py WORKING_DIRECTORY ${GL3W_DIR})
endif()

include_directories(${GL3W_INC})

find_package(OpenGL REQUIRED)
add_library(gl3w STATIC ${GL3W_SRC})
target_link_libraries(gl3w ${OPENGL_LIBRARIES})