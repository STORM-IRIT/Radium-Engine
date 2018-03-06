#!/bin/sh

ROOT=`git rev-parse --show-toplevel`

find "${ROOT}/src/" "${ROOT}/Applications/"  "${ROOT}/Plugins/" \( -name \*.cpp -or -name \*.hpp -or -name \*.inl \) -exec clang-format-6.0 -i -style=file  "{}" \;
