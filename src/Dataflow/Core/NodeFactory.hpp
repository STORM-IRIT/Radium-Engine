#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/Node.hpp>

#include <Core/Utils/StdOptional.hpp>

#include <functional>
#include <iostream>
#include <unordered_map>

namespace Ra {
namespace Dataflow {
namespace Core {

class DataflowGraph;

/**
 * NodeFactory store a set of functions allowing to dynmically create dataflow nodes.
 * A NodeFactory is used when loading a nodegraph from a json representation of the graph to
 * instanciate all the loaded nodes.
 * Each DataflowGraph must have a reference to the nodeFactory to be used to create all the node he
 * has.
 *
 */
class RA_DATAFLOW_API NodeFactory
{
  public:
    /** Creates an empty factory with the given name */
    explicit NodeFactory( std::string name );

    std::string getName() const;

    /** Function that creates and initialize a node.
     *  Typical implementation of such a function should do the following :
     *   {
     *     auto node = new ConcreteNodeType( unique_instance_name );
     *     node->fromJson( data);
     *     return node;
     *   }
     */
    using NodeCreatorFunctor = std::function<Node*( const nlohmann::json& data )>;

    /**
     * Associate, for a given concrete node type, a custom NodeCreatorFunctor
     * @tparam T Concrete node type identifier
     * \param nodeCreator Functor to create an node of the corresponding concrete node type.
     * \param nodeCategory Category of the node.
     * \return true if the node creator is successfully added, false if not (e.g. due to a name
     * collision).
     */
    template <typename T>
    bool registerNodeCreator( NodeCreatorFunctor nodeCreator,
                              const std::string& nodeCategory = "RadiumNodes" );

    /**
     * Associate, for a given concrete node type, a generic NodeCreatorFunctor
     * @tparam T Concrete node type identifier
     * \param instanceNamePrefix prefix of the node instance name (will be called "prefix_i" with i
     * a unique number.
     * \param nodeCategory Category of the node.
     * \return true if the node creator is successfully added, false if not (e.g. due to a name
     * collision).
     */
    template <typename T>
    bool registerNodeCreator( const std::string& instanceNamePrefix,
                              const std::string& nodeCategory = "RadiumNodes" );
    /**
     * Associate, for a given concrete node type name, a NodeCreatorFunctor
     * \param nodeType the name of the concrete type
     * (the same as what is obtained by T::getTypename() on a node of type T)
     * \param nodeCreator Functor to create an node of the corresponding concrete node type.
     * \return true if the node creator is successfully added, false if not (e.g. due to a name
     * collision).
     */
    bool registerNodeCreator( std::string nodeType,
                              NodeCreatorFunctor nodeCreator,
                              const std::string& nodeCategory = "RadiumNodes" );

    /**
     * Get an unique, increasing node id.
     * \return
     */
    size_t nextNodeId();

    /** Create a node of the requested type.
     * The node is filled with the given json content.
     * If owningGraph is non null, the node is added to this graph
     * \param nodeType Type name of the node to be created.
     * \param data json representation of the node data (might be empty)
     * \param owningGraph if non null, the node is added to ths graph
     * \return the new node. Ownership of the returned pointer is left to the caller.
     */
    [[nodiscard]] Node* createNode( std::string& nodeType,
                                    const nlohmann::json& data,
                                    DataflowGraph* owningGraph = nullptr );

    /**
     * The type of the associative container used to store the factory
     * this container associate a concrete node type name to a pair
     * <concrete instanciator functor, name of the node category>
     * The name of the node category is helpful for graphical NodeGraph editor.
     * By default this is set to be "RadiumNodes"
     */
    using ContainerType =
        std::unordered_map<std::string, std::pair<NodeCreatorFunctor, std::string>>;
    /**
     * Get a const reference on the associative map
     * \return
     */
    [[nodiscard]] const ContainerType& getFactoryMap() const;

  private:
    ContainerType m_nodesCreators;
    size_t m_nodesCreated { 0 };
    std::string m_name;
};

/**
 * NodeFactorySet store a set of NodeFactory
 */
class RA_DATAFLOW_API NodeFactorySet
{
  public:
    using key_type    = std::string;
    using mapped_type = std::shared_ptr<NodeFactory>;
    using value_type  = std::pair<const key_type, mapped_type>;

    using container_type = std::map<key_type, mapped_type>;

    using const_iterator = container_type::const_iterator;
    using iterator       = container_type::iterator;

    /**
     * Add a factory to the set of factories available
     * \param factoryname the name of the factory
     * \param factory the factory
     * \return true if the factory was inserted, false if the insertion was prevented by an
     * already existing factory with the same name.
     */
    bool addFactory( key_type factoryname, mapped_type factory );

    /**
     * \brief Test if a factory exists in the set with the given name
     * \param factoryname The name of the factory to search for
     * \return an optional that is empty (evaluates to false) if no factory exeist with the given
     * name or that contains the existing factory.
     */
    Ra::Core::Utils::optional<mapped_type>
    hasFactory( const NodeFactorySet::key_type& factoryname );

    /**
     * Remove the identified factory from the set
     * \param factoryname the name of the factory to remove
     * \return true if the factory was removed, false if the factory does not exist in the set.
     */
    bool removeFactory( const key_type& factoryname );

    /**
     * Create a node using one of the functor (if it exists) registered in one factory for the given
     * type name.
     * \return the created node, nullptr if there is no construction functor registered for the
     * type.
     */
    [[nodiscard]] Node* createNode( std::string& nodeType,
                                    const nlohmann::json& data,
                                    DataflowGraph* owningGraph = nullptr );

    /* Wrappers to the interface of the underlying container
     * see https://en.cppreference.com/w/cpp/container/map
     */
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    const_iterator find( const key_type& key ) const;
    std::pair<iterator, bool> insert( value_type value );
    size_t erase( const key_type& key );

  private:
    container_type m_factories;
};

/** TODO Make this a class similar to all the managers of Radium.
 * Implement a NodeFactoryManager that stores a set of factories available to the  system.
 * Such a manager will be populated with Core::Dataflow node factories (Specialized sources,
 * specialized sink, ...) and will allow users to register its own factories.
 *
 * When creating a graph, the set of needed factories should be given as a constructor parameter
 * or built by adding factories identifier to the graph.
 *
 * When a graph is saved, the name of the factories he needs will be exported as string array
 * in the json.
 *
 * When a graph is loaded, the set of factories is built using the factories array in the json.
 *
 * @note: the factory name "DataFlowBuiltIns" is reserved and correspond to the base nodes available
 * for each dataflow graph (Specialized sources, specialized sink, ...). This factory will be
 * automatically added to all created factoryset.
 */
namespace NodeFactoriesManager {
/** Names of the system Builtins factories (automatically added to each graph) */
extern const std::string dataFlowBuiltInsFactoryName;

RA_DATAFLOW_API NodeFactorySet& getFactoryManager();

/** Register a factory into the manager.
 * The key will be fetched from the factory (its name)
 */
/**
 * \brief Register a factory into the manager.
 * The key will be fetched from the factory (its name)
 * \param factory
 * \return true if the factory was registered, false if not (e.g. due to name collision).
 */
RA_DATAFLOW_API bool registerFactory( NodeFactorySet::mapped_type factory );

/**
 * \brief Gets the given factory from the manager
 * \param factoryName
 * \return  a shared_ptr to the requested factory, nullptr if the factory does not exist.
 */
RA_DATAFLOW_API NodeFactorySet::mapped_type getFactory( NodeFactorySet::key_type factoryName );

/**
 * \brief Unregister the factory from the manager
 * \param factoryName
 * \return true if the factory was unregistered, false if not (e.g. for names not being managed).
 */
RA_DATAFLOW_API bool unregisterFactory( NodeFactorySet::key_type factoryName );

/**
 * \brief Gets the factory for nodes exported by the Core dataflow library.
 * \return
 */
RA_DATAFLOW_API NodeFactorySet::mapped_type getDataFlowBuiltInsFactory();
} // namespace NodeFactoriesManager

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

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
