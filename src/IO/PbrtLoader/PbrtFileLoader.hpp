#ifndef RADIUMENGINE_PBRTFILELOADER_HPP
#define RADIUMENGINE_PBRTFILELOADER_HPP

#include <vector>

#include <IO/RaIO.hpp>
#include <Core/File/FileData.hpp>
#include <Core/File/FileLoaderInterface.hpp>

namespace Ra {
    namespace IO {

        class RA_IO_API PbrtFileLoader : public Asset::FileLoaderInterface
        {
        public:
            PbrtFileLoader();

            virtual ~PbrtFileLoader();

            virtual std::vector<std::string> getFileExtensions() const override;
            virtual bool handleFileExtension( const std::string& extension ) const override;
            virtual Asset::FileData * loadFile( const std::string& filename ) override;
        };

    } // namespace IO
} // namespace Ra

#endif //RADIUMENGINE_PBRTFILELOADER_HPP
