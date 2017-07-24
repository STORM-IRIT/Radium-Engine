#ifndef INDEXMAP_HPP
#define INDEXMAP_HPP

#include <Core/RaCore.hpp>
#include <deque>
#include <algorithm>
#include <assert.h>

#include <Core/Index/Index.hpp>

namespace Ra {
namespace Core {

/*
* The class IndexMap define a map where a object is coupled with a index.
* The index is unique, it is assigned to a object when it's inserted and is kept until the object is removed.
* After a removal, the index becomes free again.
* A object will be given the first free index available.
* If no free indices are available, the object will not be inserted and the IndexMap is considered full.
*/
template <typename T>
class IndexMap {
public:
    /// ===============================================================================
    /// TYPEDEF
    /// ===============================================================================
    typedef typename std::deque< Index >::const_iterator ConstIndexIterator;    // Const iterator to the list of indices of the IndexMap.
    typedef typename std::deque< T >::iterator           Iterator;              // Iterator to the list of objects of the IndexMap.
    typedef typename std::deque< T >::const_iterator     ConstIterator;         // Const iterator to the list of objects of the IndexMap.

    /// ===============================================================================
    /// CONSTRUCTOR
    /// ===============================================================================
    inline IndexMap();                                      // Default constructor.
    inline IndexMap( const IndexMap& id_map );              // Copy constructor.

    /// ===============================================================================
    /// DESTRUCTOR
    /// ===============================================================================
    inline ~IndexMap();                                     // Destructor.

    /// ===============================================================================
    /// INSERT
    /// ===============================================================================
    inline Index insert( const T& obj );                    // Insert a object in the IndexMap. Return an invalid index if the object is not inserted.
    inline bool  insert( const T& obj, Index& idx );        // Insert a object in the IndexMap. Return true if the object is inserted, and store it in idx.

    template<typename... Args>
    inline Index emplace( const Args&&... args );             // Construct an object in place in the IndexMap. Return an invalid index if the object is not inserted.
    template<typename... Args>
    inline bool  emplace(  Index& idx, const Args&&... args ); // Construct an object in place in the IndexMap. Return true if the object is inserted, and store it in idx.

    /// ===============================================================================
    /// REMOVE
    /// ===============================================================================
    inline bool  remove( const Index& idx );                // Remove the object with the given index. Return false if the operation failed.

    /// ===============================================================================
    /// ACCESS
    /// ===============================================================================
    inline const T&  at( const Index& idx ) const;          // Return the object with the given index. Crash if the index is not present.
    inline bool  at( const Index& idx, T& obj ) const;      // Return the object with the given index. Return false if the index is not present, true otherwise.

    inline T&    access( const Index& idx );                // Return a reference to the object with the given index. Crash if index is not present.
    inline bool  access( const Index& idx, T& obj );        // Return a reference to the object with the given index. Return false if the index is not present, true otherwise.

    /// ===============================================================================
    /// SIZE
    /// ===============================================================================
    inline size_t size() const;                              // Return the size of the IndexMap ( number of object contained ).
    inline void  clear();                                   // Clear the IndexMap.

    /// ===============================================================================
    /// QUERY
    /// ===============================================================================
    inline bool  empty() const;                             // Return true if the IndexMap is empty.
    inline bool  full()  const;                             // Return true if the IndexMap cannot contain more objects.
    inline bool  contains( const Index& idx ) const;         // Return true if the IndexMap contains a object with the given index.
    inline bool  compact() const;                           // Return true if the indices in the map are all consecutive.
    inline Index index( const uint i ) const;               // Return the i-th index. Return an invalid index if i is out of bound.

    /// ===============================================================================
    /// OPERATOR
    /// ===============================================================================
    inline T&         operator[]( const Index& idx );       // Return the reference to the object with given index. Equal to using access( idx ).
    inline const T&   operator[]( const Index& idx ) const; // Return the reference to the object with given index. Equal to using access( idx ).

    /// ===============================================================================
    /// INDEX ITERATOR
    /// ===============================================================================
    inline ConstIndexIterator cbegin_index() const;         // Return a const iterator to the first index in the IndexMap.
    inline ConstIndexIterator   cend_index() const;         // Return a const iterator to the end of the indices of the IndexMap.

    /// ===============================================================================
    /// DATA ITERATOR
    /// ===============================================================================
    inline      Iterator  begin();                          // Return a iterator to the first object in the IndexMap.
    inline      Iterator    end();                          // Return a iterator to the end of the object list in the IndexMap.
    inline ConstIterator  begin() const;                    // Return a iterator to the first object in the IndexMap.
    inline ConstIterator    end() const;                    // Return a iterator to the end of the object list in the IndexMap.
    inline ConstIterator cbegin() const;                    // Return a const iterator to the first object in the IndexMap.
    inline ConstIterator   cend() const;                    // Return a const iterator to the end of the object list in the IndexMap.

protected:
    /// ===============================================================================
    /// VARIABLE
    /// ===============================================================================
    std::deque< T >     m_data;                             // Objects in the IndexMap
    std::deque< Index > m_index;                            // Indices in the IndexMap

private:
    /// ===============================================================================
    /// PUSH
    /// ===============================================================================
    inline void push_free_index( const Index& idx );        // Push a new free index in the list of the free indices.

    /// ===============================================================================
    /// POP
    /// ===============================================================================
    inline bool pop_free_index( Index& idx );               // Pop a free index from the list of free indices. Return false if no free indices are available.

    /// ===============================================================================
    /// VARIABLE
    /// ===============================================================================
    std::deque< Index > m_free;                             // List of available free indices.
};

} // namespace Core
} // namespace Ra

#include <Core/Index/IndexMap.inl>

#endif // INDEXMAP_HPP
