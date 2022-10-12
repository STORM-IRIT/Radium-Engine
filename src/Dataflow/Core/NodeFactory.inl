#pragma once
#include <Dataflow/Core/NodeFactory.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

template <typename T>
bool NodeFactory::registerNodeCreator( NodeCreatorFunctor nodeCreator,
                                       const std::string& nodeCategory ) {
    return registerNodeCreator( T::getTypename(), std::move( nodeCreator ), nodeCategory );
}

template <typename T>
bool NodeFactory::registerNodeCreator( const std::string& instanceNamePrefix,
                                       const std::string& nodeCategory ) {
    return registerNodeCreator(
        T::getTypename(),
        [this, instanceNamePrefix]( const nlohmann::json& data ) {
            auto node = new T( instanceNamePrefix + std::to_string( this->nextNodeId() ) );
            node->fromJson( data );
            return node;
        },
        nodeCategory );
}

inline const NodeFactory::ContainerType& NodeFactory::getFactoryMap() const {
    return m_nodesCreators;
}

inline bool NodeFactorySet::addFactory( NodeFactorySet::key_type factoryname,
                                        NodeFactorySet::mapped_type factory ) {
    const auto [loc, inserted] = insert( { std::move( factoryname ), std::move( factory ) } );
    return inserted;
}

inline Ra::Core::Utils::optional<NodeFactorySet::mapped_type>
NodeFactorySet::hasFactory( const NodeFactorySet::key_type& factoryname ) {
    auto f = m_factories.find( factoryname );
    if ( f != m_factories.end() ) { return f->second; }
    else {
        return {};
    }
}

inline bool NodeFactorySet::removeFactory( const NodeFactorySet::key_type& factoryname ) {
    return erase( factoryname );
}
inline NodeFactorySet::const_iterator NodeFactorySet::begin() const {
    return m_factories.begin();
}
inline NodeFactorySet::const_iterator NodeFactorySet::end() const {
    return m_factories.end();
}
inline NodeFactorySet::const_iterator NodeFactorySet::cbegin() const {
    return m_factories.cbegin();
}
inline NodeFactorySet::const_iterator NodeFactorySet::cend() const {
    return m_factories.cend();
}
inline NodeFactorySet::const_iterator
NodeFactorySet::find( const NodeFactorySet::key_type& key ) const {
    return m_factories.find( key );
}
inline std::pair<NodeFactorySet::iterator, bool>
NodeFactorySet::insert( NodeFactorySet::value_type value ) {
    return m_factories.insert( std::move( value ) );
}
inline size_t NodeFactorySet::erase( const NodeFactorySet::key_type& key ) {
    return m_factories.erase( key );
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
