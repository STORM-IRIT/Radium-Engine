#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Please provide one and only one module name (e.g. Core) that corresponds to the directory name under src/ (e.g. src/Core)."
    exit 2
fi

BASE=$1
UPBASE=${BASE^^}
OUTPUT="../src/${BASE}/filelist.cmake"

echo "generate [${BASE}] filelist"
echo "-- input files from         [../src/${BASE}]"
echo "-- output vars prefix       [${UPBASE}]"
echo "-- output file is           [${OUTPUT}]"

rm -f "${OUTPUT}"

echo  "# ----------------------------------------------------" >> "${OUTPUT}"
echo  "# This file can be generated from a script:" >> "${OUTPUT}"
echo  "# To do so, run \"$0 ${BASE}\" " >> "${OUTPUT}"
echo  "# from ./scripts directory" >> "${OUTPUT}"
echo  "# ----------------------------------------------------" >> "${OUTPUT}"
echo ""  >> "${OUTPUT}"


function genList(){
    ext=$1
    suffix=$2
    L=$(find  "../src/${BASE}/" -name "*.${ext}")
    if [ ! -z "$L" ]
    then
        echo  "set(${UPBASE}_${suffix}" >> "${OUTPUT}"
        echo "${L}" | cut -f 4- -d/ | sort | grep -v 'pch.hpp' | xargs -n1 echo "   "  >> "${OUTPUT}"
        echo ")" >> "${OUTPUT}"
        echo ""  >> "${OUTPUT}"
    fi
}

function genListIo(){
    ext=$1
    suffix=$2
    L=$(find  "../src/${BASE}/" -name "*.${ext}")
    if [ ! -z "$L" ]
    then
        echo  "set(${UPBASE}_${suffix}" >> "${OUTPUT}"
        echo "${L}"  | grep -v 'pch.hpp' | grep -v deprecated | grep -v AssimpLoader | grep -v TinyPlyLoader | grep -v VolumesLoader | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> "${OUTPUT}"
        echo ")" >> "${OUTPUT}"
        echo ""  >> "${OUTPUT}"
    fi
}

function genListIoAppend(){
    ext=$1
    suffix=$2
    L=$(find  "../src/${BASE}/deprecated/" -name "*.${ext}")
    if [ ! -z "$L" ]
    then
        echo "list(APPEND ${UPBASE}_${suffix}"  >> "${OUTPUT}"
        echo "${L}" | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> "${OUTPUT}"
        echo ")" >> "${OUTPUT}"
        echo ""  >> "${OUTPUT}"
    fi
}


function genListIoAppendSubdir(){
    ext=$1
    suffix=$2
    subdir=$3
    L=$(find  "../src/${BASE}/${subdir}/" -name "*.${ext}")
    if [ ! -z "$L" ]
    then
        echo "list(APPEND ${UPBASE}_${suffix}"  >> "${OUTPUT}"
        echo "${L}" | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> "${OUTPUT}"
        echo ")" >> "${OUTPUT}"
        echo ""  >> "${OUTPUT}"
    fi
}

if [ "$BASE" != "IO" ]; then
    genList "cpp" "SOURCES"
    genList "hpp" "HEADERS"
    genList "inl" "INLINES"
    genList "ui"   "UIS"
    genList "qrc"  "RESOURCES"
fi

if [ "$BASE" = "PluginBase" ]; then
    genList "json" "JSON"
fi

if [ "$BASE" = "Engine" ]; then
    echo ""  >> "${OUTPUT}"
    echo  "set(${UPBASE}_SHADERS"  >> "${OUTPUT}"
    find  "../Shaders/" -name "*.glsl" | grep -v Deprecated | cut -f 3- -d/ | sort | xargs -n1 echo "   "  >> "${OUTPUT}"
    echo ")"  >> "${OUTPUT}"
fi

if [ "$BASE" = "IO" ]; then
    genListIo "cpp" "SOURCES"
    genListIo "hpp" "HEADERS"
    genListIo "inl" "INLINES"
    genListIo "ui"   "UIS"
    genListIo "qrc"  "RESOURCES"
    echo "if(RADIUM_IO_DEPRECATED)"  >> "${OUTPUT}"
    genListIoAppend "cpp" "SOURCES"
    genListIoAppend "hpp" "HEADERS"
    genListIoAppend "inl" "INLINES"
    echo "endif(RADIUM_IO_DEPRECATED)"  >> "${OUTPUT}"
    echo ""  >> "${OUTPUT}"
    echo "if( RADIUM_IO_ASSIMP )"  >> "${OUTPUT}"
    genListIoAppendSubdir "cpp" "SOURCES" "AssimpLoader"
    genListIoAppendSubdir "hpp" "HEADERS" "AssimpLoader"
    genListIoAppendSubdir "inl" "INLINES" "AssimpLoader"
    echo "endif( RADIUM_IO_ASSIMP )"  >> "${OUTPUT}"
    echo ""  >> "${OUTPUT}"
    echo "if( RADIUM_IO_TINYPLY )"  >> "${OUTPUT}"
    genListIoAppendSubdir "cpp" "SOURCES" "TinyPlyLoader"
    genListIoAppendSubdir "hpp" "HEADERS" "TinyPlyLoader"
    genListIoAppendSubdir "inl" "INLINES" "TinyPlyLoader"
    echo "endif( RADIUM_IO_TINYPLY )"  >> "${OUTPUT}"
    echo ""  >> "${OUTPUT}"
    echo "if( RADIUM_IO_VOLUMES )"  >> "${OUTPUT}"
    genListIoAppendSubdir "cpp" "SOURCES" "VolumesLoader"
    genListIoAppendSubdir "hpp" "HEADERS" "VolumesLoader"
    genListIoAppendSubdir "inl" "INLINES" "VolumesLoader"
    echo "endif( RADIUM_IO_VOLUMES )"  >> "${OUTPUT}"
fi
cmake-format -i "${OUTPUT}"
