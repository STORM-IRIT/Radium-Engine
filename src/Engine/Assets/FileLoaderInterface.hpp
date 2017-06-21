#ifndef RADIUMENGINE_FILELOADERINTERFACE_HPP
#define RADIUMENGINE_FILELOADERINTERFACE_HPP

#include <vector>

namespace Ra
{
    namespace Engine
    {
        class FileData;
    }
}

namespace Ra
{
    namespace Engine
    {
        class FileLoaderInterface
        {
            virtual const std::vector<std::string>& getFileExtensions() const = 0;
            virtual bool handleFileExtension( const std::string& extension ) const = 0;
            virtual FileData loadFile( const std::string& filename ) const = 0;
        };
    }
}

#endif //RADIUMENGINE_FILELOADER_H
