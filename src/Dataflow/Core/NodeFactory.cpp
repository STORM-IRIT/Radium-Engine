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
        if ( owningGraph != nullptr ) { owningGraph->addNode( node ); }
        return node;
    }
#if 0
    else {
        std::cerr << "NodeFactory: no defined node for type " << nodeType << "." << std::endl;
        std::cerr << "Available nodes are : " << std::endl;
        for ( const auto& e : m_nodesCreators ) {
            std::cerr << "\t" << e.first << std::endl;
        }
    }
#endif
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
    else {
        std::cerr << "NodeFactory: trying to add an already existing node creator for type "
                  << nodeType << "." << std::endl;
        return false;
    }
}

size_t NodeFactory::nextNodeId() {
    return ++m_nodesCreated;
}

NodeFactorySet::NodeFactorySet() {
    // Add the "DataFlowBuiltIns" factory
    NodeFactoriesManager::getDataFlowBuiltInsFactory();
}

Node* NodeFactorySet::createNode( std::string& nodeType,
                                  const nlohmann::json& data,
                                  DataflowGraph* owningGraph ) {
    for ( const auto& it : m_factories ) {
        auto node = it.second->createNode( nodeType, data, owningGraph );
        if ( node ) { return node; }
    }
    std::cerr << "NodeFactorySet: unable to find constructor for " << nodeType
              << " in any of the following factories :" << std::endl;
    for ( const auto& it : m_factories ) {
        std::cerr << "\t" << it.first << std::endl;
    }
    return nullptr;
}

namespace NodeFactoriesManager {

/*
 * TODO, replace static data by this when implementing an autoregistration mecanism
// to prevent static intialization order
static NodeFactorySet& s_factoryManager() {
    static NodeFactorySet real_manager {};
    return real_manager;
};
*/
static NodeFactorySet s_factoryManager {};

NodeFactorySet getFactoryManager() {
    return s_factoryManager;
}

bool registerFactory( NodeFactorySet::mapped_type factory ) {
    auto factoryName = factory->getName();
    return s_factoryManager.addFactory( std::move( factoryName ), std::move( factory ) );
}

NodeFactorySet::mapped_type getFactory( NodeFactorySet::key_type factoryName ) {
    auto factory = s_factoryManager.find( factoryName );
    if ( factory == s_factoryManager.end() ) { return nullptr; }
    return factory->second;
}

bool unregisterFactory( NodeFactorySet::key_type factoryName ) {
    return s_factoryManager.removeFactory( factoryName );
}

NodeFactorySet::mapped_type getDataFlowBuiltInsFactory() {

    auto i = s_factoryManager.find( NodeFactoriesManager::dataFlowBuiltInsFactoryName );
    if ( i != s_factoryManager.end() ) { return i->second; }

    // TODO, replace this by an autoregistration mecanism
    registerStandardFactories();
    i = s_factoryManager.find( NodeFactoriesManager::dataFlowBuiltInsFactoryName );
    return i->second;
}

} // namespace NodeFactoriesManager

} // namespace Core
} // namespace Dataflow
} // namespace Ra
