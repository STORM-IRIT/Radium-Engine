#ifndef RADIUMENGINE_ASSIMPFILELOADER_HPP
#define RADIUMENGINE_ASSIMPFILELOADER_HPP

#include <Engine/Assets/FileLoaderInterface.hpp>

namespace Ra
{
    namespace Engine
    {
        class AssimpFileLoader : public FileLoaderInterface
        {
        public:
            virtual const std::vector<std::string>& getFileExtensions() const override;
            virtual bool handleFileExtension( const std::string& extension ) const override;
            virtual FileData * loadFile( const std::string& filename ) const override;
        };
    }
}

#endif //RADIUMENGINE_ASSIMPFILELOADER_HPP
