project (Radium-Tests)

#############################################
## testing files
set (testing_include_dir ${CMAKE_CURRENT_SOURCE_DIR})
set (testing_SRCS
    ${testing_include_dir}/Tests.hpp
)


add_subdirectory(CoreTests)
