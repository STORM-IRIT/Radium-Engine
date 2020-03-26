#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Please provide one and only one module name among [Core|Engine|IO|GuiBase]"
    exit 2
fi

BASE=$1
LOWBASE=${BASE,,}
OUTPUT="../cmake/filelist${BASE}.cmake"

echo "generate [${BASE}] filelist"
echo "-- input files from         [../src/${BASE}]"
echo "-- output vars prefiex with [${LOWBASE}]"
echo "-- output file is           [${OUTPUT}]"

rm "${OUTPUT}"
if [ "$BASE" != "IO" ]; then
    echo  "set( ${LOWBASE}_sources" >> ${OUTPUT}
    find  ../src/${BASE}/ -name \*.cpp | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> ${OUTPUT}
    echo ")"  >> ${OUTPUT}
    echo ""  >> ${OUTPUT}
    echo  "set( ${LOWBASE}_headers"  >> ${OUTPUT}
    find  ../src/${BASE}/ -name \*.hpp | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> ${OUTPUT}
    echo ")"  >> ${OUTPUT}
    echo ""  >> ${OUTPUT}
    echo  "set( ${LOWBASE}_inlines"  >> ${OUTPUT}
    find  ../src/${BASE}/ -name \*.inl | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> ${OUTPUT}
    echo ")"  >> ${OUTPUT}
fi

if [ "$BASE" = "Engine" ]; then
    echo ""  >> ${OUTPUT}
    echo  "set(${LOWBASE}_shaders"  >> ${OUTPUT}
    find  ../Shaders/ -name \*.glsl | grep -v Deprecated | cut -f 3- -d/ | sort | xargs -n1 echo "   "  >> ${OUTPUT}
    echo ")"  >> ${OUTPUT}
fi

if [ "$BASE" = "IO" ]; then
    echo  "set( ${LOWBASE}_sources"  >> ${OUTPUT}
    find  ../src/${BASE}/ -name \*.cpp | grep -v deprecated | grep -v AssimpLoader | grep -v TinyPlyLoader | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> ${OUTPUT}
    echo ")"  >> ${OUTPUT}
    echo ""      >> ${OUTPUT}
    echo  "set( ${LOWBASE}_headers"  >> ${OUTPUT}
    find  ../src/${BASE}/ -name \*.hpp | grep -v deprecated | grep -v AssimpLoader | grep -v TinyPlyLoader | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> ${OUTPUT}
    echo ")"  >> ${OUTPUT}
    echo ""  >> ${OUTPUT}
    echo  "set( ${LOWBASE}_inlines"  >> ${OUTPUT}
    find  ../src/${BASE}/ -name \*.inl | grep -v deprecated | grep -v AssimpLoader | grep -v TinyPlyLoader | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> ${OUTPUT}
    echo ")"      >> ${OUTPUT}
    echo ""      >> ${OUTPUT}
    echo "if(RADIUM_IO_DEPRECATED)"  >> ${OUTPUT}
    echo "list(APPEND ${LOWBASE}_sources"  >> ${OUTPUT}
    find  ../src/${BASE}/deprecated/ -name \*.cpp | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> ${OUTPUT}
    echo ")"  >> ${OUTPUT}
    echo ""      >> ${OUTPUT}
    echo "list(APPEND ${LOWBASE}_headers"  >> ${OUTPUT}
    find  ../src/${BASE}/deprecated/ -name \*.hpp | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> ${OUTPUT}
    echo ")"  >> ${OUTPUT}
    echo ""  >> ${OUTPUT}
    echo "list(APPEND ${LOWBASE}_inlines"  >> ${OUTPUT}
    find  ../src/${BASE}/deprecated/ -name \*.inl | cut -f 4- -d/ | sort | xargs -n1 echo "   "     >> ${OUTPUT}
    echo ")"  >> ${OUTPUT}
    echo "endif(RADIUM_IO_DEPRECATED)"  >> ${OUTPUT}
    echo ""  >> ${OUTPUT}
    echo "if( RADIUM_IO_ASSIMP )"  >> ${OUTPUT}
    echo "list(APPEND ${LOWBASE}_sources"  >> ${OUTPUT}
    find  ../src/${BASE}/AssimpLoader/ -name \*.cpp | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> ${OUTPUT}
    echo ")"  >> ${OUTPUT}
    echo ""      >> ${OUTPUT}
    echo "list(APPEND ${LOWBASE}_headers"  >> ${OUTPUT}
    find  ../src/${BASE}/AssimpLoader/ -name \*.hpp | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> ${OUTPUT}
    echo ")"  >> ${OUTPUT}
    echo "endif( RADIUM_IO_ASSIMP )"  >> ${OUTPUT}
    echo ""  >> ${OUTPUT}
    echo "if( RADIUM_IO_TINYPLY )"  >> ${OUTPUT}
    echo "list(APPEND ${LOWBASE}_sources"  >> ${OUTPUT}
    find  ../src/${BASE}/TinyPlyLoader/ -name \*.cpp | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> ${OUTPUT}
    echo ")"  >> ${OUTPUT}
    echo ""      >> ${OUTPUT}
    echo "list(APPEND ${LOWBASE}_headers"  >> ${OUTPUT}
    find  ../src/${BASE}/TinyPlyLoader/ -name \*.hpp | cut -f 4- -d/ | sort | xargs -n1 echo "   "  >> ${OUTPUT}
    echo ")"     >> ${OUTPUT}
    echo "endif( RADIUM_IO_TINYPLY )"  >> ${OUTPUT}
fi
