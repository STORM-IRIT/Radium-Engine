#!/bin/bash

declare -a repositories=(
    "https://github.com/STORM-IRIT/Radium-PluginExample.git"
    "https://gitlab.com/Storm-IRIT/radium-official-plugins/postprocesssubdivplugin.git"
#    "https://github.com/STORM-IRIT/Radium-NanoGUIApp.git"
#    "https://github.com/STORM-IRIT/Radium-AppExample.git"
    )

# \param $1 input a repository url
getRepositoryName ()
{
    local spliturl=($(echo $1 | tr '/' "\n")) # split by /
    local supername="${spliturl[${#spliturl[@]}-1]}" # get name.git
    echo ${supername%????} # remove .git
}

# Stop on error
set -e

cd $CHECKOUT_PATH

NEW_UUID=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1)
PLUGIN_FOLDER="plugins-$NEW_UUID"

mkdir $PLUGIN_FOLDER
cd $PLUGIN_FOLDER

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
   cmake .. -DCMAKE_MODULE_PATH=$CHECKOUT_PATH/cmake -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_PREFIX_PATH=${PREFIX_PATH}
   make -j 4

   echo "Plugin $repoName done"
done

echo "All plugins have been compiled."

