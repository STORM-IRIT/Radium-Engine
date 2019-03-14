#ifndef INDEXMAP_HPP
#define INDEXMAP_HPP

#include <Core/RaCore.hpp>

#include <algorithm>
#include <assert.h>
#include <deque>

#include <Core/Utils/Index.hpp>

namespace Ra {
namespace Core {
namespace Utils {

/**
 * The class IndexMap define a map where a object is coupled with a index.
 * The index is unique, it is assigned to a object when it's inserted and is kept
 * until the object is removed. After a removal, the index becomes free again.
 * An object will be given the first free index available. If no free indices are
 * available, the object will not be inserted and the IndexMap is considered full.
 */
template <typename T>
class IndexMap {
  public:
    /// \name Storage typedefs
    /// \{

    /**
     * Type for objects storage.
     */
    using Container = typename std::deque<T>;

    /**
     * Type for iterators on the objects storage.
     */
    using Iterator = typename Container::iterator;

    /**
     * Type for const iterators on the objects storage.
     */
    using ConstIterator = typename Container::const_iterator;

    /**
     * Type for Indices storage.
     */
    using IndexContainer = typename std::deque<Index>;

    /**
     * Type for const iterators on the Indices storage.
     */
    using ConstIndexIterator = typename IndexContainer::const_iterator;
    /// \}

    inline IndexMap();

    inline IndexMap( const IndexMap& id_map );

    inline ~IndexMap();

    /// \name Object management
    /// \{

    /**
     * Insert a object in the IndexMap. Return an invalid index if the object is not inserted.
     */
    inline Index insert( const T& obj );

    /**
     * Construct an object in place in the IndexMap. Return an invalid index if
     * the object is not inserted.
     */
    template <typename... Args>
    inline Index emplace( const Args&&... args );

    /**
     * Remove the object with the given index. Return false if the operation failed.
     */
    inline bool remove( const Index& idx );

    /**
     * Clear the IndexMap.
     */
    inline void clear();
    /// \}

    /// \name Object access
    /// \{

    /**
     * Return a read-only reference to object with the given index.
     * \warning \p idx must exist in the map.
     */
    inline const T& at( const Index& idx ) const;

    /**
     * Return a reference to the object with the given index.
     * \warning \p idx must exist in the map.
     */
    inline T& access( const Index& idx );

    /**
     * Return a reference to the object with given index.
     * \warning \p idx must exist in the map.
     */
    inline T& operator[]( const Index& idx );

    /**
     * Return a const reference to the object with given index.
     * \warning \p idx must exist in the map.
     */
    inline const T& operator[]( const Index& idx ) const;
    /// \}

    /// \name Status queries
    /// \{

    /**
     * Return the size of the IndexMap ( number of object contained ).
     */
    inline size_t size() const;

    /**
     * Return true if the IndexMap is empty, false otherwise.
     */
    inline bool empty() const;

    /**
     * Return true if the IndexMap cannot contain more objects, false otherwise.
     */
    inline bool full() const;

    /**
     * Return true if the IndexMap contains a object with the given index, false otherwise.
     */
    inline bool contains( const Index& idx ) const;

    /**
     * Return the i-th index.
     * \note Return an invalid index if i is out of bound.
     */
    inline Index index( const uint i ) const;
    /// \}

    /// \name Iterator interface
    /// \{

    /**
     * Return an iterator to the first object in the IndexMap.
     */
    inline Iterator begin();

    /**
     * Return an iterator to the end of the object list in the IndexMap.
     */
    inline Iterator end();

    /**
     * Return an iterator to the first object in the IndexMap.
     */
    inline ConstIterator begin() const;

    /**
     * Return an iterator to the end of the object list in the IndexMap.
     */
    inline ConstIterator end() const;

    /**
     * Return a const iterator to the first object in the IndexMap.
     */
    inline ConstIterator cbegin() const;

    /**
     * Return a const iterator to the end of the object list in the IndexMap.
     */
    inline ConstIterator cend() const;

    /**
     * Return a const iterator to the first index in the IndexMap.
     */
    inline ConstIndexIterator cbegin_index() const;

    /**
     * Return a const iterator to the end of the indices of the IndexMap.
     */
    inline ConstIndexIterator cend_index() const;
    /// \}

  protected:
    Container m_data;       ///< Objects in the IndexMap.
    IndexContainer m_index; ///< Indices in the IndexMap.

  private:
    /**
     * Push a new free index in free list.
     */
    inline void push_free_index( const Index& idx );

    /**
     * Pop a free index from the free list.
     */
    inline bool pop_free_index( Index& idx );

    /** \name Helper functions
     *  These function return an iterator to an object from an interator to an index.
     */
    /// \{

    /**
     * Returns a const iterator on an object from its intex iterator.
     */
    inline ConstIterator citfromIndex( const ConstIndexIterator& it ) const;

    /**
     * Returns a non-const iterator on an object from its intex iterator.
     */
    inline Iterator itfromIndex( const ConstIndexIterator& it );

    /**
     * Return the index of the object in the map from its index iterator.
     */
    inline size_t idxfromIndex( const ConstIndexIterator& it ) const;
    /// \}

  private:
    IndexContainer m_free; ///< List of available free indices.
};

} // namespace Utils
} // namespace Core
} // namespace Ra

#include <Core/Utils/IndexMap.inl>

#endif // INDEXMAP_HPP
