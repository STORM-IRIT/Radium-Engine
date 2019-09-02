#!/bin/bash
# git pre-commit hook that runs an clang-format stylecheck.
# Features:
#  - abort commit when commit does not comply with the style guidelines
#  - create a patch of the proposed style changes
# Modifications for clang-format by rene.milk@wwu.de

# Getted  by dlyr from https://gitlab.cern.ch/GeantV/geant/tree/master/, remixed with original version
# https://github.com/godotengine/godot/blob/master/misc/hooks/pre-commit-clang-format
# modifications for clang-format by rene.milk@wwu.de
# This file is part of a set of unofficial pre-commit hooks available
# at github.
# Link:    https://github.com/githubbrowser/Pre-commit-hooks
# Contact: David Martin, david.martin.mailbox@googlemail.com

# Some quality of life modifications made for Godot Engine.

##################################################################
# SETTINGS
# set path to clang-format binary
CLANG_FORMAT=`which clang-format`

# remove any older patches from previous commits. Set to true or false.
# DELETE_OLD_PATCHES=false
DELETE_OLD_PATCHES=true

# Only parse files with the extensions in FILE_EXTS. Set to true or false.
# If false every changed file in the commit will be parsed with clang-format.
# If true only files matching one of the extensions are parsed with clang-format.
# PARSE_EXTS=true
PARSE_EXTS=true

# File types to parse. Only effective when PARSE_EXTS is true.
# FILE_EXTS=".c .h .cpp .hpp"
FILE_EXTS=".c .h .cpp .hpp .cc .hh .cxx .m .inl"

# Use pygmentize instead of cat to parse diff with highlighting.
# Install it with `pip install pygments` (Linux) or `easy_install Pygments` (Mac)
# READER="pygmentize -l diff"
READER=cat

##################################################################
# There should be no need to change anything below this line.

# Canonicalize by recursively following every symlink in every component of the
# specified filename.  This should reproduce the results of the GNU version of
# readlink with the -f option.
#
# Reference: http://stackoverflow.com/questions/1055671/how-can-i-get-the-behavior-of-gnus-readlink-f-on-a-mac
canonicalize_filename () {
    local target_file="$1"
    local physical_directory=""
    local result=""

    # Need to restore the working directory after work.
    local working_dir="`pwd`"

    cd -- "$(dirname -- "$target_file")"
    target_file="$(basename -- "$target_file")"

    # Iterate down a (possible) chain of symlinks
    while [ -L "$target_file" ]
    do
        target_file="$(readlink -- "$target_file")"
        cd -- "$(dirname -- "$target_file")"
        target_file="$(basename -- "$target_file")"
    done

    # Compute the canonicalized name by finding the physical path
    # for the directory we're in and appending the target file.
    physical_directory="`pwd -P`"
    result="$physical_directory/$target_file"

    # restore the working directory after work.
    cd -- "$working_dir"

    echo "$result"
}

# exit on error
set -e

# check whether the given file matches any of the set extensions
matches_extension() {
    local filename=$(basename "$1")
    local extension=".${filename##*.}"
    local ext

    for ext in $FILE_EXTS; do [[ "$ext" == "$extension" ]] && return 0; done

    return 1
}

# necessary check for initial commit
if git rev-parse --verify HEAD >/dev/null 2>&1 ; then
    against=HEAD
else
    # Initial commit: diff against an empty tree object
    against=4b825dc642cb6eb9a060e54bf8d69288fbee4904
fi

if [ ! -x "$CLANG_FORMAT" ] ; then
    printf "Error: clang-format executable not found.\n"
    printf "Set the correct path in $(canonicalize_filename "$0").\n"
    exit 1
fi

# create a random filename to store our generated patch
prefix="pre-commit-clang-format"
suffix="$(date +%s)"
patch="/tmp/$prefix-$suffix.patch"

# clean up any older clang-format patches
$DELETE_OLD_PATCHES && rm -f /tmp/$prefix*.patch

# create one patch containing all changes to the files
git diff-index --cached --diff-filter=ACMR --name-only $against -- | while read file;
do
    # ignore file if we do check for file extensions and the file
    # does not match any of the extensions specified in $FILE_EXTS
    if $PARSE_EXTS && ! matches_extension "$file"; then
        continue;
    fi

    # clang-format our sourcefile, create a patch with diff and append it to our $patch
    # The sed call is necessary to transform the patch from
    #    --- $file timestamp
    #    +++ - timestamp
    # to both lines working on the same file and having a/ and b/ prefix.
    # Else it can not be applied with 'git apply'.
    "$CLANG_FORMAT" -style=file "$file" | \
        diff -u "$file" - | \
        sed -e "1s|--- |--- a/|" -e "2s|+++ -|+++ b/$file|" >> "$patch"
done

# if no patch has been generated all is ok, clean up the file stub and exit
if [ ! -s "$patch" ] ; then
    printf "Files in this commit comply with the clang-format rules.\n"
    rm -f "$patch"
    exit 0
fi

# a patch has been created, notify the user and exit
printf "\nThe following differences were found between the code to commit "
printf "and the clang-format rules:\n\n"
$READER "$patch"
printf "\n"

# Allows us to read user input below, assigns stdin to keyboard
exec < /dev/tty


git apply $patch;
git diff-index --cached --diff-filter=ACMR --name-only $against -- | while read file;
do git add $file;
done
printf "The patch was applied and the changed files staged. You can now commit.\n\n";

exit 1 # we don't commit in any case
