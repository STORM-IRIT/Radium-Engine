git submodule radium in my own project

for instance Radium in ./myproject/external/Radium-Engine

.gitmodules
[submodule "Radium-Engine"]
        path = extern/Radium-Engine
        url = https://github.com/AGGA-IRIT/Radium-Engine
        branch = master

git submodule update --init --recursive
cd extern/Radium-Egine
git pull origin master

in project master CMakeLists.txt
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/Radium-Engine/CMakeModules)

in subdir that compile something that need's Radium :

find_package(Radium REQUIRED)