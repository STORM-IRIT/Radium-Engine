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

            // Read the file and create a std::istringstream suitable
            // for the lib -- tinyply does not perform any file i/o.
            std::ifstream ss(filename, std::ios::binary);

            // Parse the ASCII header fields
            tinyply::PlyFile file(ss);

            for (auto e : file.get_elements())
            {
                if(e.name.compare("face") == 0 && e.size != 0)
                {
                    // Mesh found. Let the other loaders handle it
                    LOG( logINFO ) << "Faces found. Aborting" << std::endl;
                    return nullptr;
                }
            }

            // we are now sure to have a point-cloud
            Asset::FileData * fileData = new Asset::FileData( filename );

            if ( !fileData->isInitialized() )
            {
                delete fileData;
                return nullptr;
            }


            if ( fileData->isVerbose() )
            {
                LOG( logINFO ) << "File Loading begin...";
            }


            // Define containers to hold the extracted data. The type must match
            // the property type given in the header. Tinyply will interally allocate the
            // the appropriate amount of memory.
            std::vector<float> verts;

            // The count returns the number of instances of the property group. The vectors
            // above will be resized into a multiple of the property group size as
            // they are "flattened"... i.e. verts = {x, y, z, x, y, z, ...}
            uint32_t vertexCount= file.request_properties_from_element("vertex", { "x", "y", "z" }, verts);

            std::clock_t startTime;
            startTime = std::clock();

            file.read(ss);
            fileData->m_geometryData.clear();
            fileData->m_geometryData.reserve(1);

            Asset::GeometryData* geometry = new Asset::GeometryData();
            geometry->setType( Asset::GeometryData::POINT_CLOUD );



            // maps are not vectorized, but who cares, they will be copied
            Eigen::Matrix <float, 3, 1, Eigen::DontAlign>* beginPtr =
                    reinterpret_cast<Eigen::Matrix <float, 3, 1, Eigen::DontAlign> *> (&(verts[0]));
            Eigen::Matrix <float, 3, 1, Eigen::DontAlign>* endPtr   =
                    reinterpret_cast<Eigen::Matrix <float, 3, 1, Eigen::DontAlign> *> (&(verts[verts.size()]));

            uint32_t vertexCount2= std::distance(beginPtr, endPtr);

//            LOG( logINFO ) << "vertexCount: " << vertexCount;
//            LOG( logINFO ) << "verts.size(): " << verts.size();
//            LOG( logINFO ) << "vertexCount2: " << vertexCount2;

            geometry->setVertices(beginPtr, endPtr);

            fileData->m_loadingTime = ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

            fileData->m_geometryData.push_back( std::unique_ptr< Asset::GeometryData >( geometry ) );

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
