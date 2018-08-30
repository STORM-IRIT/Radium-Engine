#ifndef INDEXMAP_HPP
#define INDEXMAP_HPP

#include <Core/RaCore.hpp>

#include <algorithm>
#include <assert.h>
#include <deque>

#include <Core/Index/Index.hpp>

namespace Ra {
namespace Core {

/**
 * The class IndexMap define a map where a object is coupled with a index.
 * The index is unique, it is assigned to a object when it's inserted and is kept until the object
 * is removed. After a removal, the index becomes free again. A object will be given the first free
 * index available. If no free indices are available, the object will not be inserted and the
 * IndexMap is considered full.
 */
template <typename T>
class IndexMap {
  public:
    /// Where the objects are stored
    using Container = typename std::deque<T>;

    /// Where the indices are stored
    using IndexContainer = typename std::deque<Index>;

    /// Const iterator to the list of indices of the IndexMap.
    using ConstIndexIterator = typename IndexContainer::const_iterator;

    /// Iterator to the list of objects of the IndexMap.
    using Iterator = typename Container::iterator;

    /// Const iterator to the list of objects of the IndexMap.
    using ConstIterator = typename Container::const_iterator;

    // =========================================================================
    // CONSTRUCTOR
    // =========================================================================
    inline IndexMap();
    inline IndexMap( const IndexMap& id_map );

    // =========================================================================
    // DESTRUCTOR
    // =========================================================================
    inline ~IndexMap();

    // =========================================================================
    // INSERT
    // =========================================================================
    /// Insert a object in the IndexMap. Return an invalid index if the object is not inserted.
    inline Index insert( const T& obj );

    /// Construct an object in place in the IndexMap. Return an invalid index if the object is not
    /// inserted.
    template <typename... Args>
    inline Index emplace( const Args&&... args );

    // =========================================================================
    // REMOVE
    // =========================================================================
    /// Remove the object with the given index. Return false if the operation failed.
    inline bool remove( const Index& idx );

    // =========================================================================
    // ACCESS
    // =========================================================================
    /// Return a read-only ref to object with the given index.  Crashes if index does not exist.
    inline const T& at( const Index& idx ) const;

    /// Return a reference to the object with the given index. Crash if index does not exist
    inline T& access( const Index& idx );

    // =========================================================================
    // SIZE
    // =========================================================================
    /// Return the size of the IndexMap ( number of object contained ).
    inline size_t size() const;

    /// Clear the IndexMap.
    inline void clear();

    // =========================================================================
    // QUERY
    // =========================================================================
    /// Return true if the IndexMap is empty.
    inline bool empty() const;

    /// Return true if the IndexMap cannot contain more objects.
    inline bool full() const;

    /// Return true if the IndexMap contains a object with the given index.
    inline bool contains( const Index& idx ) const;

    /// Return the i-th index. Return an invalid index if i is out of bound.
    inline Index index( const uint i ) const;

    // =========================================================================
    // OPERATOR
    // =========================================================================
    /// Return a reference to the object with given index.
    inline T& operator[]( const Index& idx );

    /// Return a const reference to the object with given index.
    inline const T& operator[]( const Index& idx ) const;

    // =========================================================================
    // INDEX ITERATOR
    // =========================================================================
    /// Return a const iterator to the first index in the IndexMap.
    inline ConstIndexIterator cbegin_index() const;

    /// Return a const iterator to the end of the indices of the IndexMap.
    inline ConstIndexIterator cend_index() const;

    // =========================================================================
    // DATA ITERATOR
    // =========================================================================
    /// Return a iterator to the first object in the IndexMap.
    inline Iterator begin();

    /// Return a iterator to the end of the object list in the IndexMap.
    inline Iterator end();

    /// Return a iterator to the first object in the IndexMap.
    inline ConstIterator begin() const;

    /// Return a iterator to the end of the object list in the IndexMap.
    inline ConstIterator end() const;

    /// Return a const iterator to the first object in the IndexMap.
    inline ConstIterator cbegin() const;

    /// Return a const iterator to the end of the object list in the IndexMap.
    inline ConstIterator cend() const;

  protected:
    // =========================================================================
    // Member variables
    // =========================================================================
    /// Objects in the IndexMap
    Container m_data;

    /// Indices in the IndexMap
    IndexContainer m_index;

  private:
    // =========================================================================
    // FREE LIST MANAGEMENT
    // =========================================================================
    /// Push a new free index in free list
    inline void push_free_index( const Index& idx );

    /// Pop a free index from the free list
    inline bool pop_free_index( Index& idx );

    // =========================================================================
    // HELPER FUNCTIONS
    // =========================================================================
    // These function return an iterator to an object from an interator in an index.
    /// Returns a const iterator on an object from its intex iterator
    inline ConstIterator citfromIndex( const ConstIndexIterator& it ) const;

    /// Returns a non-const iterator on an object from its intex iterator
    inline Iterator itfromIndex( const ConstIndexIterator& it );

    /// Return the index of the object in the map from its index iterator
    inline size_t idxfromIndex( const ConstIndexIterator& it ) const;

  private:
    // =========================================================================
    // Member variables
    // =========================================================================
    /// List of available free indices.
    IndexContainer m_free;
};

} // namespace Core
} // namespace Ra

#include <Core/Index/IndexMap.inl>

#endif // INDEXMAP_HPP
