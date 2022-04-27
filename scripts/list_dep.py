#!/usr/bin/python3
# run from Radium-Engine/external directory
# This files generates information about Radium externals, and is used to generate the Documentation,

from pathlib import Path
import re

p = Path('.')

filenames=list(p.glob('**/CMakeLists.txt'))

name = re.compile(r'Add\([\s]*(.*)')
git = re.compile(r'GIT_REPOSITORY (.*)')
tag = re.compile(r'[^\)]*GIT_TAG (.*)')
option = re.compile(r'[^\)](-D\S*)')
installdir = re.compile(r'set\([^ ]* (${CMAKE_INSTALL_PREFIX}.*)\)')
installdir = re.compile(r'set\([^ ]* (.*)\)')
optionfilter = re.compile(r'indent')

dep = {}
for filename in filenames:
    with open(filename, "r") as f:
        currentname="dummy"
        filetext = f.read()
        start=0

        match = name.search(filetext)
        while match:
            currentname=match.group(1)
            dep[currentname]={}
            start = match.end(1)
            newstart = start
            for key,regex  in zip(['git', 'tag', 'installdir'], [git, tag, installdir]):
                match2 = regex.search(filetext, start)
                if match2 and currentname != 'dummy':
                    if key in dep[currentname]:
                        dep[currentname][key] += " "+match2.group(1)
                    else:
                        dep[currentname][key] = match2.group(1)
                    newstart = max(match2.end(1), newstart)
            key,regex = 'option', option
            match3 = regex.search(filetext, start)
            while match3:
                if match3 and not optionfilter.search(match3.group(1)) and currentname != 'dummy':
                    if key in dep[currentname]:
                        dep[currentname][key] += " "+match3.group(1)
                    else:
                        dep[currentname][key] = match3.group(1)
                newstart = max(match3.end(1), newstart)
                match3 = regex.search(filetext, newstart)
            if  currentname != 'dummy' and not 'installdir' in dep[currentname]:
                dep[currentname]['installdir']="default"
            start = newstart
            match = name.search(filetext, start)


for key in dep:
    print(f" *  `{key}_DIR`")

print("\n\nRadium is compiled and tested with specific version of dependencies, as given in the external's folder CMakeLists.txt and state here for the record\n\n")

for key in dep:
    print(f" *  {key}: {dep[key]['git']}, [{dep[key]['tag']}],\n    *  with options `{dep[key]['option'] if 'option' in dep[key] else None }`")


print("\n\nConfigure your Radium build with:")

for key in dep:
    print(f"{key} {dep[key]['installdir']}")
