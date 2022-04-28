#!/usr/bin/python3
# run from Radium-Engine/external directory
# This files generates information about Radium externals, and is used to generate the Documentation,

from pathlib import Path
import re

p = Path('.')

filenames=list(p.glob('**/CMakeLists.txt'))

full = re.compile(r'(Add\([^\)]*\))')
name = re.compile(r'Add\([\s]*(\S*)')
git = re.compile(r'[^\)]*GIT_REPOSITORY (.*)')
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
        match0 = full.search(filetext)
        while(match0):
            match1 = name.search(match0.group(0))
            currentname=match1.group(1)
            dep[currentname]={}
            for key,regex  in zip(['git', 'tag', 'installdir'], [git, tag, installdir]):
                match2 = regex.search(filetext, start)
                if match2 and currentname != 'dummy':
                    if key in dep[currentname]:
                        dep[currentname][key] += " "+match2.group(1)
                    else:
                        dep[currentname][key] = match2.group(1)
            key,regex = 'option', option
            match3 = regex.search(filetext, start)
            while match3:
                if match3 and not optionfilter.search(match3.group(1)) and currentname != 'dummy':
                    if key in dep[currentname]:
                        dep[currentname][key] += " "+match3.group(1)
                    else:
                        dep[currentname][key] = match3.group(1)
                match3 = regex.search(filetext, match3.end(1))
            if  currentname != 'dummy' and not 'installdir' in dep[currentname]:
                dep[currentname]['installdir']="default"
            start = match0.end(0)
            match0 = full.search(filetext, start)

for key in dep:
    print(f" *  `{key}_DIR`")

print("\n\nRadium is compiled and tested with specific version of dependencies, as given in the external's folder CMakeLists.txt and state here for the record\n\n")

for key in dep:
    print(f" *  {key}: {dep[key]['git']}, [{dep[key]['tag']}],\n    *  with options `{dep[key]['option'] if 'option' in dep[key] else None }`")
