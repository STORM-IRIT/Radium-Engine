#pragma once

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/Utils/ObjectWithSemantic.hpp>
#include <unordered_map>

namespace Ra {
namespace Core {
namespace Geometry {

///
/// \brief Base class for index collections stored in MultiIndexedGeometry
class RA_CORE_API GeometryIndexLayerBase : public Utils::ObservableVoid,
                                           public Utils::ObjectWithSemantic
{
  public:
    inline explicit GeometryIndexLayerBase( const GeometryIndexLayerBase& other ) :
        ObjectWithSemantic( other.semantics() ) {}

    inline GeometryIndexLayerBase& operator=( const GeometryIndexLayerBase& other ) {
        CORE_UNUSED( other );
        CORE_ASSERT( semantics() == other.semantics(),
                     "Try to assign object with different semantics" );
        return *this;
    }
    inline GeometryIndexLayerBase& operator=( GeometryIndexLayerBase&& other ) {
        CORE_UNUSED( other );
        CORE_ASSERT( semantics() == other.semantics(),
                     "Try to assign GeometryIndexLayer of different type" );
        return *this;
    }

  protected:
    template <class... SemanticNames>
    inline GeometryIndexLayerBase( SemanticNames... names ) : ObjectWithSemantic( names... ) {}
};

/// \brief Typed index collection
template <typename T>
struct GeometryIndexLayer : public GeometryIndexLayerBase {
    using IndexType          = T;
    using IndexContainerType = VectorArray<IndexType>;

    inline IndexContainerType& collection() { return _collection; };
    const IndexContainerType& collection() const { return _collection; };

  protected:
    template <class... SemanticNames>
    inline GeometryIndexLayer( SemanticNames... names ) : GeometryIndexLayerBase( names... ) {}

  private:
    IndexContainerType _collection;
};

/// \brief AbstractGeometry with per-vertex attributes and layers of indices.
/// Each layer represents a different topology or indexing logic, e.g. triangle/line/quad
/// meshes, point-clouds.
///
/// Multiple layers are useful to share and maintain consistency of per-vertex attributes
/// between different meshes representing the same geometry, e.g., a quad and triangle mesh
/// layers connecting the same set of vertices.
///
/// ## Data-structure
/// It is designed as follow:
///  - Per-vertex attributes are stored as AttribArrayGeometry,
///  - Each layer of indices is represented as a GeometryIndexLayer, which inherits
///  Utils::ObjectWithSemantic
///    to store its semantics (Utils::ObjectWithSemantic::SemanticNameCollection), e.g.,
///    triangle/line/quad meshes, point-clouds.
///  - The collection of layers is stored as a map, indexed by #LayerKeyType, which is defined as
///  the union
///    of the layer name (set to "" by default) and semantics.
///
/// \see GeometryIndexLayerBase for more details about layers, semantics, and custom layers
/// definition. \see PointCloudIndexLayer, TriangleIndexLayer for examples of layers
///
/// ## Adding new layers
/// \see setLayer to add or update an existing layer.
///
/// Example of adding a PointCloudIndexLayer to an existing MultiIndexedGeometry `geo`:
/// \snippet tests/unittest/Core/indexview.cpp Creating and adding pointcloud layer
///
///
/// ## Accessing layers
/// Each layer is also associated with a `lock` state, used to give read-only or to lock write
/// access.
///
/// Layers can be accessed in different ways (see #containsLayer, #countLayers,
/// #getFirstLayerOccurrence, and #getLayer):
///  - query by name and semantics, by passing either #LayerKeyType or a pair of name/semantics
///  - query by semantics (Utils::ObjectWithSemantic::SemanticNameCollection), names are ignored.
///    Only the first occurrence found is returned when required.
///  - query by semantic name (Utils::ObjectWithSemantic::SemanticName): matches any layer including
///  the
///    given semantic name. Only the first occurrence found is returned when required.
///
/// \note Layer ordering is arbitrary and might change each time a new layer is added.
///
///
class RA_CORE_API MultiIndexedGeometry : public AttribArrayGeometry, public Utils::ObservableVoid
{
  public:
    using LayerSemanticCollection = Utils::ObjectWithSemantic::SemanticNameCollection;
    using LayerSemantic           = Utils::ObjectWithSemantic::SemanticName;
    using LayerKeyType            = std::pair<LayerSemanticCollection, std::string>;

    inline MultiIndexedGeometry() = default;
    explicit MultiIndexedGeometry( const MultiIndexedGeometry& other );
    explicit MultiIndexedGeometry( MultiIndexedGeometry&& other );
    explicit MultiIndexedGeometry( const AttribArrayGeometry& other );
    explicit MultiIndexedGeometry( AttribArrayGeometry&& other );
    MultiIndexedGeometry& operator=( const MultiIndexedGeometry& other );
    MultiIndexedGeometry& operator=( MultiIndexedGeometry&& other );

    void clear() override;

    /// \brief Copy only the geometry and the indices from \p other, but not the attributes.
    /// \see AttribArrayGeometry::copyBaseGeometry
    void copy( const MultiIndexedGeometry& other );

    /// \brief Check that the MultiIndexedGeometry is well built, asserting when it is not.
    /// \note Only compiles to something when in debug mode.
    void checkConsistency() const;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////

    /// \brief Check if at least one layer with such properties exists
    /// \param layerKey layer key
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    inline bool containsLayer( const LayerKeyType& layerKey ) const {
        return m_indices.find( layerKey ) != m_indices.end();
    }

    /// \copybrief containsLayer( const LayerKeyType& ) const
    ///
    /// Convenience function.
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \param layerName layer name
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    inline bool containsLayer( const LayerSemanticCollection& semantics,
                               const std::string& layerName ) const {
        return containsLayer( std::make_pair( semantics, layerName ) );
    }

    /// \copybrief containsLayer( const LayerKeyType& ) const
    ///
    /// Convenience function.
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    bool containsLayer( const LayerSemanticCollection& semantics ) const;

    /// \copybrief containsLayer( const LayerKeyType& ) const
    ///
    /// Convenience function.
    /// \param semanticName layer one semantic associated with the layer
    /// \complexity \f$ O(N) \f$, with \f$ N \f$ the number of semantic names in the collection
    bool containsLayer( const LayerSemantic& semanticName ) const;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////

    /// \brief Count the number of layer matching the input parameters
    /// \param layerKey layer key
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    inline size_t countLayers( const LayerKeyType& layerKey ) const {
        return m_indices.count( layerKey );
    }

    /// \copybrief countLayers( const LayerKeyType& ) const
    ///
    /// Convenience function.
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \param layerName layer name
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    inline size_t countLayers( const LayerSemanticCollection& semantics,
                               const std::string& layerName ) const {
        return countLayers( std::make_pair( semantics, layerName ) );
    }

    /// \copybrief countLayers( const LayerKeyType& ) const
    ///
    /// Convenience function.
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    size_t countLayers( const LayerSemanticCollection& semantics ) const;

    /// \copybrief countLayers( const LayerKeyType& ) const
    ///
    /// Convenience function.
    /// \param semanticName layer one semantic associated with the layer
    /// \complexity \f$ O(N) \f$, with \f$ N \f$ the number of semantic names in the collection
    size_t countLayers( const LayerSemantic& semanticName ) const;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////

    /// \brief Read-only access to a layer
    /// \param layerKey layer key
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    /// \throws std::out_of_range
    inline const GeometryIndexLayerBase& getLayer( const LayerKeyType& layerKey ) const {
        return *( m_indices.at( layerKey ).second );
    }

    /// \copybrief getLayer( const LayerKeyType& ) const
    ///
    /// Convenience function.
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \param layerName layer name
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    /// \throws std::out_of_range
    inline const GeometryIndexLayerBase& getLayer( const LayerSemanticCollection& semantics,
                                                   const std::string& layerName ) const {
        return getLayer( std::make_pair( semantics, layerName ) );
    }

    /// \copybrief getLayer( const LayerKeyType& ) const
    ///
    /// Convenience function.
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    /// \throws std::out_of_range
    const GeometryIndexLayerBase&
    getFirstLayerOccurrence( const LayerSemanticCollection& semantics ) const;

    /// \copybrief getLayer( const LayerKeyType& ) const
    ///
    /// Convenience function.
    /// \param semanticName layer one semantic associated with the layer
    /// \complexity \f$ O(N) \f$, with \f$ N \f$ the number of semantic names in the collection
    /// \throws std::out_of_range
    const GeometryIndexLayerBase&
    getFirstLayerOccurrence( const LayerSemantic& semanticName ) const;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////

    /// \brief Write access to a layer.
    ///
    /// Lock the layer for the caller, which needs to unlock after use,
    /// in order to release for other users and notify observers.
    /// \see unlockLayer( const LayerKeyType & )
    /// \param layerKey layer key
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    /// \throws std::out_of_range
    GeometryIndexLayerBase& getLayerWithLock( const LayerKeyType& layerKey );

    /// \copybrief getLayerWithLock( const LayerKeyType& )
    ///
    /// Convenience function.
    /// \see getLayerWithLock( const LayerKeyType& ) for details about locks
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \param layerName layer name
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    /// \throws std::out_of_range
    inline GeometryIndexLayerBase& getLayerWithLock( const LayerSemanticCollection& semantics,
                                                     const std::string& layerName ) {
        return getLayerWithLock( std::make_pair( semantics, layerName ) );
    }

    /// \copybrief getLayerWithLock( const LayerKeyType& )
    ///
    /// Convenience function.
    /// \see getLayerWithLock( const LayerKeyType& ) for details about locks
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    /// \throws std::out_of_range
    GeometryIndexLayerBase&
    getFirstLayerOccurrenceWithLock( const LayerSemanticCollection& semantics );

    /// \copybrief getLayerWithLock( const LayerKeyType& )
    ///
    /// Convenience function.
    /// \see getLayerWithLock( const LayerKeyType& ) for details about locks
    /// \param semanticName layer one semantic associated with the layer
    /// \complexity \f$ O(N) \f$, with \f$ N \f$ the number of semantic names in the collection
    /// \throws std::out_of_range
    GeometryIndexLayerBase& getFirstLayerOccurrenceWithLock( const LayerSemantic& semanticName );

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////

    /// \brief Unlock layer with write acces, notify observers of the update.
    ///
    /// \see getLayerWithLock( const LayerKeyType& ) for details about locks
    /// \param layerKey layer key
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    /// \throws std::out_of_range
    void unlockLayer( const LayerKeyType& layerKey );

    /// \copybrief unlockLayer( const LayerKeyType& )
    ///
    /// Convenience function.
    /// \see getLayerWithLock( const LayerKeyType& ) for details about locks
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \param layerName layer name
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    /// \throws std::out_of_range
    inline void unlockLayer( const LayerSemanticCollection& semantics,
                             const std::string& layerName ) {
        unlockLayer( std::make_pair( semantics, layerName ) );
    }

    /// \copybrief unlockLayer( const LayerKeyType& )
    ///
    /// Convenience function.
    /// \see getLayerWithLock( const LayerKeyType& ) for details about locks
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    /// \throws std::out_of_range
    void unlockFirstLayerOccurrence( const LayerSemanticCollection& semantics );

    /// \copybrief unlockLayer( const LayerKeyType& )
    ///
    /// Convenience function.
    /// \see getLayerWithLock( const LayerKeyType& ) for details about locks
    /// \param semanticName layer one semantic associated with the layer
    /// \complexity \f$ O(N) \f$, with \f$ N \f$ the number of semantic names in the collection
    /// \throws std::out_of_range
    void unlockFirstLayerOccurrence( const LayerSemantic& semanticName );

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////

    /// \brief Add layer
    ///
    /// Notify observers of the update.
    /// \return false if a layer with same semantics and name already exists.
    ///
    /// \warning Takes the ownership of the layer
    ///
    bool addLayer( std::unique_ptr<GeometryIndexLayerBase>&& layer,
                   const std::string& layerName = "" );

  private:
    /// Note: we cannot store unique_ptr here has unordered_map needs its
    /// elements to be copy-constructible
    using EntryType = std::pair<bool, GeometryIndexLayerBase*>;
    struct RA_CORE_API KeyHash {
        std::size_t operator()( const LayerKeyType& k ) const;
    };

    /// Collection of pairs <lockStatus, Indices>
    /// \note There is no natural ordering for these elements, thus
    /// we need an unordered_map. In contrast to map, transparent hashing
    /// require c++20, so we need to implement them explicitely here
    /// https://en.cppreference.com/w/cpp/container/unordered_map/find
    std::unordered_map<LayerKeyType, EntryType, KeyHash> m_indices;
};

struct RA_CORE_API PointCloudIndexLayer : public GeometryIndexLayer<Vector1ui> {
    inline PointCloudIndexLayer() : GeometryIndexLayer( "IndexPointCloud" ) {}
    /// \brief Generate linearly spaced indices with same size as \p attr vertex buffer
    void generateIndicesFromAttributes( const AttribArrayGeometry& attr );

  protected:
    template <class... SemanticNames>
    inline PointCloudIndexLayer( SemanticNames... names ) :
        GeometryIndexLayer( "IndexPointCloud", names... ) {}
};

struct RA_CORE_API TriangleIndexLayer : public GeometryIndexLayer<Vector3ui> {
    inline TriangleIndexLayer() : GeometryIndexLayer( "TriangleMesh" ) {}

  protected:
    template <class... SemanticNames>
    inline TriangleIndexLayer( SemanticNames... names ) :
        GeometryIndexLayer( "TriangleMesh", names... ) {}
};

class RA_CORE_API PolyIndexLayer : public GeometryIndexLayer<VectorNui>
{
    inline PolyIndexLayer() : GeometryIndexLayer( "PolyMesh" ) {}

  protected:
    template <class... SemanticNames>
    inline PolyIndexLayer( SemanticNames... names ) : GeometryIndexLayer( "PolyMesh", names... ) {}
};

class RA_CORE_API LineIndexLayer : public GeometryIndexLayer<Vector2ui>
{
    inline LineIndexLayer() : GeometryIndexLayer( "LineMesh" ) {}

  protected:
    template <class... SemanticNames>
    inline LineIndexLayer( SemanticNames... names ) : GeometryIndexLayer( "LineMesh", names... ) {}
};

} // namespace Geometry
} // namespace Core
} // namespace Ra
