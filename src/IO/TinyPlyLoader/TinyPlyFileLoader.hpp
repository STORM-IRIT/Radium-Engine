#ifndef RADIUMENGINE_TINYPLYFILELOADER_HPP
#define RADIUMENGINE_TINYPLYFILELOADER_HPP

#include <IO/RaIO.hpp>
#include <Core/File/FileData.hpp>
#include <Core/File/FileLoaderInterface.hpp>

namespace Ra {
    namespace IO {

        class RA_IO_API TinyPlyFileLoader : public Asset::FileLoaderInterface
        {
        public:
            TinyPlyFileLoader();

            virtual ~TinyPlyFileLoader();

            virtual std::vector<std::string> getFileExtensions() const override;
            virtual bool handleFileExtension( const std::string& extension ) const override;
            virtual Asset::FileData * loadFile( const std::string& filename ) override;
        };

    } // namespace IO
} // namespace Ra

#endif //RADIUMENGINE_ASSIMPFILELOADER_HPP
