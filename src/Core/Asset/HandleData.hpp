#ifndef RADIUMENGINE_HANDLE_DATA_HPP
#define RADIUMENGINE_HANDLE_DATA_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/RaCore.hpp>
#include <Core/Types.hpp>

#include <Core/Asset/AssetData.hpp>

namespace Ra {
namespace Core {
namespace Asset {

struct RA_CORE_API HandleComponentData {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    HandleComponentData();

    /// Handle name
    std::string m_name;

    /// Handle transformation in model space
    Core::Transform m_frame;

    /// Matrix from mesh space to local space
    Core::Transform m_offset{Core::Transform::Identity()};

    /// Per skinned-mesh vertex weigths.
    std::map<std::string, std::vector<std::pair<uint, Scalar>>> m_weight;
};

class RA_CORE_API HandleData : public AssetData {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /// ENUM
    enum HandleType { UNKNOWN = 1 << 0, POINT_CLOUD = 1 << 1, SKELETON = 1 << 2, CAGE = 1 << 3 };

    /// CONSTRUCTOR
    HandleData( const std::string& name = "", const HandleType& type = UNKNOWN );

    HandleData( const HandleData& data ) = default;

    /// DESTRUCTOR
    ~HandleData();

    /// NAME
    inline void setName( const std::string& name );

    /// TYPE
    inline HandleType getType() const;
    inline void setType( const HandleType& type );

    /// FRAME
    inline Core::Transform getFrame() const;
    inline void setFrame( const Core::Transform& frame );

    /// VERTEX SIZE
    inline uint getVertexSize() const;
    inline void setVertexSize( uint size );

    /// NAME TABLE
    inline void setNameTable( const std::map<std::string, uint>& nameTable );

    /// DATA
    inline uint getComponentDataSize() const;
    inline const Core::AlignedStdVector<HandleComponentData>& getComponentData() const;
    inline Core::AlignedStdVector<HandleComponentData>& getComponentData();
    inline const HandleComponentData& getComponent( const uint i ) const;
    inline HandleComponentData& getComponent( const uint i );
    inline void setComponents( const Core::AlignedStdVector<HandleComponentData>& components );
    inline const Core::AlignedStdVector<Core::Vector2i>& getEdgeData() const;
    inline Core::AlignedStdVector<Core::Vector2i>& getEdgeData();
    inline void setEdges( const Core::AlignedStdVector<Core::Vector2i>& edgeList );
    inline const Core::AlignedStdVector<Core::VectorNi>& getFaceData() const;
    inline Core::AlignedStdVector<Core::VectorNi>& getFaceData();
    inline void setFaces( const Core::AlignedStdVector<Core::VectorNi>& faceList );
    inline void recomputeAllIndices();

    /// SKINNED MESHES
    inline void addBindMesh( const std::string& name );
    inline const std::set<std::string>& getBindMeshes() const;

    /// QUERY
    inline bool isPointCloud() const;
    inline bool isSkeleton() const;
    inline bool isCage() const;
    inline bool hasComponents() const;
    inline bool hasEdges() const;
    inline bool hasFaces() const;
    inline bool needsEndNodes() const;
    inline int getIndexOf( const std::string& name ) const;

    inline void needEndNodes( bool need );

    /// DEBUG
    inline void displayInfo() const;

  protected:
    /// VARIABLE
    Core::Transform m_frame;
    HandleType m_type;

    bool m_endNode;
    uint m_vertexSize;
    std::map<std::string, uint> m_nameTable;
    std::set<std::string> m_bindMeshes;

    Core::AlignedStdVector<HandleComponentData> m_component;
    Core::AlignedStdVector<Core::Vector2i> m_edge;
    Core::AlignedStdVector<Core::VectorNi> m_face;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/HandleData.inl>

#endif // RADIUMENGINE_HANDLE_DATA_HPP
