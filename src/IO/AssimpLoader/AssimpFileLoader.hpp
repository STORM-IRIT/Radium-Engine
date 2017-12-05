#ifndef RADIUMENGINE_ASSIMPFILELOADER_HPP
#define RADIUMENGINE_ASSIMPFILELOADER_HPP

#include <assimp/Importer.hpp>

#include <IO/RaIO.hpp>
#include <Core/File/FileData.hpp>
#include <Core/File/FileLoaderInterface.hpp>

namespace Ra {
    namespace IO {

        //! This class loads scenes containing MESHES only (not point-clouds)
        class RA_IO_API AssimpFileLoader : public Asset::FileLoaderInterface
        {
        public:
            AssimpFileLoader();

            virtual ~AssimpFileLoader();

            std::vector<std::string> getFileExtensions() const override;
            bool handleFileExtension( const std::string& extension ) const override;
            Asset::FileData * loadFile( const std::string& filename ) override;
            std::string name() const override;

        private:
            Assimp::Importer m_importer;
        };

    } // namespace IO
} // namespace Ra

#endif //RADIUMENGINE_ASSIMPFILELOADER_HPP
