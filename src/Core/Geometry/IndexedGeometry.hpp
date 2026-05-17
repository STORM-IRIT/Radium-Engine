#pragma once

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/AttribArrayGeometry.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/ContainerIntrospectionInterface.hpp>
#include <Core/Utils/ObjectWithSemantic.hpp>
#include <Core/Utils/StdMapIterators.hpp>
#include <Core/Utils/StdUtils.hpp>

#include <memory>
#include <unordered_map>

namespace Ra {
namespace Core {
namespace Geometry {
namespace Helper {
template <typename T>
VectorArray<Vector3ui> triangulate( const VectorArray<T>& in ) {
    VectorArray<Vector3ui> out;

    out.reserve( in.size() );
    for ( const auto& face : in ) {
        if ( face.size() == 3 ) { out.push_back( face ); }
        else {
            /// simple sew triangulation
            int minus { int( face.size() ) - 1 };
            int plus { 0 };
            while ( plus + 1 < minus ) {
                if ( ( plus - minus ) % 2 ) {
                    out.emplace_back( face[plus], face[plus + 1], face[minus] );
                    ++plus;
                }
                else {
                    out.emplace_back( face[minus], face[plus], face[minus - 1] );
                    --minus;
                }
            }
        }
    }
    return out;
}

template <>
inline VectorArray<Vector3ui> triangulate( const VectorArray<Vector4ui>& in ) {
    VectorArray<Vector3ui> out;
    out.reserve( 2 * in.size() );
    // assume quads are convex
    for ( const auto& face : in ) {
        out.emplace_back( face[0], face[1], face[2] );
        out.emplace_back( face[0], face[2], face[3] );
    }
    return out;
}
}
/**
 * \brief Base class for index collections stored in MultiIndexedGeometry.
 */
class RA_CORE_API GeometryIndexLayerBase : public Utils::ObservableVoid,
                                           public Utils::ObjectWithSemantic,
                                           public Utils::ContainerIntrospectionInterface
{
  public:
    /// \brief Copy constructor
    /// \note Do not copy observers
    inline explicit GeometryIndexLayerBase( const GeometryIndexLayerBase& other );
    /// \brief Assignment operator
    /// \copydetails GeometryIndexLayerBase(const GeometryIndexLayerBase&)
    inline GeometryIndexLayerBase& operator=( const GeometryIndexLayerBase& other );
    /// \brief Move assignment operator
    /// \copydetails GeometryIndexLayerBase(const GeometryIndexLayerBase&)
    inline GeometryIndexLayerBase& operator=( GeometryIndexLayerBase&& other );
    virtual ~GeometryIndexLayerBase() {}

    /// \brief Create new layer with duplicated content
    virtual std::unique_ptr<GeometryIndexLayerBase> clone() = 0;

    /// \brief Append content from another layer
    /// \return false if data cannot be appended, e.g., different semantics
    virtual bool append( const GeometryIndexLayerBase& other, int offset = 0 ) = 0;
    virtual void offset( int offset, uint start_index = 0 )                    = 0;

    /// \brief Compare if two layers have the same content
    virtual inline bool operator==( const GeometryIndexLayerBase& ) const { return false; }

  protected:
    /// \brief Hidden constructor that must be called by inheriting classes to define the object
    ///        semantics.
    template <class... SemanticNames>
    inline GeometryIndexLayerBase( SemanticNames... names ) : ObjectWithSemantic( names... ) {}
};

/**
 * \brief Typed index collection.
 */
template <typename T>
struct GeometryIndexLayer : public GeometryIndexLayerBase {
    using IndexType          = T;
    using IndexContainerType = VectorArray<IndexType>;

    inline IndexContainerType& collection() { return m_collection; }
    const IndexContainerType& collection() const { return m_collection; }

    inline bool append( const GeometryIndexLayerBase& other, int offset = 0 ) final;
    inline void offset( int offset, uint start_index = 0 ) final;
    /// \warning Does not account for elements permutations
    inline bool operator==( const GeometryIndexLayerBase& other ) const final;

    inline size_t getSize() const override final { return m_collection.size(); }

    inline std::unique_ptr<GeometryIndexLayerBase> clone() override;

    inline size_t getNumberOfComponents() const override final;

    inline size_t getBufferSize() const override final;

    /// \warning it's meaningful only if the attrib do not contain heap
    /// allocated data.
    inline int getStride() const override final;

    inline const void* dataPtr() const override final;

  protected:
    template <class... SemanticNames>
    inline GeometryIndexLayer( SemanticNames... names ) : GeometryIndexLayerBase( names... ) {}

  private:
    IndexContainerType m_collection;
};

#define INDEX_LAYER_CLONE_IMPLEMENTATION( TYPE )                      \
    inline std::unique_ptr<GeometryIndexLayerBase> clone() override { \
        auto copy          = std::make_unique<TYPE>( *this );         \
        copy->collection() = collection();                            \
        return copy;                                                  \
    }

#define OPEN_DECLARATION_INDEX_LAYER( NAME, TYPE )                        \
    struct RA_CORE_API NAME : public GeometryIndexLayer<TYPE> {           \
        inline NAME() : GeometryIndexLayer( NAME::staticSemanticName ) {} \
        static constexpr const char* staticSemanticName = #NAME;          \
        INDEX_LAYER_CLONE_IMPLEMENTATION( NAME )                          \
      protected:                                                          \
        template <class... SemanticNames>                                 \
        inline explicit NAME( SemanticNames... names ) :                  \
            GeometryIndexLayer( NAME::staticSemanticName, names... ) {}   \
                                                                          \
      public:

#define DECLARE_INDEX_LAYER( NAME, TYPE )      \
    OPEN_DECLARATION_INDEX_LAYER( NAME, TYPE ) \
    }                                          \
    ;

DECLARE_INDEX_LAYER( InvalidIndexLayer, Vector1ui );

/**
 * \name Predefined index layers
 * The use of these layers helps in generic management of geometries
 * \{
 */
/// \brief Index layer for a point cloud
OPEN_DECLARATION_INDEX_LAYER( PointCloudIndexLayer, Vector1ui )

/// \brief Constructor of an index layer with linearly spaced indices ranging from \f$0\f$ to
/// \f$n-1\f$
inline explicit PointCloudIndexLayer( size_t n ) :
    GeometryIndexLayer( PointCloudIndexLayer::staticSemanticName ) {
    collection().resize( n );
    collection().getMap() = IndexContainerType::Matrix::LinSpaced( n, 0, n - 1 );
}

/// \brief Generate linearly spaced indices with same size as \p attr vertex buffer
void linearIndices( const AttribArrayGeometry& attr ) {
    auto nbVert = attr.vertices().size();
    collection().resize( nbVert );
    collection().getMap() = IndexContainerType::Matrix::LinSpaced( nbVert, 0, nbVert - 1 );
}

};

/**
 * \brief Index layer for triangle mesh.
 *
 * \note, This layer ensures that all faces have exactly 3 vertices.
 */
DECLARE_INDEX_LAYER( TriangleIndexLayer, Vector3ui )

/**
 * \brief Index layer for quadrilateral mesh.
 *
 * \note, This layer ensures that all faces have exactly 4 vertices
 */
DECLARE_INDEX_LAYER( QuadIndexLayer, Vector4ui )

/**
 * \brief Index layer for polygonal mesh.
 *
 * \note, Using this layer, all faces might have more than 4 vertices or have different number of
 * vertices.
 */
DECLARE_INDEX_LAYER( PolyIndexLayer, VectorNui )

/** one Ni index -> one strip */
DECLARE_INDEX_LAYER( StripOrFanIndexLayer, VectorNui )

/**
 * \brief Index layer for line mesh.
 *
 * \note, This layer ensures that all faces have exactly 2 vertices
 */
DECLARE_INDEX_LAYER( LineIndexLayer, Vector2ui )

/**
 * \}
 */

#undef INDEX_LAYER_CLONE_IMPLEMENTATION
#undef OPEN_DECLARATION_INDEX_LAYER
#undef DECLARE_INDEX_LAYER

/**
 * \brief AbstractGeometry with per-vertex attributes and layers of indices.
 * Each layer represents a different topology or indexing logic, e.g. triangle/line/quad
 * meshes, point-clouds.
 *
 * Multiple layers are useful to share and maintain consistency of per-vertex attributes
 * between different meshes representing the same geometry, e.g., a quad and triangle mesh
 * layers connecting the same set of vertices.
 *
 * ## Data-structure
 * It is designed as follow:
 *  - Per-vertex attributes are stored as AttribArrayGeometry,
 *  - Each layer of indices is represented as a GeometryIndexLayer, which inherits
 *  Utils::ObjectWithSemantic
 *    to store its semantics (Utils::ObjectWithSemantic::SemanticNameCollection), e.g.,
 *    triangle/line/quad meshes, point-clouds.
 *  - The collection of layers is stored as a map, indexed by #LayerKeyType, which is defined as
 *  the union
 *    of the layer name (set to "" by default) and semantics.
 *
 * \see GeometryIndexLayerBase for more details about layers, semantics, and custom layers
 * definition. \see PointCloudIndexLayer, TriangleIndexLayer for examples of layers
 *
 * ## Adding new layers
 * \see setLayer to add or update an existing layer.
 *
 * Example of adding a PointCloudIndexLayer to an existing MultiIndexedGeometry `geo`:
 * \snippet tests/unittest/Core/indexview.cpp Creating and adding pointcloud layer
 *
 *
 * ## Accessing layers
 * Each layer is also associated with a `lock` state, used to give read-only or to lock write
 * access.
 *
 * Layers can be accessed in different ways (see #containsLayer, #countLayers,
 * #getFirstLayerOccurrence, and #getLayer):
 *  - query by name and semantics, by passing either #LayerKeyType or a pair of name/semantics
 *  - query by semantics (Utils::ObjectWithSemantic::SemanticNameCollection), names are ignored.
 *    Only the first occurrence found is returned when required.
 *  - query by semantic name (Utils::ObjectWithSemantic::SemanticName): matches any layer including
 *  the
 *    given semantic name. Only the first occurrence found is returned when required.
 *
 * \note Layer ordering is arbitrary and might change each time a new layer is added.
 *
 */

class RA_CORE_API MultiIndexedGeometry : public AttribArrayGeometry, public Utils::ObservableVoid
{
  public:
    using LayerSemanticCollection = Utils::ObjectWithSemantic::SemanticNameCollection;
    using LayerSemantic           = Utils::ObjectWithSemantic::SemanticName;
    using LayerKeyType            = std::pair<LayerSemanticCollection, std::string>;

    /// Hash function for layer keys
    struct RA_CORE_API LayerKeyHash {
        std::size_t operator()( const LayerKeyType& k ) const;
    };
    /// bool -> locked, ptr -> actual data
    using LayerEntryType  = std::pair<bool, std::unique_ptr<GeometryIndexLayerBase>>;
    using IndexCollection = std::unordered_map<LayerKeyType, LayerEntryType, LayerKeyHash>;

    using PointAttribHandle  = AttribArrayGeometry::PointAttribHandle;
    using NormalAttribHandle = AttribArrayGeometry::NormalAttribHandle;
    using FloatAttribHandle  = AttribArrayGeometry::FloatAttribHandle;
    using Vec2AttribHandle   = AttribArrayGeometry::Vec2AttribHandle;
    using Vec3AttribHandle   = AttribArrayGeometry::Vec3AttribHandle;
    using Vec4AttribHandle   = AttribArrayGeometry::Vec4AttribHandle;

    MultiIndexedGeometry() = default;
    MultiIndexedGeometry( const MultiIndexedGeometry& other );
    MultiIndexedGeometry( MultiIndexedGeometry&& other );
    MultiIndexedGeometry( const AttribArrayGeometry& other );
    MultiIndexedGeometry( AttribArrayGeometry&& other );
    MultiIndexedGeometry& operator=( const MultiIndexedGeometry& other );
    MultiIndexedGeometry& operator=( MultiIndexedGeometry&& other );

    virtual ~MultiIndexedGeometry();
    void clear() override;

    /// \brief Copy geometry and indices from \p others.
    /// \see AttribArrayGeometry::copyBaseGeometry
    void copy( const MultiIndexedGeometry& other );

    /// \brief Check that the MultiIndexedGeometry is well built, asserting when it is not.
    /// \note Only compiles to something when in debug mode.
    void checkConsistency() const;

    /// Append another MultiIndexedGeometry to this one. Layers with same
    /// name/semantics are concatenated, and other layers are merged.
    /// \return true if all fields have been copied
    bool append( const MultiIndexedGeometry& other );

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////

    /// \brief Check if at least one layer with such properties exists
    /// \param layerKey layer key
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    bool containsLayer( const LayerKeyType& layerKey ) const;

    /// \copybrief containsLayer( const LayerKeyType& ) const
    ///
    /// Convenience function.
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \param layerName layer name
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    bool containsLayer( const LayerSemanticCollection& semantics,
                        const std::string& layerName ) const;

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
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of semantic names in the collection
    bool containsLayer( const LayerSemantic& semanticName ) const;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////

    /// \brief Count the number of layer matching the input parameters
    /// \param layerKey layer key
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    size_t countLayers( const LayerKeyType& layerKey ) const;

    /// \copybrief countLayers( const LayerKeyType& ) const
    ///
    /// Convenience function.
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \param layerName layer name
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    size_t countLayers( const LayerSemanticCollection& semantics,
                        const std::string& layerName ) const;

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
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of semantic names in the collection
    size_t countLayers( const LayerSemantic& semanticName ) const;

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////

    /// \brief Read-only access to a layer
    /// \param layerKey layer key
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    /// \throws std::out_of_range
    const GeometryIndexLayerBase& getLayer( const LayerKeyType& layerKey ) const;

    /// \copybrief getLayer( const LayerKeyType& ) const
    ///
    /// Convenience function.
    /// \param semantics collection of semantics associated with the layer (they should all match)
    /// \param layerName layer name
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of layers in the collection
    /// \throws std::out_of_range
    const GeometryIndexLayerBase& getLayer( const LayerSemanticCollection& semantics,
                                            const std::string& layerName ) const;
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
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of semantic names in the collection
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
    GeometryIndexLayerBase& getLayerWithLock( const LayerSemanticCollection& semantics,
                                              const std::string& layerName );

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
    /// \complexity \f$ O(n) \f$, with \f$ n \f$ the number of semantic names in the collection
    /// \return The layer and its LayerKey (to be used with getLayer, getLayerWithLock, unlockLayer)
    /// \throws std::out_of_range
    std::pair<LayerKeyType, GeometryIndexLayerBase&>
    getFirstLayerOccurrenceWithLock( const LayerSemantic& semanticName );
    auto getFirstLayerIteratorWithLock( const LayerSemantic& semanticName )
        -> IndexCollection::iterator {
        return std::find_if( m_indices.begin(),
                             m_indices.end(),
                             [&semanticName]( const IndexCollection::value_type& v ) {
                                 return Utils::hasSemantic( v.first.first, semanticName );
                             } );
    }

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
    void unlockLayer( const LayerSemanticCollection& semantics, const std::string& layerName );

    template <typename IndexLayer>
    LayerKeyType set_indices( const typename IndexLayer::IndexContainerType& indices ) {
        auto own_indices = indices;
        return set_indices<IndexLayer>( std::move( own_indices ) );
    }

    template <typename IndexLayer>
    void set_indices( LayerKeyType layer_key,
                      const typename IndexLayer::IndexContainerType& indices ) {
        auto own_indices = indices;
        set_indices<IndexLayer>( layer_key, std::move( own_indices ) );
    }

    template <typename IndexLayer>
    LayerKeyType set_indices( typename IndexLayer::IndexContainerType&& indices ) {
        auto itr = getFirstLayerIteratorWithLock( IndexLayer::staticSemanticName );
        LayerKeyType layer_key;
        GeometryIndexLayerBase* locked_layer = nullptr;
        if ( itr == m_indices.end() ) {
            auto added   = addLayer( std::move( std::make_unique<IndexLayer>() ) );
            layer_key    = added.second;
            locked_layer = &getLayerWithLock( layer_key );
        }
        else {
            layer_key    = itr->first;
            locked_layer = itr->second.second.get();
        }
        set_indices<IndexLayer>( layer_key, *locked_layer, std::move( indices ) );
        return layer_key;
    }

    template <typename IndexLayer>
    void set_indices( LayerKeyType layer_key, typename IndexLayer::IndexContainerType&& indices ) {
        auto& locked_layer = getLayerWithLock( layer_key );
        set_indices<IndexLayer>( layer_key, locked_layer, std::move( indices ) );
    }

    template <typename IndexLayer>
    const typename IndexLayer::IndexContainerType& indices() const {
        auto result = getFirstLayerOccurrence( IndexLayer::staticSemanticName );
        return static_cast<const IndexLayer&>( result.second ).collection();
    }

    /// Call triangulate, if there isn't any triangle index, on the first found index between quad
    /// and poly.
    /// \return the layer key of the triangle layer (old or new)
    LayerKeyType triangulate_any() {
        if ( !containsLayer( TriangleIndexLayer::staticSemanticName ) ) {
            if ( containsLayer( QuadIndexLayer::staticSemanticName ) )
                return triangulate<QuadIndexLayer>();
            if ( containsLayer( PolyIndexLayer::staticSemanticName ) )
                return triangulate<PolyIndexLayer>();
        }
        return getFirstLayerOccurrence( TriangleIndexLayer::staticSemanticName ).first;
    }

    /// triangulate IndexLayer if there isn't any triangle index yet.
    /// \return the layer key of the triangle layer (old or new)
    template <typename IndexLayer>
    LayerKeyType triangulate() {
        if ( !containsLayer( TriangleIndexLayer::staticSemanticName ) ) {
            auto result = getFirstLayerOccurrence( IndexLayer::staticSemanticName );
            auto& layer = static_cast<const IndexLayer&>( result.second ).collection();

            auto triangle_layer          = std::make_unique<TriangleIndexLayer>();
            triangle_layer->collection() = Helper::triangulate( layer );
            return addLayer( std::move( triangle_layer ) ).second;
        }
        return getFirstLayerOccurrence( TriangleIndexLayer::staticSemanticName ).first;
    }

    template <typename IndexLayer>
    auto indices_with_lock() {
        auto itr = getFirstLayerIteratorWithLock( IndexLayer::staticSemanticName );
        LayerKeyType layer_key;
        GeometryIndexLayerBase* locked_layer = nullptr;
        if ( itr == m_indices.end() ) {
            auto added   = addLayer( std::move( std::make_unique<IndexLayer>() ) );
            layer_key    = added.second;
            locked_layer = &getLayerWithLock( layer_key );
        }
        else {
            layer_key    = itr->first;
            locked_layer = itr->second.second.get();
        }
        return std::pair<LayerKeyType, typename IndexLayer::IndexContainerType&>(
            layer_key, static_cast<IndexLayer&>( *locked_layer ).collection() );
    }

  protected:
    //////////////////////////////////////////////////////////////////////
    template <typename IndexLayer>
    void set_indices( LayerKeyType layer_key,
                      GeometryIndexLayerBase& locked_layer,
                      IndexLayer::IndexContainerType&& indices ) {
        static_cast<IndexLayer&>( locked_layer ).collection() = std::move( indices );
        unlockLayer( layer_key );
        notify();
    }

    //////////////////////////////////////////////////////////////////////
  public:
    /// \brief Add layer
    ///
    /// If not inserted, the pointer is deleted. So the caller must ensure this possible
    /// deletion is safe before calling this method.
    ///
    /// Notify observers of the update.
    /// If first added layer, default layer key is set to this layer.
    ///
    /// \return false if a layer with same semantics and name already exists.
    ///
    /// \warning Takes the ownership of the layer
    ///
    auto addLayer( std::unique_ptr<GeometryIndexLayerBase>&& layer,
                   const bool withLock          = false,
                   const std::string& layerName = "" ) -> std::pair<bool, LayerKeyType>;

    /// \brief Range on layer keys (read-only)
    ///
    /// Usage:
    /// \snippet tests/unittest/Core/indexview.cpp Iterating over layer keys
    [[nodiscard]] auto layerKeys() const;

    /// returs default layer key, initialized to InvalidLayerKey,
    const LayerKeyType& default_layer_key() const { return m_default_layer_key; }

    /// Set default layer key, the indexed geometry must contains the key, otherwith default key is
    /// not modifiend
    void set_default_layer_key( const LayerKeyType& layer_key ) {
        if ( containsLayer( layer_key ) ) m_default_layer_key = layer_key;
    }

  private:
    /// \brief Duplicate attributes stored as pointers
    void deepCopy( const MultiIndexedGeometry& other );

    /// \brief Clear attributes stored as pointers
    void deepClear();

  private:
    /**
     * Collection of pairs <lockStatus, Indices>
     *
     * \note There is no natural ordering for these elements, thus we need an unordered_map. In
     * contrast to map, transparent hashing require c++20, so we need to implement them explicitely
     * here https://en.cppreference.com/w/cpp/container/unordered_map/find
     */
    IndexCollection m_indices;

    /// Default layer key, initialized as invalid, set to first added Layer Key.
    LayerKeyType m_default_layer_key { { InvalidIndexLayer::staticSemanticName }, "invalid" };
};

/**
 * \brief A single layer MultiIndexedGeometry.
 *
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
    using DefaultLayerType   = T;
    using IndexType          = T::IndexType;
    using IndexContainerType = T::IndexContainerType;

  public:
    IndexedGeometry();

    const IndexContainerType& getIndices() const;
    /// read write access to indices.
    /// Cause indices to be "lock" for the caller
    /// need to be unlock by the caller before any one can ask for write access.
    IndexContainerType& getIndicesWithLock();

    /// unlock previously read write acces, notify observers of the update.
    void indicesUnlock();
    /// set indices. Indices must be unlock, i.e. no one should have write
    /// access to it.
    /// Notify observers of the update.
    void setIndices( IndexContainerType&& indices );
    void setIndices( const IndexContainerType& indices );
    const LayerKeyType& getLayerKey() const;
};

class RA_CORE_API IndexedPointCloud : public IndexedGeometry<PointCloudIndexLayer>
{};

using TriangleMesh = IndexedGeometry<TriangleIndexLayer>;

//-----------------------------------------------------------------------------
//- Implementation ------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//- GeometryIndexLayerBase ----------------------------------------------------
inline GeometryIndexLayerBase::GeometryIndexLayerBase( const GeometryIndexLayerBase& other ) :
    ObjectWithSemantic( other.semantics() ) {}

inline GeometryIndexLayerBase&
GeometryIndexLayerBase::operator=( const GeometryIndexLayerBase& other ) {
    CORE_UNUSED( other );
    CORE_ASSERT( semantics() == other.semantics(),
                 "Try to assign object with different semantics" );
    return *this;
}

inline GeometryIndexLayerBase& GeometryIndexLayerBase::operator=( GeometryIndexLayerBase&& other ) {
    CORE_UNUSED( other );
    CORE_ASSERT( semantics() == other.semantics(),
                 "Try to assign GeometryIndexLayer of different type" );
    return *this;
}

//-----------------------------------------------------------------------------
//- GeometryIndexLayer --------------------------------------------------------
template <typename T>
bool GeometryIndexLayer<T>::append( const GeometryIndexLayerBase& other, int offset ) {
    if ( shareSemantic( other ) ) {
        const auto& othercasted = static_cast<const GeometryIndexLayer<T>&>( other );

        std::transform( othercasted.collection().cbegin(),
                        othercasted.collection().cend(),
                        std::back_inserter( m_collection ),
                        [offset]( const T& indice ) -> T { return indice.array() + offset; } );
        return true;
    }
    return false;
}

template <typename T>
void GeometryIndexLayer<T>::offset( int offset, uint start_index ) {
    std::transform( m_collection.cbegin() + start_index,
                    m_collection.cend(),
                    m_collection.begin() + start_index,
                    [offset]( const T& indice ) -> T { return indice.array() + offset; } );
}

template <typename T>
bool GeometryIndexLayer<T>::operator==( const GeometryIndexLayerBase& other ) const {
    if ( shareSemantic( other ) ) {
        const auto& othercasted = static_cast<const GeometryIndexLayer<T>&>( other );
        return othercasted.collection() == m_collection;
    }
    return false;
}

template <typename T>
size_t GeometryIndexLayer<T>::getNumberOfComponents() const {
    return IndexType::RowsAtCompileTime;
}

template <typename T>
size_t GeometryIndexLayer<T>::getBufferSize() const {
    return m_collection.size() * sizeof( IndexType );
}

template <typename T>
int GeometryIndexLayer<T>::getStride() const {
    return sizeof( IndexType );
}

template <typename T>
const void* GeometryIndexLayer<T>::dataPtr() const {
    return m_collection.data();
}

template <typename T>
std::unique_ptr<GeometryIndexLayerBase> GeometryIndexLayer<T>::clone() {
    auto copy          = std::make_unique<GeometryIndexLayer<T>>( *this );
    copy->m_collection = m_collection;
    return copy;
}

//-----------------------------------------------------------------------------
//- MultiIndexedGeometry ------------------------------------------------------
inline MultiIndexedGeometry::~MultiIndexedGeometry() {
    detachAll();
    clear();
}

inline bool
MultiIndexedGeometry::containsLayer( const MultiIndexedGeometry::LayerKeyType& layerKey ) const {
    return m_indices.find( layerKey ) != m_indices.end();
}

inline bool
MultiIndexedGeometry::containsLayer( const MultiIndexedGeometry::LayerSemanticCollection& semantics,
                                     const std::string& layerName ) const {
    return containsLayer( { semantics, layerName } );
}

inline size_t
MultiIndexedGeometry::countLayers( const MultiIndexedGeometry::LayerKeyType& layerKey ) const {
    return m_indices.count( layerKey );
}

inline size_t
MultiIndexedGeometry::countLayers( const MultiIndexedGeometry::LayerSemanticCollection& semantics,
                                   const std::string& layerName ) const {
    return countLayers( { semantics, layerName } );
}

inline const GeometryIndexLayerBase&
MultiIndexedGeometry::getLayer( const MultiIndexedGeometry::LayerKeyType& layerKey ) const {
    return *( m_indices.at( layerKey ).second.get() );
}

inline GeometryIndexLayerBase& MultiIndexedGeometry::getLayerWithLock(
    const MultiIndexedGeometry::LayerSemanticCollection& semantics,
    const std::string& layerName ) {
    return getLayerWithLock( { semantics, layerName } );
}

inline void
MultiIndexedGeometry::unlockLayer( const MultiIndexedGeometry::LayerSemanticCollection& semantics,
                                   const std::string& layerName ) {
    unlockLayer( { semantics, layerName } );
}

inline auto MultiIndexedGeometry::layerKeys() const {
    return Utils::map_keys( m_indices );
}

//-----------------------------------------------------------------------------
//- IndexedGeometry -----------------------------------------------------------
template <typename T>
IndexedGeometry<T>::IndexedGeometry() {
    auto layer = std::make_unique<DefaultLayerType>();
    auto added = addLayer( std::move( layer ) );
    if ( added.first ) { set_default_layer_key( added.second ); }
}

template <typename T>
const typename IndexedGeometry<T>::IndexContainerType& IndexedGeometry<T>::getIndices() const {
    return indices<T>();
}

template <typename T>
typename IndexedGeometry<T>::IndexContainerType& IndexedGeometry<T>::getIndicesWithLock() {
    return indices_with_lock<T>().second;
}

template <typename T>
void IndexedGeometry<T>::indicesUnlock() {
    unlockLayer( default_layer_key() );
}

template <typename T>
void IndexedGeometry<T>::setIndices( IndexContainerType&& indices ) {
    set_indices<T>( default_layer_key(), std::move( indices ) );
}

template <typename T>
void IndexedGeometry<T>::setIndices( const IndexContainerType& indices ) {
    set_indices<T>( default_layer_key(), indices );
}

template <typename T>
const typename IndexedGeometry<T>::LayerKeyType& IndexedGeometry<T>::getLayerKey() const {
    return default_layer_key();
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
