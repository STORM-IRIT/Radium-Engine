#!/bin/sh

ROOT=`git rev-parse --show-toplevel`

find "${ROOT}/src/" "${ROOT}/tests/"  "${ROOT}/Shaders/" \( -name \*.cpp -or -name \*.hpp -or -name \*.inl -or -name \*.glsl \) -exec clang-format -i -style=file  "{}" \;
