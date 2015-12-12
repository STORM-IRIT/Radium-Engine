#ifndef RADIUMENGINE_HANDLE_DATA_HPP
#define RADIUMENGINE_HANDLE_DATA_HPP

#include <string>
#include <map>
#include <vector>
#include <Core/Log/Log.hpp>
#include <Core/Math/LinearAlgebra.hpp>

#include <Engine/Assets/AssimpHandleDataLoader.hpp>

namespace Ra {
namespace Asset {

struct HandleComponentData {
    HandleComponentData() :
        m_name( "" ),
        m_weight(),
        m_frame( Core::Transform::Identity() ) { }

    std::string                              m_name;
    std::vector< std::pair< uint, Scalar > > m_weight;
    Core::Transform                          m_frame;
};

class HandleData {
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

    /// CONSTRUCTOR
    HandleData( const std::string& name = "",
                const HandleType&  type = UNKNOWN ) :
        m_name( name ),
        m_type( type ),
        m_frame( Core::Transform::Identity() ),
        m_endNode( false ),
        m_nameTable(),
        m_component(),
        m_edge(),
        m_face() { }

    /// NAME
    inline std::string getName() const {
        return m_name;
    }

    /// TYPE
    inline HandleType getType() const {
        return m_type;
    }

    /// FRAME
    inline Core::Transform getFrame() const {
        return m_frame;
    }

    /// DATA
    inline uint getComponentDataSize() const {
        return m_component.size();
    }

    inline std::vector<HandleComponentData> getComponentData() const {
        return m_component;
    }

    inline HandleComponentData getComponent( const uint i ) const {
        return m_component.at( i );
    }

    inline std::vector<Core::Vector2i> getEdgeData() const {
        return m_edge;
    }

    inline std::vector<Core::VectorNi> getFaceData() const {
        return m_face;
    }

    inline void recomputeAllIndices() {
        m_nameTable.clear();
        const uint size = getComponentDataSize();
        for( uint i = 0; i < size; ++i ) {
            m_nameTable[m_component[i].m_name] = i;
        }
    }

    /// QUERY
    inline bool isPointCloud() const {
        return ( m_type == POINT_CLOUD );
    }

    inline bool isSkeleton() const {
        return ( m_type == SKELETON );
    }

    inline bool isCage() const {
        return ( m_type == CAGE );
    }

    inline bool hasComponents() const {
        return !m_component.empty();
    }

    inline bool hasEdges() const {
        return !m_edge.empty();
    }

    inline bool hasFaces() const {
        return !m_face.empty();
    }

    inline bool needsEndNodes() const {
        return m_endNode;
    }

    inline int getIndexOf( const std::string& name ) const {
        auto it = m_nameTable.find( name );
        if( it == m_nameTable.end() ) {
            return -1;
        }
        return it->second;
    }

    /// DEBUG
    inline void displayInfo() const {
        std::string type;
        switch( m_type ) {
            case UNKNOWN     : type = "UNKNOWN";     break;
            case POINT_CLOUD : type = "POINT CLOUD"; break;
            case SKELETON    : type = "SKELETON";    break;
            case CAGE        : type = "CAGE";        break;
        }
        LOG( logDEBUG ) << "======== HANDLE INFO ========";
        LOG( logDEBUG ) << " Name            : " << m_name;
        LOG( logDEBUG ) << " Type            : " << type;
        LOG( logDEBUG ) << " Element #       : " << m_component.size();
        LOG( logDEBUG ) << " Edge #          : " << m_edge.size();
        LOG( logDEBUG ) << " Face #          : " << m_face.size();
        LOG( logDEBUG ) << " Need EndNodes ? : " << ( ( m_endNode ) ? "YES" : "NO" );
    }

protected:
    /// NAME
    inline void setName( const std::string& name ) {
        m_name = name;
    }

    /// TYPE
    inline void setType( const HandleType& type ) {
        m_type = type;
    }

    /// FRAME
    inline void setFrame( const Core::Transform& frame ) {
        m_frame = frame;
    }

    /// TABLE
    inline void setNameTable( const std::map< std::string, uint >& nameTable ) {
        m_nameTable = nameTable;
    }

    /// COMPONENT
    inline void setComponents( const std::vector< HandleComponentData >& components ) {
        const uint size = components.size();
        m_component.resize( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            m_component[i] = components[i];
        }
    }

    /// EDGE
    inline void setEdges( const std::vector< Core::Vector2i >& edgeList ) {
        const uint size = edgeList.size();
        m_edge.resize( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            m_edge[i] = edgeList[i];
        }
    }

    /// FACE
    inline void setFaces( const std::vector< Core::VectorNi >& faceList ) {
        const uint size = faceList.size();
        m_face.resize( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            m_face[i] = faceList[i];
        }
    }

protected:
    /// VARIABLE
    std::string                        m_name;
    HandleType                         m_type;
    Core::Transform                    m_frame;
    bool                               m_endNode;
    std::map< std::string, uint >      m_nameTable;
    std::vector< HandleComponentData > m_component;
    std::vector< Core::Vector2i >      m_edge;
    std::vector< Core::VectorNi >      m_face;
};

} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_HANDLE_DATA_HPP
