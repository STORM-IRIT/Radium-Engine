set(target coretests)

file(GLOB_RECURSE sources *.cpp)
file(GLOB_RECURSE headers *.hpp)
file(GLOB_RECURSE inlines *.inl)

add_executable(
 ${target}
 ${sources}
 ${headers}
 ${inlines}
)

target_link_libraries(
 ${target}
 radiumCore
 radiumTestsUtils
)

add_dependencies( ${target} radiumCore radiumTestsUtils )
