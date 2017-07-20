#ifndef RADIUMENGINE_ASSIMPFILELOADER_HPP
#define RADIUMENGINE_ASSIMPFILELOADER_HPP

#include <assimp/Importer.hpp>

#include <IO/RaIO.hpp>
#include <Core/File/FileData.hpp>
#include <Core/File/FileLoaderInterface.hpp>

namespace Ra {
    namespace IO {

        class RA_IO_API AssimpFileLoader : public Asset::FileLoaderInterface
        {
        public:
            AssimpFileLoader();

            virtual ~AssimpFileLoader();

            virtual std::vector<std::string> getFileExtensions() const override;
            virtual bool handleFileExtension( const std::string& extension ) const override;
            virtual Asset::FileData * loadFile( const std::string& filename ) override;

        private:
            Assimp::Importer m_importer;
        };

    } // namespace IO
} // namespace Ra

#endif //RADIUMENGINE_ASSIMPFILELOADER_HPP
