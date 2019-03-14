#ifndef RADIUMENGINE_INDEXEDOBJECT_HPP
#define RADIUMENGINE_INDEXEDOBJECT_HPP

#include <Core/RaCore.hpp>
#include <Core/Utils/Index.hpp>

namespace Ra {
namespace Core {
namespace Utils {

/**
 * An IndexedObject is an object identifiable through it's Index.
 */
class IndexedObject {
  public:
    explicit inline IndexedObject( Index idx = Index::Invalid() ) : m_idx{idx} {}

    inline IndexedObject( const IndexedObject& id_obj ) = default;

    virtual inline ~IndexedObject() = default;

    /**
     * Set the object's Index.
     */
    inline void setIndex( const Index& idx ) { m_idx = idx; }

    /**
     * Return the object's Index.
     */
    inline const Index& getIndex() const { return m_idx; }

  protected:
    /// The index of the object.
    Index m_idx;
};

} // namespace Utils
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_INDEXEDOBJECT_HPP
