#ifndef RADIUMENGINE_PBRTFILELOADER_HPP
#define RADIUMENGINE_PBRTFILELOADER_HPP

#include <vector>

#include <Engine/Assets/FileData.hpp>
#include <Engine/Assets/FileLoaderInterface.hpp>

namespace Ra
{
    namespace Asset
    {
        class PbrtFileLoader : public FileLoaderInterface
        {
        public:
            PbrtFileLoader();

            virtual ~PbrtFileLoader();

            virtual const std::vector<std::string>& getFileExtensions() const override;
            virtual bool handleFileExtension( const std::string& extension ) const override;
            virtual FileData * loadFile( const std::string& filename ) override;
        };
    }
}

#endif //RADIUMENGINE_PBRTFILELOADER_HPP
