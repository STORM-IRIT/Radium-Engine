#!/bin/sh

# list include files and count how many time there included for current source dir.
# call from source dir for which you want to list includes

PWD=$(pwd)
BASE=$(basename "${PWD}")
LIST1=$(mktemp)
LIST2=$(mktemp)

find . \( -iname "*.cpp" -or -iname "*.hpp" -or -iname "*.inl" \) -exec grep \#include {} \; > "${LIST1}"
grep -v "${BASE}"  "${LIST1}" | grep -v \" | sort > "${LIST2}"
sed -i 's/\/\/.*//g' "${LIST2}"
sed -i 's/[ ]*$//g' "${LIST2}"
sed -i 's/^[ ]*//g' "${LIST2}"
uniq -c < "${LIST2}" | sort -nr

rm "${LIST1}" "${LIST2}"
