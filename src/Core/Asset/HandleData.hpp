#ifndef RADIUMENGINE_HANDLE_DATA_HPP
#define RADIUMENGINE_HANDLE_DATA_HPP

#include <map>
#include <string>
#include <vector>

#include <Core/Container/AlignedStdVector.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/RaCore.hpp>

#include <Core/Asset/AssetData.hpp>

namespace Ra {
namespace Core {
namespace Asset {

struct RA_CORE_API HandleComponentData {
    RA_CORE_ALIGNED_NEW

    HandleComponentData();

    Core::Math::Transform m_frame;
    std::string m_name;
    std::vector<std::pair<uint, Scalar>> m_weight;
};

class RA_CORE_API HandleData : public AssetData {
  public:
    RA_CORE_ALIGNED_NEW

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
    inline Core::Math::Transform getFrame() const;
    inline void setFrame( const Core::Math::Transform& frame );

    /// VERTEX SIZE
    inline uint getVertexSize() const;
    inline void setVertexSize( uint size );

    /// NAME TABLE
    inline void setNameTable( const std::map<std::string, uint>& nameTable );

    /// DATA
    inline uint getComponentDataSize() const;
    inline const Core::Container::AlignedStdVector<HandleComponentData>& getComponentData() const;
    inline Core::Container::AlignedStdVector<HandleComponentData>& getComponentData();
    inline const HandleComponentData& getComponent( const uint i ) const;
    inline HandleComponentData& getComponent( const uint i );
    inline void setComponents( const Core::Container::AlignedStdVector<HandleComponentData>& components );
    inline const Core::Container::AlignedStdVector<Core::Math::Vector2i>& getEdgeData() const;
    inline Core::Container::AlignedStdVector<Core::Math::Vector2i>& getEdgeData();
    inline void setEdges( const Core::Container::AlignedStdVector<Core::Math::Vector2i>& edgeList );
    inline const Core::Container::AlignedStdVector<Core::Math::VectorNi>& getFaceData() const;
    inline Core::Container::AlignedStdVector<Core::Math::VectorNi>& getFaceData();
    inline void setFaces( const Core::Container::AlignedStdVector<Core::Math::VectorNi>& faceList );
    inline void recomputeAllIndices();

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
    Core::Math::Transform m_frame;
    HandleType m_type;

    bool m_endNode;
    uint m_vertexSize;
    std::map<std::string, uint> m_nameTable;

    Core::Container::AlignedStdVector<HandleComponentData> m_component;
    Core::Container::AlignedStdVector<Core::Math::Vector2i> m_edge;
    Core::Container::AlignedStdVector<Core::Math::VectorNi> m_face;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/HandleData.inl>

#endif // RADIUMENGINE_HANDLE_DATA_HPP
