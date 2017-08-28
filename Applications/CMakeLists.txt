add_subdirectory(MainApplication)
add_subdirectory(Test0)

if(${NANOGUI_FOUND})
    add_subdirectory(TestGLFW)
endif()
