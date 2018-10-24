#!/bin/bash

declare -a repositories=(
    "https://github.com/STORM-IRIT/Radium-PluginExample.git"
    )

# \param $1 input a repository url
getRepositoryName ()
{
    local spliturl=($(echo $1 | tr '/' "\n"))
    local supername="${spliturl[${#spliturl[@]}-1]}"
    echo ${supername: : -4}
}

# Stop on error
set -e

cd $CHECKOUT_PATH

mkdir plugins
cd plugins

 # Loop over plugins and: fetch, compile
for repoUrl in "${repositories[@]}"
do
   repoName=$(getRepositoryName "$repoUrl")
   echo "Processing $repoName"

   # fetch repo
   git clone $repoUrl $repoName
   cd $repoName
   git submodule init
   git submodule update --recursive

   # build plugin
   mkdir build
   cd build

   # \FIXME the CMAKE_MODULE_PATH needs to be updated once the installation procedure is working
   cmake .. -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_PREFIX_PATH=${PREFIX_PATH} -DCMAKE_MODULE_PATH=$CHECKOUT_PATH/Radium-Engine/cmake
   make -j 4

done
