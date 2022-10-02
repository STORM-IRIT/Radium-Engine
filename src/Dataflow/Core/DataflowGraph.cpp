#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Port.hpp>

#include <fstream>
#include <map>

namespace Ra {
namespace Dataflow {
namespace Core {

DataflowGraph::DataflowGraph( const std::string& name ) : DataflowGraph( name, getTypename() ) {}

DataflowGraph::DataflowGraph( const std::string& instanceName, const std::string& typeName ) :
    Node( instanceName, typeName ) {
    // This will alllow to edit subgraph in an independant editor
    addEditableParameter( new EditableParameter( instanceName, *this ) );
    // A graph always use the builtin nodes factory
    addFactory( NodeFactoriesManager::getDataFlowBuiltInsFactory() );
}

void DataflowGraph::init() {
    Node::init();
    auto compileOK = compile();
    if ( compileOK ) {
#ifdef GRAPH_CALL_TRACE
        int i = 0;
        std::for_each( m_nodesByLevel.begin(), m_nodesByLevel.end(), [&i]( const auto& level ) {
            std::cout << "- \e[1mLevel " << i++ << "\e[0m" << std::endl;
#else

        std::for_each( m_nodesByLevel.begin(), m_nodesByLevel.end(), []( const auto& level ) {
#endif
            std::for_each( level.begin(), level.end(), []( auto node ) {
                if ( !node->m_initialized ) {
                    node->init();
                    node->m_initialized = true;
                }
            } );
        } );
    }
    m_recompile = !compileOK;
}

void DataflowGraph::execute() {
    if ( m_ready ) {
#ifdef GRAPH_CALL_TRACE
        std::cout << std::endl
                  << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName << "\": execute."
                  << std::endl;

        int i = 0;
        std::for_each( m_nodesByLevel.begin(), m_nodesByLevel.end(), [&i]( const auto& level ) {
            std::cout << "- \e[1mLevel " << i++ << "\e[0m" << std::endl;
#else

        std::for_each( m_nodesByLevel.begin(), m_nodesByLevel.end(), []( const auto& level ) {
#endif
            std::for_each( level.begin(), level.end(), []( auto node ) { node->execute(); } );
        } );
    }
}

void DataflowGraph::destroy() {
    Node::destroy();
    int i = 0;
    std::for_each( m_nodesByLevel.begin(), m_nodesByLevel.end(), [&i]( const auto& level ) {
#ifdef GRAPH_CALL_TRACE
        std::cout << "- \e[1mLevel " << i << "\e[0m" << std::endl;
#endif
        i++;
        std::for_each( level.begin(), level.end(), []( auto node ) { node->destroy(); } );
    } );
}

void DataflowGraph::saveToJson( const std::string& jsonFilePath ) {
    if ( !jsonFilePath.empty() ) {
        nlohmann::json data;
        toJson( data );
        std::ofstream file( jsonFilePath );
        file << std::setw( 4 ) << data << std::endl;
    }
}

void DataflowGraph::toJsonInternal( nlohmann::json& data ) const {
    nlohmann::json factories   = nlohmann::json::array();
    nlohmann::json nodes       = nlohmann::json::array();
    nlohmann::json connections = nlohmann::json::array();
    nlohmann::json model;
    nlohmann::json graph;

    if ( m_factories ) {
        for ( const auto& [name, factory] : *m_factories ) {
            // do not save the standard factory, it will always be there
            if ( name != NodeFactoriesManager::dataFlowBuiltInsFactoryName ) {
                factories.push_back( name );
            }
        }
        graph["factories"] = factories;
    }

    for ( const auto& n : m_nodes ) {
        nlohmann::json nodeData;
        n->toJson( nodeData );
        nodes.push_back( nodeData );
        int numPort = 0;
        for ( const auto& input : n->getInputs() ) {
            if ( input->isLinked() ) {
                nlohmann::json link = nlohmann::json::object();
                link["in_id"]       = n->getUuid();
                link["in_index"]    = numPort;
                auto portOut        = input->getLink();
                auto nodeOut        = portOut->getNode();
                int outPortIndex    = 0;
                for ( const auto& p : nodeOut->getOutputs() ) {
                    if ( p.get() == portOut ) { break; }
                    outPortIndex++;
                }
                link["out_id"]    = nodeOut->getUuid();
                link["out_index"] = outPortIndex;
                connections.push_back( link );
            }
            numPort++;
        }
    }

    // write the common content of the Node to the json data
    graph["nodes"]       = nodes;
    graph["connections"] = connections;
    // Fill the specific concrete node informations
    data.emplace( "graph", graph );
}

bool DataflowGraph::loadFromJson( const std::string& jsonFilePath ) {
    std::cout << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName
              << "\": loadFromJson: " << jsonFilePath << std::endl;
    std::ifstream file( jsonFilePath );
    nlohmann::json j;
    file >> j;
    fromJson( j );
    return true;
}

void DataflowGraph::fromJsonInternal( const nlohmann::json& data ) {

    if ( data.contains( "graph" ) ) {
        // indicate that the graph must be recompiled after loading
        m_recompile = true;
        // load the graph
        m_factories.reset( new NodeFactorySet );
        addFactory( NodeFactoriesManager::getDataFlowBuiltInsFactory() );
        if ( data["graph"].contains( "factories" ) ) {
            auto factories = data["graph"]["factories"];
            for ( const auto& factoryName : factories ) {
                // Do not add factories already registered for the graph.
                if ( m_factories->hasFactory( factoryName ) ) {
                    std::cerr << "PLOPOPOPOO !!!!\n";
                    continue;
                }
                auto factory = NodeFactoriesManager::getFactory( factoryName );
                if ( factory ) { addFactory( factory ); }
                else {
                    std::cerr << "DataflowGraph::loadFromJson : Unable to find a factory with name "
                              << factoryName << std::endl;
                    return;
                }
            }
        }
        if ( !m_factories ) {
            std::cerr << "DataflowGraph::loadFromJson : no node factories available !";
            return;
        }
        std::unordered_map<std::string, Node*> nodeById;

        auto nodes = data["graph"]["nodes"];
        for ( auto& n : nodes ) {
            std::string name = n["model"]["name"];
            std::string id   = n["id"];

            auto newNode = m_factories->createNode( name, n, this );
            if ( newNode ) { nodeById.emplace( id, newNode ); }
            else {
                std::cerr << "Unable to create the node " << name << std::endl;
            }
        }

        auto links = data["graph"]["connections"];
        for ( auto& l : links ) {
            Node* nodeFrom { nullptr };
            std::string fromOutput { "" };
            Node* nodeTo { nullptr };
            std::string toInput { "" };

            if ( nodeById.find( l["out_id"] ) != nodeById.end() ) {
                nodeFrom      = nodeById[l["out_id"]];
                int fromIndex = l["out_index"];

                if ( fromIndex >= 0 && fromIndex < int( nodeFrom->getOutputs().size() ) ) {
                    fromOutput = nodeFrom->getOutputs()[fromIndex]->getName();
                }
                else {
                    std::cerr << "Error when reading JSON file \""
                              << "\": Output index " << fromIndex << " for node \""
                              << nodeFrom->getInstanceName() << " (" << nodeFrom->getTypeName()
                              << ")\" must be between 0 and " << nodeFrom->getOutputs().size() - 1
                              << ". Link not added." << std::endl;
                }
            }
            else {
                std::cerr << "Error when reading JSON file \""
                          << "\": Could not find a node associated with id " << l["out_id"]
                          << ". Link not added." << std::endl;
            }

            if ( nodeById.find( l["in_id"] ) != nodeById.end() ) {
                nodeTo      = nodeById[l["in_id"]];
                int toIndex = l["in_index"];

                if ( toIndex >= 0 && toIndex < int( nodeTo->getInputs().size() ) ) {
                    toInput = nodeTo->getInputs()[toIndex]->getName();
                }
                else {
                    std::cerr << "Error when reading JSON file \""
                              << "\": Input index " << toIndex << " for node \""
                              << nodeFrom->getInstanceName() << " (" << nodeFrom->getTypeName()
                              << ")\" must be between 0 and " << nodeTo->getInputs().size() - 1
                              << ". Link not added." << std::endl;
                }
            }
            else {
                std::cerr << "Error when reading JSON file \""
                          << "\": Could not find a node associated with id " << l["in_id"]
                          << ". Link not added." << std::endl;
            }

            if ( nodeFrom && ( fromOutput != "" ) && nodeTo && ( toInput != "" ) ) {
                addLink( nodeFrom, fromOutput, nodeTo, toInput );
            }
            else {
                std::cerr
                    << "Error when reading JSON file \""
                    << "\": Could not add a link (missing or wrong information, please refer to "
                       "the previous error messages). Link not added."
                    << std::endl;
            }
        }
    }
}

bool DataflowGraph::addNode( Node* newNode ) {
    std::map<std::string, std::string> m_mapInputs;
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName << "\": trying to add node \""
              << newNode->getInstanceName() << "\"..." << std::endl;
#endif
    // Check if the new node already exists (= same name)
    if ( findNode( newNode->getInstanceName() ) == -1 ) {
#ifdef GRAPH_CALL_TRACE
        std::cout << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName
                  << "\": success adding node \"" << newNode->getInstanceName() << "\"!"
                  << std::endl;
#endif
        if ( newNode->getInputs().size() == 0 ) { // Check if it is a source node
            for ( auto& p : newNode->getOutputs() ) {
#ifdef GRAPH_CALL_TRACE
                std::cout << "\e[33m\e[1mDataflowGraph\e[0m \"" << m_instanceName
                          << "\": reflecting OUTPUT port \"" << p->getName() << "\" from \""
                          << newNode->getInstanceName() << "\"!" << std::endl;
#endif
                auto portInterface =
                    p->reflect( this, newNode->getInstanceName() + '_' + p->getName() );
                portInterface->mustBeLinked();
                newNode->addInterface( portInterface );
                addInput( portInterface );
            }
        }
        if ( newNode->getOutputs().size() == 0 ) { // Check if it is a sink node
            for ( auto& p : newNode->getInputs() ) {
#ifdef GRAPH_CALL_TRACE
                std::cout << "\e[34m\e[1mDataflowGraph\e[0m \"" << m_instanceName
                          << "\": reflecting INPUT port \"" << p->getName() << "\" from \""
                          << newNode->getInstanceName() << "\"!" << std::endl;
#endif
                auto portInterface =
                    p->reflect( this, newNode->getInstanceName() + '_' + p->getName() );
                newNode->addInterface( portInterface );
                addOutput( portInterface );
            }
        }
        m_nodes.emplace_back( newNode );
        return true;
    }
    else {
#ifdef GRAPH_CALL_TRACE
        std::cerr << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName
                  << "\": could not add node \"" << newNode->getInstanceName()
                  << "\" (node already exists)." << std::endl;
#endif
        return false;
    }
}

bool DataflowGraph::removeNode( Node* node ) {
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName
              << "\": trying to remove node \"" << node->getInstanceName() << "\"..." << std::endl;
#endif
    // Check if the new node already exists (= same name)
    int index = -1;
    if ( ( index = findNode( node->getInstanceName() ) ) == -1 ) {
#ifdef GRAPH_CALL_TRACE
        std::cerr << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName
                  << "\": could not remove node \"" << node->getInstanceName()
                  << "\" (node does not exist)." << std::endl;
#endif
        return false;
    }
    else {
#ifdef GRAPH_CALL_TRACE
        std::cout << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName
                  << "\": success removing node \"" << node->getInstanceName() << "\"!"
                  << std::endl;
#endif
        if ( node->getInputs().size() == 0 ) {          // Check if it is a source node
            for ( auto& port : node->getInterface() ) { // Erase input ports of the graph associated
                                                        // to the interface ports of the node
                for ( auto itG = m_inputs.begin(); itG != m_inputs.end(); ++itG ) {
                    if ( port->getName() ==
                         ( *itG )->getName() ) { // Check if these ports are the same
                        m_inputs.erase( itG );
                    }
                    break;
                }
            }
        }
        if ( node->getOutputs().size() == 0 ) {         // Check if it is a sink node
            for ( auto& port : node->getInterface() ) { // Erase input ports of the graph associated
                                                        // to the interface ports of the node
                for ( auto itG = m_outputs.begin(); itG != m_outputs.end(); ++itG ) {
                    if ( port->getName() ==
                         ( *itG )->getName() ) { // Check if these ports are the same
                        m_outputs.erase( itG );
                        break;
                    }
                }
            }
        }
        m_nodes.erase( m_nodes.begin() + index );
        return true;
    }
}

bool DataflowGraph::addLink( Node* nodeFrom,
                             const std::string& nodeFromOutputName,
                             Node* nodeTo,
                             const std::string& nodeToInputName ) {
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName
              << "\": ADD LINK : try to connect output \"" + nodeFromOutputName + "\" of node \"" +
                     nodeFrom->getInstanceName() + "\" to input \"" + nodeToInputName +
                     "\" of node \"" + nodeTo->getInstanceName() + "\"."
              << std::endl;
#endif
    // Check node "from" existence in the graph
    if ( findNode( nodeFrom->getInstanceName() ) == -1 ) {
#ifdef GRAPH_CALL_TRACE
        std::cerr << "ADD LINK : node \"from\" \"" + nodeFrom->getInstanceName() +
                         "\" does not exist."
                  << std::endl;
#endif
        return false;
    }

    // Check node "to" existence in the graph
    if ( findNode( nodeTo->getInstanceName() ) == -1 ) {
#ifdef GRAPH_CALL_TRACE
        std::cerr << "ADD LINK : node \"to\" \"" + nodeTo->getInstanceName() + "\" does not exist."
                  << std::endl;
#endif
        return false;
    }

    // Check if node "from"'s output exists
    int foundFrom = -1;
    int index     = 0;
    for ( auto& output : nodeFrom->getOutputs() ) {
        if ( output->getName() == nodeFromOutputName ) {
            foundFrom = index;
            break;
        }
        index++;
    }
    if ( foundFrom == -1 ) {
#ifdef GRAPH_CALL_TRACE
        std::cerr << "ADD LINK : output \"" + nodeFromOutputName + "\" for node \"from\" \"" +
                         nodeFrom->getInstanceName() + "\" does not exist."
                  << std::endl;
#endif
        return false;
    }

    // Check if node "to"'s input exists
    int foundTo = -1;
    index       = 0;
    for ( auto& input : nodeTo->getInputs() ) {
        if ( input->getName() == nodeToInputName ) {
            foundTo = index;
            break;
        }
        index++;
    }
    if ( foundTo == -1 ) {
#ifdef GRAPH_CALL_TRACE
        std::cerr << "ADD LINK : input \"" + nodeToInputName + "\" for target node \"" +
                         nodeTo->getInstanceName() + "\" does not exist."
                  << std::endl;
#endif
        return false;
    }

    // Compare types
    if ( nodeTo->getInputs()[foundTo]->getType() != nodeFrom->getOutputs()[foundFrom]->getType() ) {
#ifdef GRAPH_CALL_TRACE
        std::cerr << "ADD LINK : cannot connect output \"" + nodeFromOutputName + "\" for node \"" +
                         nodeTo->getInstanceName() + "\" and input \"" + nodeToInputName +
                         "\" for node \"" + nodeFrom->getInstanceName() + "\" : type mismatch."
                  << std::endl;
#endif
        return false;
    }

    // Check if input is connected
    if ( nodeTo->getInputs()[foundTo]->isLinked() ) {
#ifdef GRAPH_CALL_TRACE
        std::cerr << "ADD LINK : cannot connect output \"" + nodeFromOutputName + "\" for node \"" +
                         nodeTo->getInstanceName() + "\" and input \"" + nodeToInputName +
                         "\" for node \"" + nodeFrom->getInstanceName() +
                         "\" : input already connected."
                  << std::endl;
#endif
        return false;
    }

    // Try to connect ports
    if ( !nodeTo->getInputs()[foundTo]->connect( nodeFrom->getOutputs()[foundFrom].get() ) ) {
#ifdef GRAPH_CALL_TRACE
        std::cerr << "ADD LINK : cannot connect output \"" + nodeFromOutputName + "\" for node \"" +
                         nodeTo->getInstanceName() + "\" and input \"" + nodeToInputName +
                         "\" for node \"" + nodeFrom->getInstanceName() + "\"."
                  << std::endl;
#endif
        return false;
    }
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName
              << "\": ADD LINK : success connecting output \"" + nodeFromOutputName +
                     "\" of node \"" + nodeFrom->getInstanceName() + "\" to input \"" +
                     nodeToInputName + "\" of node \"" + nodeTo->getInstanceName() + "\"."
              << std::endl;
#endif
    return true;
}

bool DataflowGraph::removeLink( Node* node, const std::string& nodeInputName ) {
    // Check node's existence in the graph
    if ( findNode( node->getInstanceName() ) == -1 ) {
#ifdef GRAPH_CALL_TRACE
        std::cerr << "REMOVE LINK : node \"" + node->getInstanceName() + "\" does not exist."
                  << std::endl;
#endif
        return false;
    }

    // Check if node's input exists
    int found = -1;
    int index = 0;
    for ( auto& input : node->getInputs() ) {
        if ( input->getName() == nodeInputName ) {
            found = index;
            break;
        }
        index++;
    }
    if ( found == -1 ) {
#ifdef GRAPH_CALL_TRACE
        std::cerr << "REMOVE LINK : input \"" + nodeInputName + "\" for target node \"" +
                         node->getInstanceName() + "\" does not exist."
                  << std::endl;
#endif
        return false;
    }

    node->getInputs()[found]->disconnect();
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName
              << "\": REMOVE LINK : success disconnecting input \"" + nodeInputName +
                     "\" of node \"" + node->getInstanceName() + "\"."
              << std::endl;
#endif
    return true;
}

int DataflowGraph::findNode( const std::string& name ) {
    for ( size_t i = 0; i < m_nodes.size(); i++ ) {
        if ( m_nodes[i]->getInstanceName() == name ) { return i; }
    }

    return -1;
}

bool DataflowGraph::compile() {
#ifdef GRAPH_CALL_TRACE
    std::cout << std::endl
              << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName << "\": begin compilation."
              << std::endl;
#endif
    // Find useful nodes (directly or indirectly connected to a Sink)
    std::unordered_map<Node*, std::pair<int, std::vector<Node*>>> infoNodes;
    for ( auto const& n : m_nodes ) {
        // Find all sinks
        if ( n->getOutputs().size() == 0 ) {
            // Add the sink in the useful nodes set
            infoNodes.emplace( n.get(), std::pair<int, std::vector<Node*>>( 0, {} ) );
            // recursively add the predecessors of the sink
            backtrackGraph( n.get(), infoNodes );
        }
    }
#ifdef GRAPH_CALL_TRACE
    std::cout << std::endl
              << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName << "\": useful nodes found."
              << std::endl;
#endif
    // Compute the level (rank of execution) of useful nodes
    int maxLevel = 0;
    for ( auto& infNode : infoNodes ) {
        auto n = infNode.first;
        // n->setResourcesDir( m_resourceDir ); // --> This must be configured per node, e.g. by the
        // factory Compute the nodes' level starting from sources
        if ( n->getInputs().empty() ) {
            // Tag successors
            for ( auto const successor : infNode.second.second ) {
                infoNodes[successor].first =
                    std::max( infoNodes[successor].first, infoNodes[n].first + 1 );
                maxLevel = std::max( maxLevel,
                                     std::max( infoNodes[successor].first,
                                               goThroughGraph( successor, infoNodes ) ) );
            }
        }
    }
#ifdef GRAPH_CALL_TRACE
    std::cout << std::endl
              << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName << "\": nodes level computed."
              << std::endl;
#endif
    m_nodesByLevel.clear();
    m_nodesByLevel.resize( infoNodes.size() != 0 ? maxLevel + 1 : 0 );
    for ( auto& infNode : infoNodes ) {
        m_nodesByLevel[infNode.second.first].push_back( infNode.first );
    }

    // For each level
    for ( auto& lvl : m_nodesByLevel ) {
        // For each node
        for ( size_t j = 0; j < lvl.size(); j++ ) {
            if ( !lvl[j]->compile() ) { return m_ready = false; }
            // For each input
            for ( size_t k = 0; k < lvl[j]->getInputs().size(); k++ ) {
                if ( lvl[j]->getInputs()[k]->isLinkMandatory() &&
                     !lvl[j]->getInputs()[k]->isLinked() ) {
#ifdef GRAPH_CALL_TRACE
                    std::cout << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName
                              << "\": compilation failed." << std::endl;
#endif
                    return m_ready = false;
                }
            }
        }
    }
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[32m\e[1mDataflowGraph\e[0m \"" << m_instanceName << "\": end compilation."
              << std::endl
              << std::endl;
#endif
    return m_ready = postCompilationOperation();
}

void DataflowGraph::clearNodes() {

    for ( size_t i = 0; i < m_nodesByLevel.size(); i++ ) {
        m_nodesByLevel[i].clear();
        m_nodesByLevel[i].shrink_to_fit();
    }
    m_nodesByLevel.clear();
    m_nodesByLevel.shrink_to_fit();

#if 0
    --> specific to rendergraph ???
    // Remove only non permanent nodes
    // disconnect sink
    getDisplayNode()->disconnectInputs();
    // remove node
    m_nodes.erase( m_nodes.begin() + 4, m_nodes.end() );
#endif
    m_nodes.erase( std::remove_if( m_nodes.begin(),
                                   m_nodes.end(),
                                   []( const auto& n ) { return n->isDeletable(); } ),
                   m_nodes.end() );
    m_nodes.shrink_to_fit();
}

void DataflowGraph::backtrackGraph(
    Node* current,
    std::unordered_map<Node*, std::pair<int, std::vector<Node*>>>& infoNodes ) {
    for ( auto& input : current->getInputs() ) {
        if ( input->getLink() ) {
            Node* previous = input->getLink()->getNode();
            if ( previous ) {
                auto previousInInfoNodes = infoNodes.find( previous );
                if ( previousInInfoNodes != infoNodes.end() ) {
                    // If the previous node is already in the map,
                    // find if the current node is already a successor node
                    auto& previousSuccessors = previousInInfoNodes->second.second;
                    bool foundCurrent        = std::any_of( previousSuccessors.begin(),
                                                     previousSuccessors.end(),
                                                     [current]( auto c ) { return c == current; } );
                    if ( !foundCurrent ) {
                        // If the current node is not a successor node, add it to the list
                        previousSuccessors.push_back( current );
                    }
                }
                else {
                    // Add node to info nodes
                    std::vector<Node*> successors;
                    successors.push_back( current );
                    infoNodes.emplace(
                        previous,
                        std::pair<int, std::vector<Node*>>( 0, std::move( successors ) ) );
                    backtrackGraph( previous, infoNodes );
                }
            }
        }
    }
}

int DataflowGraph::goThroughGraph(
    Node* current,
    std::unordered_map<Node*, std::pair<int, std::vector<Node*>>>& infoNodes ) {
    int maxLevel = 0;
    if ( infoNodes.find( current ) != infoNodes.end() ) {
        for ( auto const& successor : infoNodes[current].second ) {
            infoNodes[successor].first =
                std::max( infoNodes[successor].first, infoNodes[current].first + 1 );
            maxLevel =
                std::max( infoNodes[successor].first, goThroughGraph( successor, infoNodes ) );
        }
    }
    return maxLevel;
}

std::shared_ptr<PortBase> DataflowGraph::getDataSetter( std::string portName ) {
    for ( auto& portIn : m_inputs ) {
        if ( portIn->getName() == portName ) {
            portIn->disconnect();
            auto portOut = portIn->reflect( this, portName );
            portOut->connect( portIn.get() );
            return std::shared_ptr<PortBase>( portOut );
        }
    }
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[36m\e[1mDataflowGraph::graphGetInput \e[0m \""
              << "Error, can't generate the interface node for graph input " << portName
              << std::endl;
#endif
    return nullptr;
}

std::vector<DataflowGraph::DataSetterDesc> DataflowGraph::getAllDataSetters() {
    std::vector<DataflowGraph::DataSetterDesc> r;
    r.reserve( m_inputs.size() );
    for ( auto& portIn : m_inputs ) {
        portIn->disconnect();
        auto portOut = portIn->reflect( this, portIn->getName() );
        portOut->connect( portIn.get() );
        r.emplace_back(
            std::shared_ptr<PortBase>( portOut ), portOut->getName(), portOut->getTypeName() );
    }
    return r;
}

/// Not sure this method do the right thing ... if we want to get data from the port, it must be an
/// output port ...

PortBase* DataflowGraph::getDataGetter( std::string portName ) {
    for ( auto& portOut : m_outputs ) {
        if ( portOut->getName() == portName ) { return portOut.get(); }
    }
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[36m\e[1mDataflowGraph::graphGetOutput \e[0m \""
              << "Error, can't generate the interface node for graph output " << portName
              << std::endl;
#endif
    return nullptr;
}

std::vector<DataflowGraph::DataGetterDesc> DataflowGraph::getAllDataGetters() {
    std::vector<DataflowGraph::DataGetterDesc> r;
    r.reserve( m_outputs.size() );
    for ( auto& portOut : m_outputs ) {
        r.emplace_back( portOut.get(), portOut->getName(), portOut->getTypeName() );
    }
    return r;
}

Node* DataflowGraph::getNode( const std::string& instanceNameNode ) {
    for ( const auto& node : m_nodes ) {
        if ( node->getInstanceName() == instanceNameNode ) { return node.get(); }
    }
    std::cerr << getTypename() + ": The node with the instance name " + instanceNameNode +
                     " has not been found";
    return nullptr;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
