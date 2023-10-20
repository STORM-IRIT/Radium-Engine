# ----------------------------------------------------
# This file can be generated from a script:
# To do so, run "./generateFilelistForModule.sh IO"
# from ./scripts directory
# ----------------------------------------------------

set(io_sources CameraLoader/CameraLoader.cpp)

set(io_headers CameraLoader/CameraLoader.hpp RaIO.hpp)

if(RADIUM_IO_DEPRECATED)
    list(APPEND io_sources deprecated/OBJFileManager.cpp deprecated/OFFFileManager.cpp)

    list(APPEND io_headers deprecated/FileManager.hpp deprecated/OBJFileManager.hpp
         deprecated/OFFFileManager.hpp
    )

endif(RADIUM_IO_DEPRECATED)

if(RADIUM_IO_ASSIMP)
    list(
        APPEND
        io_sources
        AssimpLoader/AssimpAnimationDataLoader.cpp
        AssimpLoader/AssimpCameraDataLoader.cpp
        AssimpLoader/AssimpFileLoader.cpp
        AssimpLoader/AssimpGeometryDataLoader.cpp
        AssimpLoader/AssimpHandleDataLoader.cpp
        AssimpLoader/AssimpLightDataLoader.cpp
    )

    list(
        APPEND
        io_headers
        AssimpLoader/AssimpAnimationDataLoader.hpp
        AssimpLoader/AssimpCameraDataLoader.hpp
        AssimpLoader/AssimpFileLoader.hpp
        AssimpLoader/AssimpGeometryDataLoader.hpp
        AssimpLoader/AssimpHandleDataLoader.hpp
        AssimpLoader/AssimpLightDataLoader.hpp
        AssimpLoader/AssimpWrapper.hpp
    )

endif(RADIUM_IO_ASSIMP)

if(RADIUM_IO_TINYPLY)
    list(APPEND io_sources TinyPlyLoader/TinyPlyFileLoader.cpp)

    list(APPEND io_headers TinyPlyLoader/TinyPlyFileLoader.hpp)

endif(RADIUM_IO_TINYPLY)

if(RADIUM_IO_VOLUMES)
    list(APPEND io_sources VolumesLoader/VolumeLoader.cpp VolumesLoader/pvmutils.cpp)

    list(APPEND io_headers VolumesLoader/VolumeLoader.hpp VolumesLoader/pvmutils.hpp)

endif(RADIUM_IO_VOLUMES)
if(RADIUM_IO_GLTF)
    list(APPEND io_sources Gltf/Loader/glTFFileLoader.cpp)

    list(
        APPEND
        io_sources
        Gltf/internal/GLTFConverter/AccessorReader.cpp
        Gltf/internal/GLTFConverter/Converter.cpp
        Gltf/internal/GLTFConverter/HandleData.cpp
        Gltf/internal/GLTFConverter/MaterialConverter.cpp
        Gltf/internal/GLTFConverter/MeshData.cpp
        Gltf/internal/GLTFConverter/NormalCalculator.cpp
        Gltf/internal/GLTFConverter/SceneNode.cpp
        Gltf/internal/GLTFConverter/TangentCalculator.cpp
        Gltf/internal/GLTFConverter/TransformationManager.cpp
    )

    list(APPEND io_sources Gltf/internal/GLTFConverter/mikktspace.c)

    list(APPEND io_headers Gltf/Loader/glTFFileLoader.hpp)

    list(
        APPEND
        io_private_headers
        Gltf/internal/GLTFConverter/AccessorReader.hpp
        Gltf/internal/GLTFConverter/Converter.hpp
        Gltf/internal/GLTFConverter/HandleData.hpp
        Gltf/internal/GLTFConverter/ImageData.hpp
        Gltf/internal/GLTFConverter/MaterialConverter.hpp
        Gltf/internal/GLTFConverter/MeshData.hpp
        Gltf/internal/GLTFConverter/NormalCalculator.hpp
        Gltf/internal/GLTFConverter/SceneNode.hpp
        Gltf/internal/GLTFConverter/TangentCalculator.hpp
        Gltf/internal/GLTFConverter/TransformationManager.hpp
    )

    list(APPEND io_private_headers Gltf/internal/GLTFConverter/mikktspace.h)

    list(APPEND io_private_headers Gltf/internal/Extensions/LightExtensions.hpp
         Gltf/internal/Extensions/MaterialExtensions.hpp
    )

    list(APPEND io_private_headers Gltf/internal/fx/gltf.h)

    if(RADIUM_IO_GLTF_WRITER)
        list(APPEND io_sources Gltf/Writer/glTFFileWriter.cpp)

        list(APPEND io_headers Gltf/Writer/glTFFileWriter.hpp)

    endif(RADIUM_IO_GLTF_WRITER)
endif(RADIUM_IO_GLTF)
