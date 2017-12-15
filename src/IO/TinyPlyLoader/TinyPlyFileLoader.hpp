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

            std::vector<std::string> getFileExtensions() const override;
            bool handleFileExtension( const std::string& extension ) const override;
            Asset::FileData * loadFile( const std::string& filename ) override;
            std::string name() const override;
        };

    } // namespace IO
} // namespace Ra

#endif //RADIUMENGINE_ASSIMPFILELOADER_HPP
