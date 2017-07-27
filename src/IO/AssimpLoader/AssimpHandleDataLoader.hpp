#ifndef RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP

#include <map>
#include <set>

#include <IO/RaIO.hpp>
#include <Core/File/DataLoader.hpp>
#include <Core/Math/LinearAlgebra.hpp>

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiBone;

namespace Ra {
    namespace Asset {
        class HandleData;
        struct HandleComponentData;
    }
}

namespace Ra {
    namespace IO {

        class RA_IO_API AssimpHandleDataLoader : public Asset::DataLoader< Asset::HandleData > {
        public:
            /// CONSTRUCTOR
            AssimpHandleDataLoader( const bool VERBOSE_MODE = false );

            /// DESTRUCTOR
            ~AssimpHandleDataLoader();

            /// LOAD
            void loadData( const aiScene* scene, std::vector< std::unique_ptr< Asset::HandleData > >& data ) override;

        protected:
            /// QUERY
            bool sceneHasHandle( const aiScene* scene ) const;
            uint sceneHandleSize( const aiScene* scene ) const;

            /// LOAD
            void loadHandleData( const aiScene* scene, std::vector< std::unique_ptr< Asset::HandleData > >& data ) const;
            void loadHandleComponentData( const aiScene* scene, const aiMesh* mesh, Asset::HandleData* data ) const;
            void loadHandleComponentData( const aiScene* scene, const aiBone* bone, Asset::HandleComponentData& data ) const;
            void loadHandleComponentData( const aiNode* node, Asset::HandleComponentData& data ) const;
            void loadHandleTopologyData( const aiScene* scene, Asset::HandleData* data ) const;
            void loadHandleFrame( const aiNode*                                        node,
                                  const Core::Transform&                               parentFrame,
                                  const std::map< uint, uint >&                        indexTable,
                                  std::vector< std::unique_ptr< Asset::HandleData > >& data ) const;

            /// NAME
            void fetchName( const aiMesh& mesh, Asset::HandleData& data, std::set<std::string>& usedNames ) const;

            /// TYPE
            void fetchType( const aiMesh& mesh, Asset::HandleData& data ) const;

            /// VERTEX SIZE
            void fetchVertexSize( Asset::HandleData& data ) const;
        };

    } // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP
