#pragma once

#include <Core/Asset/AssetData.hpp>
#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/RaCore.hpp>
#include <Core/Types.hpp>

#include <map>
#include <set>
#include <string>
#include <vector>

namespace Ra {
namespace Core {
namespace Asset {

/**
 * A HandleComponentData stores the data of an animation Handle linked to an object.
 */
struct RA_CORE_API HandleComponentData {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /// Handle name.
    std::string m_name;

    /// Handle transformation in model space.
    Core::Transform m_frame { Core::Transform::Identity() };

    /// Per skinned-mesh matrix from mesh space to bone space (local).
    std::map<std::string, Core::Transform> m_bindMatrices;

    /// Per skinned-mesh vertex weights.
    std::map<std::string, std::vector<std::pair<uint, Scalar>>> m_weights;
};

/**
 * The HandleData class stores all the HandleComponentData linked to an object.
 */
class RA_CORE_API HandleData : public AssetData
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /**
     * The type of Handle system.
     */
    enum HandleType { UNKNOWN = 1 << 0, POINT_CLOUD = 1 << 1, SKELETON = 1 << 2, CAGE = 1 << 3 };

    explicit HandleData( const std::string& name = "", const HandleType& type = UNKNOWN ) :
        AssetData( name ), m_type( type ) {}

    /// \name HandleArray
    /// \{

    /**
     * Return the type of the Handle system.
     */
    inline HandleType getType() const { return m_type; }

    /**
     * Set the type of the Handle system.
     */
    inline void setType( const HandleType& type ) { m_type = type; }

    /**
     * Return the transformation of the Handle system.
     */
    inline Core::Transform getFrame() const { return m_frame; }

    /**
     * Set the transformation of the Handle system.
     */
    inline void setFrame( const Core::Transform& frame ) { m_frame = frame; }

    /**
     * Add \p name to the list of bound mesh names.
     */
    inline void addBindMesh( const std::string& name ) { m_bindMeshes.insert( name ); }

    /**
     * Returns the list of bound mesh names.
     */
    inline const std::set<std::string>& getBindMeshes() const { return m_bindMeshes; }
    /// \}

    /**
     * Return the maximal number of vertices influenced by a Handle.
     */
    inline uint getVertexSize() const { return m_vertexSize; }

    /**
     * Set the maximal number of vertices influenced by a Handle.
     */
    inline void setVertexSize( uint size ) { m_vertexSize = size; }

    /**
     * Set the map from Handle names to Handle storage index.
     */
    inline void setNameTable( const std::map<std::string, uint>& nameTable ) {
        m_nameTable = nameTable;
    }

    /**
     * Recompute the map from Handle names to Handle storage index.
     */
    inline void recomputeAllIndices();

    /// \name Data access
    /// \{

    /**
     * Return the number of Handles in the system.
     */
    inline uint getComponentDataSize() const { return m_component.size(); }

    /**
     * Return the list of HandleComponentData.
     */
    inline const auto& getComponentData() const { return m_component; }

    /**
     * Return the list of HandleComponentData.
     */
    inline auto& getComponentData() { return m_component; }

    /**
     * Return the \p i-th HandleComponentData.
     */
    inline const HandleComponentData& getComponent( const uint i ) const;

    /**
     * Return the \p i-th HandleComponentData.
     */
    inline HandleComponentData& getComponent( const uint i );

    /**
     * Set the HandleComponentData for the Handle system.
     */
    inline void setComponents( Core::AlignedStdVector<HandleComponentData>&& components ) {
        m_component = std::move( components );
    }

    /**
     * Return the HandleArray hierarchy, i.e.\ bones hierarchy.
     */
    inline const auto& getEdgeData() const { return m_edge; }

    /**
     * Return the HandleArray hierarchy, i.e.\ bones hierarchy.
     */
    inline auto& getEdgeData() { return m_edge; }

    /**
     * Set the HandleArray linear hierarchy part, i.e.\ bones hierarchy.
     */
    inline void setEdges( Core::AlignedStdVector<Core::Vector2ui>&& edgeList ) {
        m_edge = std::move( edgeList );
    }

    /**
     * Return the HandleArray N-Dimensional parts, i.e.\ cage polyhedra.
     */
    inline const auto& getFaceData() const { return m_face; }

    /**
     * Return the HandleArray N-Dimensional parts, i.e.\ cage polyhedra.
     */
    inline auto& getFaceData() { return m_face; }

    /**
     * Set the HandleArray N-Dimensional parts, i.e.\ cage polyhedra.
     */
    // inline void setFaces( const Core::AlignedStdVector<Core::VectorNui>& faceList );
    inline void setFaces( Core::AlignedStdVector<Core::VectorNui>&& faceList ) {
        m_face = std::move( faceList );
    }
    /**
     * Set whether the Handle system needs end bones.
     */
    inline void needEndNodes( bool need ) { m_endNode = need; }
    /// \}

    /// \name Status querries
    /// \{

    /**
     * Return true if the Handle system is a Point Cloud.
     */
    inline bool isPointCloud() const { return m_type == POINT_CLOUD; }

    /**
     * Return true if the Handle system is a Skeleton.
     */
    inline bool isSkeleton() const { return m_type == SKELETON; }

    /**
     * Return true if the Handle system is a Cage.
     */
    inline bool isCage() const { return m_type == CAGE; }

    /**
     * Return true if the Handle system has Handles.
     */
    inline bool hasComponents() const { return !m_component.empty(); }

    /**
     * Return true if the Handle system has a hierarchy.
     */
    inline bool hasEdges() const { return !m_edge.empty(); }

    /**
     * Return true if the Handle system has N-Dimensional parts.
     */
    inline bool hasFaces() const { return !m_face.empty(); }

    /**
     * Return true if the Handle system needs end bones.
     */
    inline bool needsEndNodes() const { return m_endNode; }

    /**
     * Return the storage index of the Handle with the given name
     * if it exists, -1 otherwise.
     */
    inline int getIndexOf( const std::string& name ) const;
    /// \}

    /**
     * Print stat info to the Debug output.
     */
    void displayInfo() const;

  private:
    /// The type of the Handle system.
    HandleType m_type;

    /// The transformation of the Handle system.
    Core::Transform m_frame { Core::Transform::Identity() };

    /// Whether the Handle System needs end bones.
    bool m_endNode { false };

    /// The maximal number of vertices influenced by a Handle of the system.
    uint m_vertexSize { 0 };

    /// The map from Handle name to storage index.
    std::map<std::string, uint> m_nameTable;

    /// The list of bound mesh names.
    std::set<std::string> m_bindMeshes;

    /// The list of HandleComponentData.
    Core::AlignedStdVector<HandleComponentData> m_component;

    /// The HandleArray hierarchy, i.e.\ bones hierarchy.
    Core::AlignedStdVector<Core::Vector2ui> m_edge;

    /// The HandleArray N-Dimensional parts, i.e.\ cage polyhedra.
    Core::AlignedStdVector<Core::VectorNui> m_face;
};

inline const HandleComponentData& HandleData::getComponent( const uint i ) const {
    CORE_ASSERT( ( i < m_component.size() ), "Index i out of bound" );
    return m_component[i];
}

inline HandleComponentData& HandleData::getComponent( const uint i ) {
    CORE_ASSERT( ( i < m_component.size() ), "Index i out of bound" );
    return m_component[i];
}

inline void HandleData::recomputeAllIndices() {
    m_nameTable.clear();
    for ( uint i = 0; i < getComponentDataSize(); ++i ) {
        m_nameTable[m_component[i].m_name] = i;
    }
}

inline int HandleData::getIndexOf( const std::string& name ) const {
    auto it = m_nameTable.find( name );
    if ( it == m_nameTable.end() ) { return -1; }
    return it->second;
}

} // namespace Asset
} // namespace Core
} // namespace Ra
