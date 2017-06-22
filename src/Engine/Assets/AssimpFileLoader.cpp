#include <Engine/Assets/AssimpFileLoader.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Engine/Assets/FileData.hpp>

#include <Engine/Assets/AssimpHandleDataLoader.hpp>
#include <Engine/Assets/AssimpGeometryDataLoader.hpp>
#include <Engine/Assets/AssimpAnimationDataLoader.hpp>
#include <Engine/Assets/AssimpLightDataLoader.hpp>

namespace Ra
{
    namespace Engine
    {
        AssimpFileLoader::AssimpFileLoader()
        {

        }

        AssimpFileLoader::~AssimpFileLoader()
        {

        }

        virtual const std::vector<std::string>& AssimpFileLoader::getFileExtensions() const
        {

        }

        virtual bool AssimpFileLoader::handleFileExtension( const std::string& extension ) const
        {

        }

        virtual FileData * AssimpFileLoader::loadFile( const std::string& filename ) const
        {
            FileData * fileData = new FileData( filename );

            if ( !fileData->isInitialized() )
            {
                return nullptr;
            }

            Assimp::Importer importer;

            const aiScene * scene = importer.ReadFile( getFileName(),
                                                       aiProcess_Triangulate           | // This could/should be taken away if we want to deal with mesh types other than trimehses
                                                       aiProcess_JoinIdenticalVertices |
                                                       aiProcess_GenSmoothNormals      |
                                                       aiProcess_SortByPType           |
                                                       aiProcess_FixInfacingNormals    |
                                                       aiProcess_CalcTangentSpace      |
                                                       aiProcess_GenUVCoords );

            if ( scene == nullptr )
            {
                LOG( logINFO ) << "File \"" << getFileName() << "\" assimp error : " << importer.GetErrorString() << ".";
                return nullptr;
            }

            if( fileData->isVerbose() )
            {
                LOG(logINFO) << "File Loading begin...";
            }

            if( fileData->isVerbose() )
            {

            }

            return fileData;
        }
    }
}
