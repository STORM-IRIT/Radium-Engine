#include <IO/TinyPlyLoader/TinyPlyFileLoader.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <tinyply/tinyply.h>

#include <string>
#include <iostream>
#include <fstream>

const std::string plyExt ("ply");

namespace Ra {
    namespace IO {

        TinyPlyFileLoader::TinyPlyFileLoader()
        {

        }

        TinyPlyFileLoader::~TinyPlyFileLoader()
        {

        }

        std::vector<std::string> TinyPlyFileLoader::getFileExtensions() const
        {
            return std::vector<std::string> ({"*."+plyExt});
        }

        bool TinyPlyFileLoader::handleFileExtension( const std::string& extension ) const
        {
            return extension.compare(plyExt) == 0;
        }

        Asset::FileData * TinyPlyFileLoader::loadFile( const std::string& filename )
        {

            LOG( logINFO ) << "This fileloader is not ready yet. Aborting" << std::endl;
            return nullptr;

            Asset::FileData * fileData = new Asset::FileData( filename );

            if ( !fileData->isInitialized() )
            {
                return nullptr;
            }
            // Read the file and create a std::istringstream suitable
            // for the lib -- tinyply does not perform any file i/o.
            std::ifstream ss(fileData->getFileName(), std::ios::binary);

            // Parse the ASCII header fields
            tinyply::PlyFile file(ss);

//            const aiScene *scene = m_importer.ReadFile( fileData->getFileName(),
//                                                        aiProcess_Triangulate           | // This could/should be taken away if we want to deal with mesh types other than trimehsesaiProcess_JoinIdenticalVertices |
//                                                        aiProcess_GenSmoothNormals      |
//                                                        aiProcess_SortByPType           |
//                                                        aiProcess_FixInfacingNormals    |
//                                                        aiProcess_CalcTangentSpace      |
//                                                        aiProcess_GenUVCoords );

//            if ( scene == nullptr )
//            {
//                LOG( logINFO ) << "File \"" << fileData->getFileName() << "\" assimp error : " << m_importer.GetErrorString() << ".";
//                return nullptr;
//            }

            if ( fileData->isVerbose() )
            {
                LOG( logINFO ) << "File Loading begin...";
            }

            std::clock_t startTime;
            startTime = std::clock();

//            AssimpGeometryDataLoader geometryLoader( Core::StringUtils::getDirName( filename ), fileData->isVerbose() );
//            geometryLoader.loadData( scene, fileData->m_geometryData );

//            AssimpHandleDataLoader handleLoader( fileData->isVerbose() );
//            handleLoader.loadData( scene, fileData->m_handleData );

//            AssimpAnimationDataLoader animationLoader( fileData->isVerbose() );
//            animationLoader.loadData( scene, fileData->m_animationData );

//            AssimpLightDataLoader lightLoader( Core::StringUtils::getDirName( filename ), fileData->isVerbose() );
//            lightLoader.loadData( scene, fileData->m_lightData );

            fileData->m_loadingTime = ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

            if ( fileData->isVerbose() )
            {
                LOG( logINFO ) << "File Loading end.";

                fileData->displayInfo();
            }

            fileData->m_processed = true;

            return fileData;
        }
    }
}
