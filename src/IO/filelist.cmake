# ----------------------------------------------------
# This file can be generated from a script:
# To do so, run form ./scripts directory
# ./generateFilelistForModule.sh IO
# ----------------------------------------------------

set(IO_SOURCES CameraLoader/CameraLoader.cpp)

set(IO_HEADERS CameraLoader/CameraLoader.hpp RaIO.hpp)

set(IO_INLINES)

if(RADIUM_IO_DEPRECATED)
    list(APPEND IO_SOURCES deprecated/OBJFileManager.cpp deprecated/OFFFileManager.cpp)

    list(APPEND IO_HEADERS deprecated/FileManager.hpp deprecated/OBJFileManager.hpp
         deprecated/OFFFileManager.hpp
    )

    list(APPEND IO_INLINES deprecated/FileManager.inl)

endif(RADIUM_IO_DEPRECATED)

if(RADIUM_IO_ASSIMP)
    list(
        APPEND
        IO_SOURCES
        AssimpLoader/AssimpAnimationDataLoader.cpp
        AssimpLoader/AssimpCameraDataLoader.cpp
        AssimpLoader/AssimpFileLoader.cpp
        AssimpLoader/AssimpGeometryDataLoader.cpp
        AssimpLoader/AssimpHandleDataLoader.cpp
        AssimpLoader/AssimpLightDataLoader.cpp
    )

    list(
        APPEND
        IO_HEADERS
        AssimpLoader/AssimpAnimationDataLoader.hpp
        AssimpLoader/AssimpCameraDataLoader.hpp
        AssimpLoader/AssimpFileLoader.hpp
        AssimpLoader/AssimpGeometryDataLoader.hpp
        AssimpLoader/AssimpHandleDataLoader.hpp
        AssimpLoader/AssimpLightDataLoader.hpp
        AssimpLoader/AssimpWrapper.hpp
    )

    list(APPEND IO_INLINES AssimpLoader/AssimpGeometryDataLoader.inl)

endif(RADIUM_IO_ASSIMP)

if(RADIUM_IO_TINYPLY)
    list(APPEND IO_SOURCES TinyPlyLoader/TinyPlyFileLoader.cpp)

    list(APPEND IO_HEADERS TinyPlyLoader/TinyPlyFileLoader.hpp)

endif(RADIUM_IO_TINYPLY)

if(RADIUM_IO_VOLUMES)
    list(APPEND IO_SOURCES VolumesLoader/pvmutils.cpp VolumesLoader/VolumeLoader.cpp)

    list(APPEND IO_HEADERS VolumesLoader/pvmutils.hpp VolumesLoader/VolumeLoader.hpp)

endif(RADIUM_IO_VOLUMES)
