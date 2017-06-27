#include <Engine/Assets/PbrtFileLoader.hpp>

namespace Ra
{
    namespace Asset
    {
        PbrtFileLoader::PbrtFileLoader()
        {

        }

        PbrtFileLoader::~PbrtFileLoader()
        {

        }

        const std::vector<std::string>& PbrtFileLoader::getFileExtensions() const
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

        FileData * PbrtFileLoader::loadFile( const std::string& filename )
        {
            FileData * fileData = new FileData( filename );

            // TODO(Matthieu) : do some pbrt loading stuff...

            return fileData;
        }
    }
}

