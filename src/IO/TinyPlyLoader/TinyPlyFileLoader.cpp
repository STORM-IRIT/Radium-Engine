#include <IO/TinyPlyLoader/TinyPlyFileLoader.hpp>

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
                    LOG( logINFO ) << "[TinyPLY] Faces found. Aborting" << std::endl;
                    return nullptr;
                }
            }

            // we are now sure to have a point-cloud
            Asset::FileData * fileData = new Asset::FileData( filename );

            if ( !fileData->isInitialized() )
            {
                delete fileData;
                LOG( logINFO ) << "[TinyPLY] Filedata cannot be initialized...";
                return nullptr;
            }


            if ( fileData->isVerbose() )
            {
                LOG( logINFO ) << "[TinyPLY] File Loading begin...";
            }


            // Define containers to hold the extracted data. The type must match
            // the property type given in the header. Tinyply will interally allocate the
            // the appropriate amount of memory.
            std::vector<float> verts;

            // The count returns the number of instances of the property group. The vectors
            // above will be resized into a multiple of the property group size as
            // they are "flattened"... i.e. verts = {x, y, z, x, y, z, ...}
            uint32_t vertexCount= file.request_properties_from_element("vertex", { "x", "y", "z" }, verts);

            if(vertexCount == 0) {
                delete fileData;
                LOG( logINFO ) << "[TinyPLY] No vertice found";
                return nullptr;
            }

            fileData->m_geometryData.clear();
            fileData->m_geometryData.reserve(1);

            Asset::GeometryData* geometry = new Asset::GeometryData();
            geometry->setType( Asset::GeometryData::POINT_CLOUD );

            std::vector<float> normals;
            std::vector<uint8_t> colors;
            uint32_t normalCount = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }, normals);
            uint32_t colorCount = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }, colors);

            std::clock_t startTime;
            startTime = std::clock();

            file.read(ss);

            geometry->setVertices(*(reinterpret_cast<std::vector<Eigen::Matrix <float, 3, 1, Eigen::DontAlign>> *> (&verts)));
            geometry->setFrame(Core::Transform::Identity());

            if (normalCount != 0) {
                geometry->setNormals(*(reinterpret_cast<std::vector<Eigen::Matrix <float, 3, 1, Eigen::DontAlign>> *> (&normals)));
            }


            if (colorCount != 0) {
                geometry->setColors(*(reinterpret_cast<std::vector<Eigen::Matrix <uint8_t, 4, 1, Eigen::DontAlign>> *> (&colors)));
                for (auto& c : geometry->getColors()) c /= Scalar(255);
            }


            fileData->m_loadingTime = ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

            fileData->m_geometryData.push_back( std::unique_ptr< Asset::GeometryData >( geometry ) );

            if ( fileData->isVerbose() )
            {
                LOG( logINFO ) << "[TinyPLY] File Loading end.";

                fileData->displayInfo();
            }

            fileData->m_processed = true;

            return fileData;
        }

        std::string TinyPlyFileLoader::name() const
        {
            return "TinyPly";
        }
    }
}
