# Radium Simplifier application

Documentation for users

Simplifier is a command-line application able to simplifie a mesh.


## Command line manual

### NOM
simplifier

### SYNOPSIS
simplifier -f <filename_in> --pr <pourcentage> | --lod <number_of_lod> | --fa <number_of_faces> [--plugins <plugins_repo>] [--load <filename_plugin>] [--ignore <filename_plugin>] -o <filename_out>

### OPTIONS
-f,--file,--scene
>Open a scene file at startup.  

--plugins,--pluginsPath  
>Set the path to the plugin dlls.  

--load,--loadPlugin  
>Only load plugin with the given name (filename without the extension).  
>If this option is not used, all plugins in the plugins folder will be loaded.  

--ignore,--ignorePlugin  
>Ignore plugins with the given name. If the name appears within both load and ignore options, it will be ignored.  

-l,--lod  
>Set the number of level of details meshs that will be generated. Value 1 will genereted two meshs (100% and 50% faces keep).  

--pr,--percentage  
>Set face percentage keep after simplification  

--fa,--faces  
>Set face number keep after simplification

-o,--out
>Output file name after simplification (default repository is same as the executable one)   
            
            
