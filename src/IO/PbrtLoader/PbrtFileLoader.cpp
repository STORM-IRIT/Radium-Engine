#include <IO/PbrtLoader/PbrtFileLoader.hpp>

namespace Ra {
    namespace IO {

        PbrtFileLoader::PbrtFileLoader()
        {

        }

        PbrtFileLoader::~PbrtFileLoader()
        {

        }

        std::vector<std::string> PbrtFileLoader::getFileExtensions() const
        {
            std::string extensionsList;

            // TODO(Matthieu) : get a list of extensions
            // m_importer.GetExtensionList( extensionsList );

            std::vector<std::string> extensions = Core::StringUtils::splitString( extensionsList, ';' );

            return extensions;
        }

        bool PbrtFileLoader::handleFileExtension( const std::string& extension ) const
        {
            // return m_importer.IsExtensionSupported( extension );

            // TODO(Matthieu) : search within extensions vector
            return false;
        }

        Asset::FileData * PbrtFileLoader::loadFile( const std::string& filename )
        {
            Asset::FileData* fileData = new Asset::FileData( filename );

            // TODO(Matthieu) : do some pbrt loading stuff...

            return fileData;
        }

    } // namespace IO
} // namespace Ra

