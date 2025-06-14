#include <Dataflow/Core/NodeFactory.hpp>

#include <Core/Utils/Log.hpp>
#include <Dataflow/Core/DataflowGraph.hpp>
#include <iostream>
#include <memory>

namespace Ra {
namespace Dataflow {
namespace Core {
class Node;

NodeFactory::NodeFactory( std::string name ) : m_name( std::move( name ) ) {}

auto NodeFactory::name() const -> std::string {
    return m_name;
}

auto NodeFactory::create_node( const std::string& nodeType,
                               const nlohmann::json& data,
                               DataflowGraph* owningGraph ) -> std::shared_ptr<Node> {
    if ( auto itr = m_nodesCreators.find( nodeType ); itr != m_nodesCreators.end() ) {
        auto node = std::shared_ptr<Node> { itr->second.first( data ) };
        if ( owningGraph != nullptr ) { owningGraph->add_node( node ); }
        return node;
    }
    return nullptr;
}

auto NodeFactory::register_node_creator( const std::string& nodeType,
                                         NodeCreatorFunctor nodeCreator,
                                         const std::string& nodeCategory ) -> bool {

    if ( auto itr = m_nodesCreators.find( nodeType ); itr == m_nodesCreators.end() ) {
        m_nodesCreators[nodeType] = { std::move( nodeCreator ), nodeCategory };
        return true;
    }
    LOG( Ra::Core::Utils::logWARNING )
        << "NodeFactory (" << name()
        << ") : trying to add an already existing node creator for type " << nodeType << ".";
    return false;
}

auto NodeFactory::next_node_id() -> size_t {
    return ++m_nodesCreated;
}

auto NodeFactorySet::create_node( const std::string& nodeType,
                                  const nlohmann::json& data,
                                  DataflowGraph* owningGraph ) -> std::shared_ptr<Node> {
    for ( const auto& itr : m_factories ) {
        if ( auto node = itr.second->create_node( nodeType, data, owningGraph ); node ) {
            return node;
        }
    }
    LOG( Ra::Core::Utils::logERROR ) << "NodeFactorySet: unable to find constructor for "
                                     << nodeType << " in any managed factory.";
    return nullptr;
}

namespace NodeFactoriesManager {

/**
 * \brief Allow static initialization without init order problems
 * \return The manager singleton
 */
auto factory_manager() -> NodeFactorySet& {
    static NodeFactorySet s_factoryManager {};
    return s_factoryManager;
}

auto register_factory( NodeFactorySet::mapped_type factory ) -> bool {
    return factory_manager().add_factory( std::move( factory ) );
}

auto create_factory( const NodeFactorySet::key_type& name ) -> NodeFactorySet::mapped_type {
    auto f = factory( name );
    if ( f == nullptr ) {
        f = std::make_shared<NodeFactory>( name );
        register_factory( f );
    }
    return f;
}

auto factory( const NodeFactorySet::key_type& name ) -> NodeFactorySet::mapped_type {
    auto& factories = factory_manager();
    if ( auto factory = factories.find( name ); factory != factories.end() ) {
        return factory->second;
    }
    return nullptr;
}

auto unregister_factory( const NodeFactorySet::key_type& name ) -> bool {
    return factory_manager().remove_factory( name );
}

auto default_factory() -> NodeFactorySet::mapped_type {
    return factory( factory_manager().default_factory_name() );
}

} // namespace NodeFactoriesManager

} // namespace Core
} // namespace Dataflow
} // namespace Ra
