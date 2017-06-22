#ifndef RADIUMENGINE_ASSIMPFILELOADER_HPP
#define RADIUMENGINE_ASSIMPFILELOADER_HPP

#include <vector>

#include <assimp/Importer.hpp>

#include <Engine/Assets/FileData.hpp>
#include <Engine/Assets/FileLoaderInterface.hpp>

namespace Ra
{
    namespace Asset
    {
        class AssimpFileLoader : public FileLoaderInterface
        {
        public:
            AssimpFileLoader();

            virtual ~AssimpFileLoader();

            virtual const std::vector<std::string>& getFileExtensions() const override;
            virtual bool handleFileExtension( const std::string& extension ) const override;
            virtual FileData * loadFile( const std::string& filename ) override;

        private:
            Assimp::Importer m_importer;
        };
    }
}

#endif //RADIUMENGINE_ASSIMPFILELOADER_HPP
