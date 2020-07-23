#!/bin/sh

ROOT=`git rev-parse --show-toplevel`

file_or_link () {
    if  [ ! -e "$1" ] && [ ! -L "$1" ]; then
	return 0
    else
	return 1
    fi
}

if file_or_link "${ROOT}/.git/hooks/pre-commit" ; then
    echo "create symlink for pre-commit hook";
    ln -rs "${ROOT}/scripts/pre-commit-clang-format.sh" "${ROOT}/.git/hooks/pre-commit"
    chmod +x "${ROOT}/scripts/pre-commit-clang-format.sh"
else
    echo "pre-commit hook already present, please remove .git/hooks/pre-commit";
fi

# now .clang-format is in git repo for auto formatiing
#if file_or_link "${ROOT}/.clang-format" ; then
#    echo "create symlink for clang-format style file";
#    ln -rs "${ROOT}/scripts/clang-format" "${ROOT}/.clang-format"
#else
#    echo "clang-format link already present, please remove .clang-format";
#fi
