#ifndef RADIUMENGINE_HANDLE_DATA_HPP
#define RADIUMENGINE_HANDLE_DATA_HPP

#include <string>
#include <map>
#include <vector>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/AlignedStdVector.hpp>

#include <Engine/Assets/AssetData.hpp>
#include <Engine/Assets/AssimpHandleDataLoader.hpp>

namespace Ra {
namespace Asset {

struct HandleComponentData {
    HandleComponentData();

    Core::Transform                          m_frame;
    std::string                              m_name;
    std::vector< std::pair< uint, Scalar > > m_weight;
};

class HandleData : public AssetData {
public:
    /// FRIEND
    friend class AssimpHandleDataLoader;

    /// ENUM
    enum HandleType {
        UNKNOWN     = 1 << 0,
        POINT_CLOUD = 1 << 1,
        SKELETON    = 1 << 2,
        CAGE        = 1 << 3
    };

    RA_CORE_ALIGNED_NEW

    /// CONSTRUCTOR
    HandleData( const std::string& name = "",
                const HandleType&  type = UNKNOWN );

    HandleData( const HandleData& data ) = default;

    /// DESTRUCTOR
    ~HandleData();

    /// NAME
    //inline std::string getName() const;

    /// TYPE
    inline HandleType getType() const;

    /// FRAME
    inline Core::Transform getFrame() const;

    /// VERTEX SIZE
    inline uint getVertexSize() const;

    /// DATA
    inline uint getComponentDataSize() const;
    inline Core::AlignedStdVector<HandleComponentData> getComponentData() const;
    inline HandleComponentData getComponent( const uint i ) const;
    inline Core::AlignedStdVector<Core::Vector2i> getEdgeData() const;
    inline Core::AlignedStdVector<Core::VectorNi> getFaceData() const;
    inline void recomputeAllIndices();

    /// QUERY
    inline bool isPointCloud() const;
    inline bool isSkeleton() const;
    inline bool isCage() const;
    inline bool hasComponents() const;
    inline bool hasEdges() const;
    inline bool hasFaces() const;
    inline bool needsEndNodes() const;
    inline int  getIndexOf( const std::string& name ) const;

    /// DEBUG
    inline void displayInfo() const;

protected:
    /// NAME
    inline void setName( const std::string& name );

    /// TYPE
    inline void setType( const HandleType& type );

    /// FRAME
    inline void setFrame( const Core::Transform& frame );

    /// TABLE
    inline void setNameTable( const std::map< std::string, uint >& nameTable );

    /// COMPONENT
    inline void setComponents( const Core::AlignedStdVector< HandleComponentData >& components );

    /// EDGE
    inline void setEdges( const Core::AlignedStdVector< Core::Vector2i >& edgeList );

    /// FACE
    inline void setFaces( const Core::AlignedStdVector< Core::VectorNi >& faceList );

protected:
    /// VARIABLE
    Core::Transform                    m_frame;
    HandleType                         m_type;
    //std::string                        m_name;

    bool                               m_endNode;
    uint                               m_vertexSize;
    std::map< std::string, uint >      m_nameTable;

    Core::AlignedStdVector< HandleComponentData > m_component;
    Core::AlignedStdVector< Core::Vector2i >      m_edge;
    Core::AlignedStdVector< Core::VectorNi >      m_face;
};

} // namespace Asset
} // namespace Ra

#include <Engine/Assets/HandleData.inl>

#endif // RADIUMENGINE_HANDLE_DATA_HPP
