#ifndef RADIUMENGINE_HANDLE_DATA_HPP
#define RADIUMENGINE_HANDLE_DATA_HPP

#include <map>
#include <string>
#include <vector>

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/RaCore.hpp>

#include <Core/File/AssetData.hpp>

namespace Ra {
namespace Asset {

/// A HandleComponentData stores the data of an animation Handle linked to an object.
struct RA_CORE_API HandleComponentData {
    RA_CORE_ALIGNED_NEW

    HandleComponentData();

    /// The transformation of the Handle.
    Core::Transform m_frame;

    /// The name of the Handle
    std::string m_name;

    /// The list of non-zero skinning weights for the vertices influenced by the Handle.
    std::vector<std::pair<uint, Scalar>> m_weight;
};

/// The HandleData class stores all the HandleComponentData linked to an object.
class RA_CORE_API HandleData : public AssetData {
  public:
    RA_CORE_ALIGNED_NEW

    /// The type of Handle system.
    enum HandleType { UNKNOWN = 1 << 0, POINT_CLOUD = 1 << 1, SKELETON = 1 << 2, CAGE = 1 << 3 };

    HandleData( const std::string& name = "", const HandleType& type = UNKNOWN );

    HandleData( const HandleData& data ) = default;

    ~HandleData();

    /// Set the name of the Handle system.
    inline void setName( const std::string& name );

    /// Return the type of the Handle system.
    inline HandleType getType() const;

    /// Set the type of the Handle system.
    inline void setType( const HandleType& type );

    /// Return the transformation of the Handle system.
    inline Core::Transform getFrame() const;

    /// Set the transformation of the Handle system.
    inline void setFrame( const Core::Transform& frame );

    /// Return the maximal number of vertices influenced by a Handle.
    inline uint getVertexSize() const;

    /// Set the maximal number of vertices influenced by a Handle.
    inline void setVertexSize( uint size );

    /// Set the map from Handle names to Handle storage index.
    inline void setNameTable( const std::map<std::string, uint>& nameTable );

    /// Recompute the map from Handle names to Handle storage index.
    inline void recomputeAllIndices();

    /// Return the number of Handles in the system.
    inline uint getComponentDataSize() const;

    /// Return the list of HandleComponentData.
    inline Core::AlignedStdVector<HandleComponentData>& getComponentData();

    /// Return the list of HandleComponentData.
    inline const Core::AlignedStdVector<HandleComponentData>& getComponentData() const;

    /// Return the \p i-th HandleComponentData.
    inline HandleComponentData& getComponent( const uint i );

    /// Return the \p i-th HandleComponentData.
    inline const HandleComponentData& getComponent( const uint i ) const;

    /// Set the HandleComponentData for the Handle system.
    inline void setComponents( const Core::AlignedStdVector<HandleComponentData>& components );

    /// Return the list of skeleton bone segments, i.e. pairs of indices of parented bones.
    inline Core::AlignedStdVector<Core::Vector2i>& getEdgeData();

    /// Return the list of skeleton bone segments, i.e. pairs of indices of parented bones.
    inline const Core::AlignedStdVector<Core::Vector2i>& getEdgeData() const;

    /// Set the list of skeleton bone segments, i.e. pairs of indices of parented bones.
    inline void setEdges( const Core::AlignedStdVector<Core::Vector2i>& edgeList );

    /// Toggle on/off addition of end bones.
    inline void needEndNodes( bool need );

    /// Return the list of cage faces.
    inline Core::AlignedStdVector<Core::VectorNi>& getFaceData();

    /// Return the list of cage faces.
    inline const Core::AlignedStdVector<Core::VectorNi>& getFaceData() const;

    /// set the list of cage faces.
    inline void setFaces( const Core::AlignedStdVector<Core::VectorNi>& faceList );

    /// Return true if the Handle system is a Point Cloud.
    inline bool isPointCloud() const;

    /// Return true if the Handle system is a Skeleton.
    inline bool isSkeleton() const;

    /// Return true if the Handle system is a Cage.
    inline bool isCage() const;

    /// Return true if the Handle system has Handles.
    inline bool hasComponents() const;

    /// Return true if the Handle system has bones segments.
    inline bool hasEdges() const;

    /// Return true if the Handle system has cage faces.
    inline bool hasFaces() const;

    /// Return true if the Handle system needs end bones.
    inline bool needsEndNodes() const;

    /// Return the storage index of the Handle with the given name
    /// if it exists, -1 otherwise.
    inline int getIndexOf( const std::string& name ) const;

    /// Print stat info to the Debug output.
    inline void displayInfo() const;

  protected:
    /// The transformation of the Handle system.
    Core::Transform m_frame;

    /// The type of the Handle system.
    HandleType m_type;

    /// Whether the Handle System needs end bones.
    bool m_endNode;

    /// The maximal number of vertices influenced by a Handle of the system.
    // FIXME: is this usefull?
    uint m_vertexSize;

    /// The map from Handle name to storage index.
    std::map<std::string, uint> m_nameTable;

    /// The list of HandleComponentData.
    Core::AlignedStdVector<HandleComponentData> m_component;

    /// The list of skeleton bones segments, i.e. bone parenthood.
    Core::AlignedStdVector<Core::Vector2i> m_edge;

    /// The list of cage faces.
    Core::AlignedStdVector<Core::VectorNi> m_face;
};

} // namespace Asset
} // namespace Ra

#include <Core/File/HandleData.inl>

#endif // RADIUMENGINE_HANDLE_DATA_HPP
