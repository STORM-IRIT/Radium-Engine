set(target coretests)

file(GLOB sources *.cpp)
file(GLOB headers *.hpp)
file(GLOB inlines *.inl)

add_executable(
 ${target}
 ${sources}
 ${headers}
 ${inlines}
)

target_link_libraries(
 ${target}
 radiumCore
)
