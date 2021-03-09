#pragma once

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/Utils/ObjectWithSemantic.hpp>
#include <map>

namespace Ra {
namespace Core {
namespace Geometry {

///
/// \brief Base class for index collections stored in MultiIndexedGeometry
class RA_CORE_API IndexViewBase : public Utils::ObservableVoid, public Utils::ObjectWithSemantic
{
  public:
    inline explicit IndexViewBase( const IndexViewBase& other ) :
        ObjectWithSemantic( other.semantics() ) {}

    // inline IndexViewBase( IndexViewBase&& other ) : x§ObjectWithSemantic( other ) {}
    inline IndexViewBase& operator=( const IndexViewBase& other ) {
        CORE_UNUSED( other );
        CORE_ASSERT( semantics() == other.semantics(),
                     "Try to assign object with different semantics" );
        return *this;
    }
    inline IndexViewBase& operator=( IndexViewBase&& other ) {
        CORE_UNUSED( other );
        CORE_ASSERT( semantics() == other.semantics(),
                     "Try to assign IndexView of different type" );
        return *this;
    }

  protected:
    template <class... SemanticNames>
    inline IndexViewBase( SemanticNames... names ) : ObjectWithSemantic( names... ) {}
};

/// \brief Typed index collection
template <typename T>
struct IndexView : public IndexViewBase {
    using IndexType          = T;
    using IndexContainerType = VectorArray<IndexType>;

    inline IndexContainerType& collection() { return _collection; };
    const IndexContainerType& collection() const { return _collection; };

  protected:
    template <class... SemanticNames>
    inline IndexView( SemanticNames... names ) : IndexViewBase( names... ) {}

  private:
    IndexContainerType _collection;
};

/// Simple Mesh structure that handles indexed geometry with vertex
/// attributes. Each face is index collection is stored as IndexView.
class RA_CORE_API MultiIndexedGeometry : public AttribArrayGeometry, public Utils::ObservableVoid
{
  public:
    using IndicesSemanticCollection = Utils::ObjectWithSemantic::SemanticNameCollection;
    using IndicesSemantic           = Utils::ObjectWithSemantic::SemanticName;

    inline MultiIndexedGeometry() = default;
    explicit MultiIndexedGeometry( const MultiIndexedGeometry& other );
    explicit MultiIndexedGeometry( MultiIndexedGeometry&& other );
    explicit MultiIndexedGeometry( const AttribArrayGeometry& other );
    explicit MultiIndexedGeometry( AttribArrayGeometry&& other );
    MultiIndexedGeometry& operator=( const MultiIndexedGeometry& other );
    MultiIndexedGeometry& operator=( MultiIndexedGeometry&& other );

    void clear() override;

    /// Copy only the geometry and the indices from \p other, but not the attributes.
    void copy( const MultiIndexedGeometry& other );

    /// Check that the MultiIndexedGeometry is well built, asserting when it is not.
    /// only compiles to something when in debug mode.
    void checkConsistency() const;

    // /// Appends another MultiIndexedGeometry to this one, but only if they have the same
    // attributes.
    // /// Return True if \p other has been successfully appended.
    // /// \warning There is no error check on the handles attribute type.
    // inline bool append( const MultiIndexedGeometry& other );

    // /// search if at least one index exists with this Semantic
    // bool indicesExists( const IndicesSemantic& semanticName ) const;

    /// search if at least one index exists with this Semantic
    bool indicesExists( const IndicesSemanticCollection& semantics ) const;

    // /// read only access to indices
    // /// \throws std::out_of_range
    // const IndexViewBase& getIndices( const IndicesSemantic& semanticName ) const;

    /// read only access to indices
    /// \throws std::out_of_range
    const IndexViewBase& getIndices( const IndicesSemanticCollection& semantics ) const;

    // /// read write access to indices.
    // /// Cause indices to be "lock" for the caller
    // /// need to be unlock by the caller before any one can ask for write access.
    // /// \throws std::out_of_range
    // IndexViewBase& getIndicesWithLock( const IndicesSemantic& name );

    /// read write access to indices.
    /// Cause indices to be "lock" for the caller
    /// need to be unlock by the caller before any one can ask for write access.
    /// \throws std::out_of_range
    IndexViewBase& getIndicesWithLock( const IndicesSemanticCollection& name );

    // /// unlock previously read write acces, notify observers of the update.
    // /// \throws std::out_of_range
    // void indicesUnlock( const IndicesSemantic& name );

    /// unlock previously read write acces, notify observers of the update.
    /// \throws std::out_of_range
    void indicesUnlock( const IndicesSemanticCollection& name );

    // /// set indices. Indices must be unlock, i.e. no one should have write
    // /// access to it.
    // /// Notify observers of the update.
    // commented: does move operator work with inheritance ?
    // void setIndices( IndexViewBase&& indices );
    void setIndices( const IndexViewBase& indices );

  private:
    // Collection of pairs <lockStatus, Indices>
    std::map<IndicesSemanticCollection, std::pair<bool, IndexViewBase>> m_indices;
};

struct RA_CORE_API PointCloudIndexView : public IndexView<Vector1ui> {
    inline PointCloudIndexView() : IndexView( "IndexPointCloud" ) {};
};

struct RA_CORE_API TriangleIndexView : public IndexView<Vector3ui> {
    inline TriangleIndexView() : IndexView( "TriangleMesh" ) {};
};

// class RA_CORE_API PolyIndexView : public IndexViewCollectionHelper<VectorNui>
// {};

// class RA_CORE_API LineIndexView : public IndexViewCollectionHelper<Vector2ui>
// {};

} // namespace Geometry
} // namespace Core
} // namespace Ra
