#!/usr/bin/python3
from pathlib import Path
import re

p = Path('.')

filenames = list(p.glob('**/*.hpp'))
re_include_inl = re.compile(r'#include [<"](.*\.inl)[>"]')

for filename in filenames:
#["Gui/Widgets/VectorEditor.hpp"]:
    with open(filename, "r+") as f:
        filetext = f.read()
        start=0
        match0 = re_include_inl.search(filetext)
        #search one inl at a time
        if(match0):
            print(match0.group(0))
            include_inl_text = match0.group(1)
            #try full path include
            inl_path= p/include_inl_text
            if(not inl_path.exists()):
                #try relative include
                file_path = Path(filename)
                inl_path= file_path.parent/Path(include_inl_text)
            if(inl_path.exists()):
                with open(inl_path, "r") as inl_f:
                    inl_text = inl_f.read()
                    inl_text = re.sub("#pragma once","",inl_text)
                    print(Path(filename).name)
                    inl_text = re.sub('#include [<"].*'+Path(filename).name+'[>"]',"",inl_text)
                    #lambda trick to not interprete escaped inl_text
                    output = re.sub(match0.group(0), lambda x: inl_text, filetext, 1)
                    f.seek(0)
                    f.write(output)
                    f.truncate()
