#!/usr/bin/python3

from pathlib import Path
import re

p = Path('.')

filenames=list(p.glob('**/CMakeLists.txt'))

name = re.compile(r'Add\([ ]*(.*)')
git = re.compile(r'GIT_REPOSITORY (.*)')
tag = re.compile(r'GIT_TAG (.*)')
option = re.compile(r'(-D.*)')
optionfilter = re.compile(r'indent')

dep = {}
for filename in filenames:
    with open(filename, "r") as f:
        currentname="dummy"
        for line in f:
            match = name.search(line)
            if match:
                currentname=match.group(1)
                dep[currentname]={}
            for key,regex  in zip(['git', 'tag', 'option'], [git, tag, option]):
                match = regex.search(line)
                if match and not optionfilter.search(line):
                    if(key in dep[currentname]):
                        dep[currentname][key] += " "+match.group(1)
                    else:
                        dep[currentname][key] = match.group(1)


for key in dep:
    print(f" *  `{key}_DIR`")

print("\n\nRadium in compiled and tested with specific version of dependencies, as given in the external's folder CMakeLists.txt and state here for the record\n\n")

for key in dep:
    print(f" *  {key}: {dep[key]['git']}, [{dep[key]['tag']}],\n    *  with options `{dep[key]['option'] if 'option' in dep[key] else None }`")
