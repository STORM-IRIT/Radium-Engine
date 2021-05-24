#pragma once

#include <Core/Containers/VectorArray.hpp>

#include <Core/Geometry/TriangleMesh.hpp>

#include <Core/Utils/ObjectWithSemantic.hpp>
#include <Core/Utils/StdMapIterators.hpp>

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
    /// these ctor and assignement operators do not copy observers
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
    virtual ~GeometryIndexLayerBase() {}

    /// \brief Create new layer with duplicated content
    virtual GeometryIndexLayerBase* duplicate() = 0;

    /// \brief Append content from another layer
    /// \return false if data cannot be appended, e.g., different semantics
    virtual bool append( const GeometryIndexLayerBase& other ) = 0;

    /// \brief Compare if two layers have the same content
    virtual inline bool operator==( const GeometryIndexLayerBase& /*other*/ ) const {
        return false;
    }

    /// \return the number of index (i.e. "faces") contained in the layer.
    virtual size_t size() = 0;

  protected:
    template <class... SemanticNames>
    inline GeometryIndexLayerBase( SemanticNames... names ) : ObjectWithSemantic( names... ) {}
};

/// \brief Typed index collection
template <typename T>
struct GeometryIndexLayer : public GeometryIndexLayerBase {
    using IndexType          = T;
    using IndexContainerType = VectorArray<IndexType>;

    inline IndexContainerType& collection() { return m_collection; };
    const IndexContainerType& collection() const { return m_collection; };

    inline bool append( const GeometryIndexLayerBase& other ) final {
        if ( shareSemantic( other ) )
        {
            const auto& othercasted = static_cast<const GeometryIndexLayer<T>&>( other );
            m_collection.insert( m_collection.end(),
                                 othercasted.collection().begin(),
                                 othercasted.collection().end() );
            return true;
        }
        return false;
    }

    /// \warning Does not account for elements permutations
    inline bool operator==( const GeometryIndexLayerBase& other ) const final {
        if ( shareSemantic( other ) )
        {
            const auto& othercasted = static_cast<const GeometryIndexLayer<T>&>( other );
            return othercasted.collection() == m_collection;
        }
        return false;
    }

    inline size_t size() override { return m_collection.size(); }

    virtual inline GeometryIndexLayerBase* duplicate() {
        auto copy          = new GeometryIndexLayer<T>( *this );
        copy->m_collection = m_collection;
        return copy;
    }

  protected:
    template <class... SemanticNames>
    inline GeometryIndexLayer( SemanticNames... names ) : GeometryIndexLayerBase( names... ) {}

  private:
    IndexContainerType m_collection;
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

    using PointAttribHandle  = AttribArrayGeometry::PointAttribHandle;
    using NormalAttribHandle = AttribArrayGeometry::NormalAttribHandle;
    using FloatAttribHandle  = AttribArrayGeometry::FloatAttribHandle;
    using Vec2AttribHandle   = AttribArrayGeometry::Vec2AttribHandle;
    using Vec3AttribHandle   = AttribArrayGeometry::Vec3AttribHandle;
    using Vec4AttribHandle   = AttribArrayGeometry::Vec4AttribHandle;

    inline MultiIndexedGeometry() = default;
    explicit MultiIndexedGeometry( const MultiIndexedGeometry& other );
    explicit MultiIndexedGeometry( MultiIndexedGeometry&& other );
    explicit MultiIndexedGeometry( const AttribArrayGeometry& other );
    explicit MultiIndexedGeometry( AttribArrayGeometry&& other );
    MultiIndexedGeometry& operator=( const MultiIndexedGeometry& other );
    MultiIndexedGeometry& operator=( MultiIndexedGeometry&& other );

    virtual inline ~MultiIndexedGeometry() { clear(); }

    void clear() override;

    /// \brief Copy geometry and indices from \p others.
    /// \see AttribArrayGeometry::copyBaseGeometry
    void copy( const MultiIndexedGeometry& other );

    /// \brief Check that the MultiIndexedGeometry is well built, asserting when it is not.
    /// \note Only compiles to something when in debug mode.
    void checkConsistency() const;

    /// Append another MultiIndexedGeometry to this one. Layers with same
    /// name/semantics are concatenated, and other layers are ignored
    /// \return true if all fields have been copied
    bool append( const MultiIndexedGeometry& other );

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
        return containsLayer( {semantics, layerName} );
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
        return countLayers( {semantics, layerName} );
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
        return getLayer( {semantics, layerName} );
    }

    /// \copybrief getLayer( const LayerKeyType& ) const
    ///
    /// Convenience function.
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    /// \return The layer and its LayerKey (to be used with getLayer, getLayerWithLock, unlockLayer)
    /// \throws std::out_of_range
    std::pair<LayerKeyType, const GeometryIndexLayerBase&>
    getFirstLayerOccurrence( const LayerSemanticCollection& semantics ) const;

    /// \copybrief getLayer( const LayerKeyType& ) const
    ///
    /// Convenience function.
    /// \param semanticName layer one semantic associated with the layer
    /// \complexity \f$ O(N) \f$, with \f$ N \f$ the number of semantic names in the collection
    /// \return The layer and its LayerKey (to be used with getLayer, getLayerWithLock, unlockLayer)
    /// \throws std::out_of_range
    std::pair<LayerKeyType, const GeometryIndexLayerBase&>
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
        return getLayerWithLock( {semantics, layerName} );
    }

    /// \copybrief getLayerWithLock( const LayerKeyType& )
    ///
    /// Convenience function.
    /// \see getLayerWithLock( const LayerKeyType& ) for details about locks
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    /// \return The layer and its LayerKey (to be used with getLayer, getLayerWithLock, unlockLayer)
    /// \throws std::out_of_range
    std::pair<LayerKeyType, GeometryIndexLayerBase&>
    getFirstLayerOccurrenceWithLock( const LayerSemanticCollection& semantics );

    /// \copybrief getLayerWithLock( const LayerKeyType& )
    ///
    /// Convenience function.
    /// \see getLayerWithLock( const LayerKeyType& ) for details about locks
    /// \param semanticName layer one semantic associated with the layer
    /// \complexity \f$ O(N) \f$, with \f$ N \f$ the number of semantic names in the collection
    /// \return The layer and its LayerKey (to be used with getLayer, getLayerWithLock, unlockLayer)
    /// \throws std::out_of_range
    std::pair<LayerKeyType, GeometryIndexLayerBase&>
    getFirstLayerOccurrenceWithLock( const LayerSemantic& semanticName );

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
        unlockLayer( {semantics, layerName} );
    }

    // The following methods are only mean to be used by PredifinedIndexGeometry and should not be
    // part of the final API
  protected:
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
  public:
    /// \brief Add layer
    ///
    /// Notify observers of the update.
    /// \return false if a layer with same semantics and name already exists.
    ///
    /// \warning Takes the ownership of the layer
    ///
    bool addLayer( std::unique_ptr<GeometryIndexLayerBase>&& layer,
                   const std::string& layerName = "" );

    /// \brief Range on layer keys (read-only)
    ///
    /// Usage:
    /// \snippet tests/unittest/Core/indexview.cpp Iterating over layer keys
    [[nodiscard]] inline auto layerKeys() const { return Utils::map_keys( m_indices ); }

  private:
    /// \brief Duplicate attributes stored as pointers
    void deepCopy( const MultiIndexedGeometry& other );

    /// \brief Clear attributes stored as pointers
    void deepClear();

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
    inline PointCloudIndexLayer() : GeometryIndexLayer( staticSemanticName ) {}
    /// \brief Generate linearly spaced indices with same size as \p attr vertex buffer
    void linearIndices( const AttribArrayGeometry& attr );

    static constexpr const char* staticSemanticName = "PointCloud";

  protected:
    template <class... SemanticNames>
    inline PointCloudIndexLayer( SemanticNames... names ) :
        GeometryIndexLayer( staticSemanticName, names... ) {}
};

struct RA_CORE_API TriangleIndexLayer : public GeometryIndexLayer<Vector3ui> {
    inline TriangleIndexLayer() : GeometryIndexLayer( staticSemanticName ) {}

    static constexpr const char* staticSemanticName = "TriangleMesh";

  protected:
    template <class... SemanticNames>
    inline TriangleIndexLayer( SemanticNames... names ) :
        GeometryIndexLayer( staticSemanticName, names... ) {}
};

struct RA_CORE_API PolyIndexLayer : public GeometryIndexLayer<VectorNui> {
    inline PolyIndexLayer() : GeometryIndexLayer( staticSemanticName ) {}

    static constexpr const char* staticSemanticName = "PolyMesh";

  protected:
    template <class... SemanticNames>
    inline PolyIndexLayer( SemanticNames... names ) :
        GeometryIndexLayer( staticSemanticName, names... ) {}
};

struct RA_CORE_API LineIndexLayer : public GeometryIndexLayer<Vector2ui> {
    inline LineIndexLayer() : GeometryIndexLayer( staticSemanticName ) {}

    static constexpr const char* staticSemanticName = "LineMesh";

  protected:
    template <class... SemanticNames>
    inline LineIndexLayer( SemanticNames... names ) :
        GeometryIndexLayer( staticSemanticName, names... ) {}
};

/// Temporary class providing the old API for TriangleMesh, LineMesh and PolyMesh
/// This class will be marked as deprecated soon.
namespace IndexLayerType {
template <class IndexT>
struct getType {};

template <>
struct getType<Vector2ui> {
    using Type = Ra::Core::Geometry::LineIndexLayer;
};

template <>
struct getType<Vector3ui> {
    using Type = Ra::Core::Geometry::TriangleIndexLayer;
};

template <>
struct getType<VectorNui> {
    using Type = Ra::Core::Geometry::PolyIndexLayer;
};

template <>
struct getType<Vector1ui> {
    using Type = Ra::Core::Geometry::PointCloudIndexLayer;
};

} // namespace IndexLayerType

/**
 * \brief A single layer MultiIndexedGeometry.
 * This class actually provide compatibility with old geometry with a main layer.
 * Main layer contains indices of a specific type (point, line, triangle, poly).
 * Derived classes explicit the kind of indices of the main layer.
 * Since IndexedGeometry is a MultiIndexedGeometry, one can add index layer on the fly.
 * \warning will be depracated when MultiIndexedGeometry will be supported directly on the engine
 * side.
 */
template <typename T>
class IndexedGeometry : public MultiIndexedGeometry
{
  public:
    using IndexType          = T;
    using IndexContainerType = VectorArray<IndexType>;

  private:
    using DefaultLayerType = typename IndexLayerType::getType<IndexType>::Type;

  public:
    inline IndexedGeometry() {
        auto layer          = std::make_unique<DefaultLayerType>();
        m_mainIndexLayerKey = {layer->semantics(), ""};
        addLayer( std::move( layer ) );
    }

    inline const IndexContainerType& getIndices() const {
        const auto& abstractLayer = getLayer( m_mainIndexLayerKey );
        return static_cast<const DefaultLayerType&>( abstractLayer ).collection();
    }

    /// read write access to indices.
    /// Cause indices to be "lock" for the caller
    /// need to be unlock by the caller before any one can ask for write access.
    inline IndexContainerType& getIndicesWithLock() {
        auto& abstractLayer = getLayerWithLock( m_mainIndexLayerKey );
        return static_cast<DefaultLayerType&>( abstractLayer ).collection();
    }

    /// unlock previously read write acces, notify observers of the update.
    inline void indicesUnlock() { unlockLayer( m_mainIndexLayerKey ); }

    /// set indices. Indices must be unlock, i.e. no one should have write
    /// access to it.
    /// Notify observers of the update.
    inline void setIndices( IndexContainerType&& indices ) {
        auto& abstractLayer = getLayerWithLock( m_mainIndexLayerKey );
        static_cast<DefaultLayerType&>( abstractLayer ).collection() = std::move( indices );
        notify();
    }
    inline void setIndices( const IndexContainerType& indices ) {
        auto& abstractLayer = getLayerWithLock( m_mainIndexLayerKey );
        static_cast<DefaultLayerType&>( abstractLayer ).collection() = indices;
        notify();
    }
    inline const LayerKeyType& getLayerKey() const { return m_mainIndexLayerKey; }

  private:
    LayerKeyType m_mainIndexLayerKey;
};

class RA_CORE_API IndexedPointCloud : public IndexedGeometry<Vector1ui>
{};

class RA_CORE_API TriangleMesh : public IndexedGeometry<Vector3ui>
{};

class RA_CORE_API PolyMesh : public IndexedGeometry<VectorNui>
{};

class RA_CORE_API LineMesh : public IndexedGeometry<Vector2ui>
{};

} // namespace Geometry
} // namespace Core
} // namespace Ra
