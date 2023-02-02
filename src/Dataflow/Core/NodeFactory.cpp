#include <Dataflow/Core/NodeFactory.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>

#include <Dataflow/Core/Nodes/CoreBuiltInsNodes.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

namespace NodeFactoriesManager {
const std::string dataFlowBuiltInsFactoryName { "DataFlowBuiltIns" };
}

NodeFactory::NodeFactory( std::string name ) : m_name( std::move( name ) ) {}

std::string NodeFactory::getName() const {
    return m_name;
}

Node* NodeFactory::createNode( std::string& nodeType,
                               const nlohmann::json& data,
                               DataflowGraph* owningGraph ) {
    auto it = m_nodesCreators.find( nodeType );
    if ( it != m_nodesCreators.end() ) {
        auto node = it->second.first( data );
        if ( owningGraph != nullptr ) {
            auto [added, n] = owningGraph->addNode( std::unique_ptr<Node>( node ) );
            return n;
        }
        return node;
    }
    return nullptr;
}

bool NodeFactory::registerNodeCreator( std::string nodeType,
                                       NodeCreatorFunctor nodeCreator,
                                       const std::string& nodeCategory ) {
    auto it = m_nodesCreators.find( nodeType );
    if ( it == m_nodesCreators.end() ) {
        m_nodesCreators[nodeType] = { std::move( nodeCreator ), nodeCategory };
        return true;
    }
    LOG( Ra::Core::Utils::logWARNING )
        << "NodeFactory (" << getName()
        << ") : trying to add an already existing node creator for type " << nodeType << ".";
    return false;
}

size_t NodeFactory::nextNodeId() {
    return ++m_nodesCreated;
}

Node* NodeFactorySet::createNode( std::string& nodeType,
                                  const nlohmann::json& data,
                                  DataflowGraph* owningGraph ) {
    for ( const auto& it : m_factories ) {
        auto node = it.second->createNode( nodeType, data, owningGraph );
        if ( node ) { return node; }
    }
    LOG( Ra::Core::Utils::logERROR ) << "NodeFactorySet: unable to find constructor for "
                                     << nodeType << " in any managed factory.";
    return nullptr;
}

namespace NodeFactoriesManager {

/**
 * \brief Allow static intialization without init order problems
 * \return The manager singleton
 */
NodeFactorySet& getFactoryManager() {
    static NodeFactorySet s_factoryManager {};
    return s_factoryManager;
}

bool registerFactory( NodeFactorySet::mapped_type factory ) {
    auto& fctMngr    = getFactoryManager();
    auto factoryName = factory->getName();
    return fctMngr.addFactory( std::move( factoryName ), std::move( factory ) );
}

NodeFactorySet::mapped_type createFactory( const std::string& name ) {
    return NodeFactorySet::mapped_type { new NodeFactorySet::mapped_type::element_type( name ) };
}

NodeFactorySet::mapped_type getFactory( NodeFactorySet::key_type factoryName ) {
    auto& fctMngr = getFactoryManager();
    auto factory  = fctMngr.find( factoryName );
    if ( factory == fctMngr.end() ) { return nullptr; }
    return factory->second;
}

bool unregisterFactory( NodeFactorySet::key_type factoryName ) {
    auto& fctMngr = getFactoryManager();
    return fctMngr.removeFactory( factoryName );
}

NodeFactorySet::mapped_type getDataFlowBuiltInsFactory() {
    auto& fctMngr = getFactoryManager();
    auto i        = fctMngr.find( NodeFactoriesManager::dataFlowBuiltInsFactoryName );
    if ( i != fctMngr.end() ) { return i->second; }

    // Should never be there
    std::cerr << "@&$&$@&$@&$ ERROR !!!!!! Core factory not registered\n";
    std::abort();
}

} // namespace NodeFactoriesManager

} // namespace Core
} // namespace Dataflow
} // namespace Ra
