#!/bin/sh
set -x
ROOT=`git rev-parse --show-toplevel`
rm "${ROOT}/.git/hooks/pre-commit"
rm "${ROOT}/.clang-format"
