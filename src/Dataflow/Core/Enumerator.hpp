#pragma once

#include <initializer_list>
#include <vector>

#include <Core/Utils/Observable.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

/**
 * \brief This class might be removed in a future and replaced by an instantiation of
 * Ra::Core::Utils::BijectiveAssociation.
 *
 * This class allows to associate Values of type T to an int.
 * Used right now to build the Node edition UI.
 * \tparam T
 */
template <typename T>
class Enumerator : public Ra::Core::Utils::Observable<const Enumerator<T>&>
{
    std::vector<T> m_values;
    size_t m_currentIndex { 0 };
    T* m_currentValue;

  public:
    explicit Enumerator( std::initializer_list<T> values );
    const T& get() const;
    size_t size() const;
    bool set( size_t p );
    bool set( const T& v );
    typename std::vector<T>::const_iterator begin() const;
    typename std::vector<T>::const_iterator end() const;
    const T& operator[]( size_t p ) const;
};

} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/Enumerator.inl>
