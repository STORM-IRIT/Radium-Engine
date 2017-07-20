#include <IO/PbrtLoader/PbrtFileLoader.hpp>

namespace Ra
{
    namespace IO
    {
        PbrtFileLoader::PbrtFileLoader()
        {
            //parseFile( "Shaders/scene.pbrt" );
        }

        PbrtFileLoader::~PbrtFileLoader()
        {

        }

        std::vector<std::string> PbrtFileLoader::getFileExtensions() const
        {
            return { "*.pbrt" };
        }

        bool PbrtFileLoader::handleFileExtension( const std::string& extension ) const
        {
            return ( extension == "pbrt" );
        }

        Asset::FileData * PbrtFileLoader::loadFile( const std::string& filename )
        {
            Asset::FileData* fileData = new Asset::FileData( filename );

            if ( !fileData->isInitialized() )
            {
                return nullptr;
            }

            // Load scene

            if ( fileData->isVerbose() )
            {
                LOG( logINFO ) << "File Loading begin...";
            }

            std::clock_t startTime;
            startTime = std::clock();

            // Load data

            fileData->m_loadingTime = ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

            if ( fileData->isVerbose() )
            {
                LOG( logINFO ) << "File Loading end.";

                fileData->displayInfo();
            }

            fileData->m_processed = true;

            return fileData;
        }
    } // namespace IO
} // namespace Ra

