#ifndef RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP

#include <map>
#include <assimp/mesh.h>
#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Assets/DataLoader.hpp>
#include <Engine/Assets/FileData.hpp>

// FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_HANDLE

namespace Ra {
namespace Asset {

class HandleData;
struct HandleComponentData;
class AssimpHandleDataLoader : public DataLoader< HandleData > {
public:
    /// CONSTRUCTOR
    AssimpHandleDataLoader( const bool VERBOSE_MODE = false );

    /// DESTRUCTOR
    ~AssimpHandleDataLoader();

    /// LOAD
    void loadData( const aiScene* scene, std::vector< std::unique_ptr< HandleData > >& data ) const override;

protected:
    /// QUERY
    bool sceneHasHandle( const aiScene* scene ) const;
    uint sceneHandleSize( const aiScene* scene ) const;

    /// LOAD
    void loadHandleData( const aiScene* scene, std::vector< std::unique_ptr< HandleData > >& data ) const;
    void loadHandleComponentData( const aiScene* scene, const aiMesh* mesh, HandleData* data ) const;
    void loadHandleComponentData( const aiScene* scene, const aiBone* bone, HandleComponentData& data ) const;
    void loadHandleComponentData( const aiNode* node, HandleComponentData& data ) const;
    void loadHandleTopologyData( const aiScene* scene, HandleData* data ) const;
    void loadHandleFrame( const aiNode*                                 node,
                          const Core::Transform&                        parentFrame,
                          const std::map< uint, uint >&                 indexTable,
                          std::vector< std::unique_ptr< HandleData > >& data ) const;

    /// NAME
    void fetchName( const aiMesh& mesh, HandleData& data ) const;

    /// TYPE
    void fetchType( const aiMesh& mesh, HandleData& data ) const;

    /// VERTEX SIZE
    void fetchVertexSize( HandleData& data ) const;
};

} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP
#endif
