#include <Engine/Assets/AssimpFileLoader.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Engine/Assets/AssimpHandleDataLoader.hpp>
#include <Engine/Assets/AssimpGeometryDataLoader.hpp>
#include <Engine/Assets/AssimpAnimationDataLoader.hpp>
#include <Engine/Assets/AssimpLightDataLoader.hpp>

namespace Ra
{
    namespace Asset
    {
        AssimpFileLoader::AssimpFileLoader()
        {

        }

        AssimpFileLoader::~AssimpFileLoader()
        {

        }

        const std::vector<std::string>& AssimpFileLoader::getFileExtensions() const
        {
            aiString extensionsList;

            m_importer.GetExtensionList( extensionsList );

            return std::vector<std::string>();
        }

        bool AssimpFileLoader::handleFileExtension( const std::string& extension ) const
        {
            return m_importer.IsExtensionSupported( extension );
        }

        FileData * AssimpFileLoader::loadFile( const std::string& filename )
        {
            FileData * fileData = new FileData( filename );

            if ( !fileData->isInitialized() )
            {
                return nullptr;
            }

            const aiScene * scene = m_importer.ReadFile( fileData->getFileName(),
                                                         aiProcess_Triangulate           | // This could/should be taken away if we want to deal with mesh types other than trimehsesaiProcess_JoinIdenticalVertices |
                                                         aiProcess_GenSmoothNormals      |
                                                         aiProcess_SortByPType           |
                                                         aiProcess_FixInfacingNormals    |
                                                         aiProcess_CalcTangentSpace      |
                                                         aiProcess_GenUVCoords );

            if ( scene == nullptr )
            {
                LOG( logINFO ) << "File \"" << fileData->getFileName() << "\" assimp error : " << m_importer.GetErrorString() << ".";
                return nullptr;
            }

            if( fileData->isVerbose() )
            {
                LOG( logINFO ) << "File Loading begin...";
            }

            std::clock_t startTime;
            startTime = std::clock();

            AssimpGeometryDataLoader geometryLoader( Core::StringUtils::getDirName( filename ), fileData->isVerbose() );
            geometryLoader.loadData( scene, fileData->m_geometryData );

            AssimpHandleDataLoader handleLoader( fileData->isVerbose() );
            handleLoader.loadData( scene, fileData->m_handleData );

            AssimpAnimationDataLoader animationLoader( fileData->isVerbose() );
            animationLoader.loadData( scene, fileData->m_animationData );

            AssimpLightDataLoader lightLoader( Core::StringUtils::getDirName( filename ), fileData->isVerbose() );
            lightLoader.loadData( scene, fileData->m_lightData );

            fileData->m_loadingTime = ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

            if( fileData->isVerbose() )
            {
                LOG( logINFO ) << "File Loading end.";

                fileData->displayInfo();
            }

            fileData->m_processed = true;

            return fileData;
        }
    }
}
