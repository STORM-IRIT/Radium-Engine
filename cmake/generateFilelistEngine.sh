#!/bin/bash
echo  "set( engine_sources"
find  ../src/Engine/ -name \*.cpp | cut -f 4- -d/ | sort
echo ")"


echo  "set( engine_headers"
find  ../src/Engine/ -name \*.hpp | cut -f 4- -d/ | sort
echo ")"

echo  "set( engine_inlines"
find  ../src/Engine/ -name \*.inl | cut -f 4- -d/ | sort
echo ")"


echo  "set(engine_shaders"
find  ../Shaders/ -name \*.glsl | cut -f 3- -d/ | sort
echo ")"
