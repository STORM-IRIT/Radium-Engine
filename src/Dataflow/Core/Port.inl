#pragma once
#include <Core/Utils/TypesUtils.hpp>
#include <Dataflow/Core/Port.hpp>

#include <Dataflow/Core/TypeDemangler.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

inline PortBase::PortBase( const std::string& name, size_t type, Node* node ) :
    m_name( name ), m_type( type ), m_node( node ) {}

inline const std::string& PortBase::getName() {
    return m_name;
}

inline size_t PortBase::getType() {
    return m_type;
}

inline Node* PortBase::getNode() {
    return m_node;
}

inline bool PortBase::hasData() {
    return false;
}

inline bool PortBase::isLinked() {
    return m_isLinked;
}

inline bool PortBase::isLinkMandatory() {
    return m_isLinkMandatory;
}

inline void PortBase::mustBeLinked() {
    m_isLinkMandatory = true;
}

inline bool PortBase::accept( PortBase* other ) {
    return m_type == other->getType();
}

template <typename T>
PortOut<T>::PortOut( const std::string& name, Node* node ) :
    PortBase( name, typeid( T ).hash_code(), node ) {}

template <typename T>
T& PortOut<T>::getData() {
    return *m_data;
}

template <typename T>
void PortOut<T>::setData( T* data ) {
    m_data = data;
}

template <typename T>
bool PortOut<T>::hasData() {
    return m_data != nullptr;
}

template <typename T>
PortBase* PortOut<T>::getLink() {
    return nullptr;
}

template <typename T>
bool PortOut<T>::accept( PortBase* ) {
    return false;
}

template <typename T>
bool PortOut<T>::connect( PortBase* o ) {
    m_isLinked = o->connect( this );
    return m_isLinked;
}

template <typename T>
std::string PortOut<T>::getTypeName() {
    // return Ra::Core::Utils::demangleType<T>();
    return simplifiedDemangledType<T>();
}

template <typename T>
std::string PortIn<T>::getTypeName() {
    // return Ra::Core::Utils::demangleType<T>();
    return simplifiedDemangledType<T>();
}

template <typename T>
void PortBase::setData( T* data ) {
    if ( !is_input() ) {
        auto thisOut = dynamic_cast<PortOut<T>*>( this );
        if ( thisOut ) {
            thisOut->setData( data );
            return;
        }
    }
#ifdef GRAPH_CALL_TRACE
    if ( is_input() ) {
        std::cout << "\e[41m\e[1mError, can't set data on the input port " << this->getName()
                  << "\e[0m" << std::endl;
    }
    else {
        std::cout << "\e[41m\e[1mError, can't set data on the port of incompatible data type."
                  << this->getName() << "\e[0m" << std::endl;
    }
#endif
}

template <typename T>
void PortBase::getData( T& t ) {
    if ( !is_input() ) {
        auto thisOut = dynamic_cast<PortOut<T>*>( this );
        if ( thisOut ) { t = thisOut->getData(); }
    }
#ifdef GRAPH_CALL_TRACE
    if ( is_input() ) {
        std::cout << "\e[41m\e[1mError, can't get data on the input port " << this->getName()
                  << "\e[0m" << std::endl;
    }
    else {
        std::cout << "\e[41m\e[1mError, can't get data on the port of incompatible data type "
                  << this->getName() << " (" << this->getTypeName() << " vs "
                  << RadiumAddons::Core::Utils::demang<T>() << ")\e[0m" << std::endl;
    }
#endif
}

template <typename T>
T& PortBase::getData() {
    if ( !is_input() ) {
        auto thisOut = dynamic_cast<PortOut<T>*>( this );
        if ( thisOut ) { return thisOut->getData(); }
    }
    std::cerr << "Could not call T& PortBase::getData() on an input port !!!\n";
    std::abort();
}

template <typename T>
bool PortOut<T>::disconnect() {
    return false;
}

template <typename T>
PortBase* PortOut<T>::reflect( Node* node, std::string name ) {
    return new PortIn<DataType>( name, node );
}

/**
 * PortIn is an Observable<PortIn<T>&> that notifies its observers at connect/disconnect event
 * @tparam T
 */
template <typename T>
PortIn<T>::PortIn( const std::string& name, Node* node ) :
    PortBase( name, typeid( T ).hash_code(), node ) {}

template <typename T>
PortBase* PortIn<T>::getLink() {
    return m_from;
}

template <typename T>
T& PortIn<T>::getData() {
    return m_from->getData();
}

template <typename T>
bool PortIn<T>::accept( PortBase* other ) {
    if ( !m_from && ( other->getType() == getType() ) ) { return PortBase::accept( other ); }
    return false;
}

template <typename T>
bool PortIn<T>::connect( PortBase* other ) {
    if ( accept( other ) ) {
        m_from     = dynamic_cast<PortOut<T>*>( other );
        m_isLinked = true;
        // notify after connect
        this->notify( getName(), *this, true );
    }
    return m_isLinked;
}

template <typename T>
bool PortIn<T>::disconnect() {
    if ( m_isLinked ) {
        // notify before disconnect
        this->notify( getName(), *this, false );
        m_from     = nullptr;
        m_isLinked = false;
        return true;
    }
    return false;
}
template <typename T>
PortBase* PortIn<T>::reflect( Node* node, std::string name ) {
    return new PortOut<DataType>( name, node );
}

template <typename T>
bool PortIn<T>::is_input() {
    return true;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
