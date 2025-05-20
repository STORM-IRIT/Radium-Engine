include(FetchContent)

set(CATCH_INSTALL_DOCS OFF CACHE BOOL "Install documentation alongside library")

FetchContent_Declare(
    Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
