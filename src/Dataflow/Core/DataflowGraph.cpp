#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Port.hpp>

#include <fstream>
#include <map>

#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

using namespace Ra::Core::Utils;

DataflowGraph::DataflowGraph( const std::string& name ) : DataflowGraph( name, getTypename() ) {}

DataflowGraph::DataflowGraph( const std::string& instanceName, const std::string& typeName ) :
    Node( instanceName, typeName ) {
    // This will allow to edit subgraph in an independent editor
    addEditableParameter( new EditableParameter( instanceName, *this ) );
    // A graph always use the builtin nodes factory
    addFactory( NodeFactoriesManager::getDataFlowBuiltInsFactory() );
}

void DataflowGraph::init() {
    if ( m_ready ) {
        Node::init();
        std::for_each( m_nodesByLevel.begin(), m_nodesByLevel.end(), []( const auto& level ) {
            std::for_each( level.begin(), level.end(), []( auto node ) {
                if ( !node->m_initialized ) { node->init(); }
            } );
        } );
    }
}

bool DataflowGraph::execute() {
    if ( !m_ready ) {
        if ( !compile() ) { return false; }
    }
    bool result = true;
    std::for_each( m_nodesByLevel.begin(), m_nodesByLevel.end(), [&result]( const auto& level ) {
        std::for_each( level.begin(), level.end(), [&result]( auto node ) {
            result = result && node->execute();
        } );
    } );
    return result;
}

void DataflowGraph::destroy() {
    std::for_each(
        m_nodesByLevel.begin(), m_nodesByLevel.end(), []( auto& level ) { level.clear(); } );
    m_nodesByLevel.clear();
    m_nodes.clear();
    m_factories.reset();
    m_dataSetters.clear();
    Node::destroy();
    m_ready         = false;
    m_shouldBeSaved = true;
}

void DataflowGraph::saveToJson( const std::string& jsonFilePath ) {
    if ( !jsonFilePath.empty() ) {
        nlohmann::json data;
        toJson( data );
        std::ofstream file( jsonFilePath );
        file << std::setw( 4 ) << data << std::endl;
        m_shouldBeSaved = false;
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

    if ( !nlohmann::json::accept( std::ifstream( jsonFilePath ) ) ) {
        LOG( logERROR ) << jsonFilePath << " is not a valid json file !!";
        return false;
    }

    std::ifstream file( jsonFilePath );
    nlohmann::json j;
    file >> j;
    m_shouldBeSaved = false;
    return fromJson( j );
}

bool DataflowGraph::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "graph" ) ) {
        // indicate that the graph must be recompiled after loading
        m_ready = false;
        // load the graph
        m_factories.reset( new NodeFactorySet );
        addFactory( NodeFactoriesManager::getDataFlowBuiltInsFactory() );
        if ( data["graph"].contains( "factories" ) ) {
            auto factories = data["graph"]["factories"];
            for ( const auto& factoryName : factories ) {
                // Do not add factories already registered for the graph.
                if ( m_factories->hasFactory( factoryName ) ) { continue; }
                auto factory = NodeFactoriesManager::getFactory( factoryName );
                if ( factory ) { addFactory( factory ); }
                else {
                    LOG( logERROR )
                        << "DataflowGraph::loadFromJson : Unable to find a factory with name "
                        << factoryName;
                    return false;
                }
            }
        }
        std::unordered_map<std::string, Node*> nodeById;

        auto nodes = data["graph"]["nodes"];
        for ( auto& n : nodes ) {
            std::string name = n["model"]["name"];
            std::string id   = n["id"];

            auto newNode = m_factories->createNode( name, n, this );
            if ( newNode ) { nodeById.emplace( id, newNode ); }
            else {
                LOG( logERROR ) << "Unable to create the node " << name;
                return false;
            }
        }

        auto links = data["graph"]["connections"];
        for ( auto& l : links ) {
            Node* nodeFrom { nullptr };
            std::string fromOutput { "" };
            Node* nodeTo { nullptr };
            std::string toInput { "" };

            auto nodeId = l["out_id"];
            auto itNode = nodeById.find( nodeId );
            if ( itNode != nodeById.end() ) {
                nodeFrom      = itNode->second;
                int fromIndex = l["out_index"];
                if ( fromIndex >= 0 && fromIndex < int( nodeFrom->getOutputs().size() ) ) {
                    fromOutput = nodeFrom->getOutputs()[fromIndex]->getName();
                }
                else {
                    LOG( logERROR ) << "DataflowGraph::loadFromJson: error when parsing JSON"
                                    << ": Output index " << fromIndex << " for node \""
                                    << nodeFrom->getInstanceName() << " ("
                                    << nodeFrom->getTypeName() << ")\" must be between 0 and "
                                    << nodeFrom->getOutputs().size() - 1 << ". Link not added.";
                    return false;
                }
            }
            else {
                LOG( logERROR ) << "DataflowGraph::loadFromJson: error when parsing JSON"
                                << ": Could not find a node associated with id " << nodeId
                                << ". Link not added.";
                return false;
            }

            nodeId = l["in_id"];
            itNode = nodeById.find( nodeId );
            if ( itNode != nodeById.end() ) {
                nodeTo      = itNode->second;
                int toIndex = l["in_index"];

                if ( toIndex >= 0 && toIndex < int( nodeTo->getInputs().size() ) ) {
                    toInput = nodeTo->getInputs()[toIndex]->getName();
                }
                else {
                    LOG( logERROR ) << "DataflowGraph::loadFromJson: error when parsing JSON"
                                    << ": Input index " << toIndex << " for node \""
                                    << nodeTo->getInstanceName() << " (" << nodeTo->getTypeName()
                                    << ")\" must be between 0 and "
                                    << nodeTo->getInputs().size() - 1 << ". Link not added.";
                    return false;
                }
            }
            else {
                LOG( logERROR ) << "DataflowGraph::loadFromJson: error when parsing JSON"
                                << ": Could not find a node associated with id " << nodeId
                                << ". Link not added.";
                return false;
            }

            if ( !addLink( nodeFrom, fromOutput, nodeTo, toInput ) ) {
                LOG( logERROR )
                    << "DataflowGraph::loadFromJson: error when parsing JSON"
                    << ": Could not add a link (missing or wrong information, please refer to "
                       "the previous error messages). Link not added.";
                return false;
            }
        }
    }
    return true;
}

bool DataflowGraph::canAdd( const Node* newNode ) const {
    return findNode( newNode ) == -1;
}

std::pair<bool, Node*> DataflowGraph::addNode( std::unique_ptr<Node> newNode ) {
    std::map<std::string, std::string> m_mapInputs;
    // Check if the new node already exists (= same name and type)
    if ( canAdd( newNode.get() ) ) {
        auto addedNode = newNode.get();
        m_nodes.emplace_back( std::move( newNode ) );
        if ( addedNode->getInputs().empty() ) {
            // it is a source node, add its interface port as input and data setter to the graph
            auto& interfaces = addedNode->buildInterfaces( this );
            for ( auto p : interfaces ) {
                addSetter( p );
            }
        }
        if ( addedNode->getOutputs().empty() ) {
            // it is a sink node, add its interface port as output to the graph
            auto& interfaces = addedNode->buildInterfaces( this );
            for ( auto p : interfaces ) {
                addGetter( p );
            }
        }
        m_ready         = false;
        m_shouldBeSaved = true;
        return { true, addedNode };
    }
    else {
        return { false, newNode.release() };
    }
}

bool DataflowGraph::removeNode( Node*& node ) {
    // This is to prevent graph destruction from the graph editor, depending on how it is used
    if ( m_nodesAndLinksProtected ) { return false; }

    // Check if the new node already exists (= same name)
    int index = -1;
    if ( ( index = findNode( node ) ) == -1 ) { return false; }
    else {
        if ( node->getInputs().empty() ) { // Check if it is a source node
            for ( auto& port :
                  node->getInterfaces() ) { // Erase input ports of the graph associated
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
        if ( node->getOutputs().empty() ) { // Check if it is a sink node
            for ( auto& port :
                  node->getInterfaces() ) { // Erase input ports of the graph associated
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
        node            = nullptr;
        m_ready         = false;
        m_shouldBeSaved = true;
        return true;
    }
}

bool DataflowGraph::addLink( Node* nodeFrom,
                             const std::string& nodeFromOutputName,
                             Node* nodeTo,
                             const std::string& nodeToInputName ) {
    // Check node "from" existence in the graph
    if ( findNode( nodeFrom ) == -1 ) {
        LOG( logERROR ) << "DataflowGraph::addLink Unable to find initial node "
                        << nodeFrom->getInstanceName();
        return false;
    }

    // Check node "to" existence in the graph
    if ( findNode( nodeTo ) == -1 ) {
        LOG( logERROR ) << "DataflowGraph::addLink Unable to find destination node "
                        << nodeTo->getInstanceName();
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
        LOG( logERROR ) << "DataflowGraph::addLink Unable to find output port "
                        << nodeFromOutputName << " from initial node "
                        << nodeFrom->getInstanceName() << " (" << nodeFrom->getTypeName() << ")";
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
        LOG( logERROR ) << "DataflowGraph::addLink Unable to find input port " << nodeFromOutputName
                        << " from destination node " << nodeTo->getInstanceName() << " ("
                        << nodeTo->getTypeName() << ")";
        return false;
    }

    // Compare types
    if ( !( nodeTo->getInputs()[foundTo]->getType() ==
            nodeFrom->getOutputs()[foundFrom]->getType() ) ) {
        LOG( logERROR ) << "DataflowGraph::addLink type mismatch from "
                        << nodeFrom->getInstanceName() << " (" << nodeFrom->getTypeName() << ") / "
                        << nodeFrom->getOutputs()[foundFrom]->getName() << " (" << foundFrom
                        << " with type " << nodeFrom->getOutputs()[foundFrom]->getTypeName() << ")"
                        << " to " << nodeTo->getInstanceName() << " (" << nodeTo->getTypeName()
                        << ") / " << nodeTo->getInputs()[foundTo]->getName() << " (" << foundTo
                        << " with type " << nodeTo->getInputs()[foundTo]->getTypeName() << ") ";
        return false;
    }

    // Check if input is connected
    if ( nodeTo->getInputs()[foundTo]->isLinked() ) {
        LOG( logERROR )
            << "DataflowGraph::addLink destination port not available (already linked) for "
            << nodeTo->getInstanceName() << " (" << nodeFrom->getTypeName() << "), port "
            << nodeTo->getInputs()[foundTo]->getName();
        return false;
    }

    // port can be connected
    nodeTo->getInputs()[foundTo]->connect( nodeFrom->getOutputs()[foundFrom].get() );
    // The state of the graph changes, set it to not ready
    m_ready         = false;
    m_shouldBeSaved = true;
    return true;
}

bool DataflowGraph::removeLink( Node* node, const std::string& nodeInputName ) {
    // This is to prevent graph destruction from the graph editor, depending on how it is used
    if ( m_nodesAndLinksProtected ) { return false; }

    // Check node's existence in the graph
    if ( findNode( node ) == -1 ) { return false; }

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
    if ( found == -1 ) { return false; }

    node->getInputs()[found]->disconnect();
    m_ready         = false;
    m_shouldBeSaved = true;
    return true;
}

int DataflowGraph::findNode( const Node* node ) const {
    auto foundIt = std::find_if(
        m_nodes.begin(), m_nodes.end(), [node]( const auto& p ) { return *p == *node; } );
    if ( foundIt != m_nodes.end() ) { return std::distance( m_nodes.begin(), foundIt ); }
    else {
        return -1;
    }
}

bool DataflowGraph::compile() {
    // Find useful nodes (directly or indirectly connected to a Sink)
    std::unordered_map<Node*, std::pair<int, std::vector<Node*>>> infoNodes;
    for ( auto const& n : m_nodes ) {
        // Find all sinks
        if ( n->getOutputs().size() == 0 ) {
            // Add the sink in the useful nodes set if any of his port is linked
            bool activeSink { false };
            for ( const auto& p : n->getInputs() ) {
                activeSink |= p->isLinked();
            }
            if ( activeSink ) {
                infoNodes.emplace( n.get(), std::pair<int, std::vector<Node*>>( 0, {} ) );
                // recursively add the predecessors of the sink
                backtrackGraph( n.get(), infoNodes );
            }
        }
    }
    // Compute the level (rank of execution) of useful nodes
    int maxLevel = 0;
    for ( auto& infNode : infoNodes ) {
        auto n = infNode.first;
        // Compute the nodes' level starting from sources
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
                    return m_ready = false;
                }
            }
        }
    }
    m_ready = true;
    init();
    return m_ready;
}

void DataflowGraph::clearNodes() {
    for ( size_t i = 0; i < m_nodesByLevel.size(); i++ ) {
        m_nodesByLevel[i].clear();
        m_nodesByLevel[i].shrink_to_fit();
    }
    m_nodesByLevel.clear();
    m_nodesByLevel.shrink_to_fit();
    m_nodes.erase( m_nodes.begin(), m_nodes.end() );
    m_nodes.shrink_to_fit();
    m_shouldBeSaved = true;
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

bool DataflowGraph::addSetter( PortBase* in ) {
    addInput( in );
    if ( m_dataSetters.find( in->getName() ) == m_dataSetters.end() ) {
        auto portOut = std::shared_ptr<PortBase>( in->reflect( this, in->getName() ) );
        m_dataSetters.emplace( std::make_pair(
            in->getName(),
            DataSetter { DataSetterDesc { portOut, portOut->getName(), portOut->getTypeName() },
                         in } ) );
        return true;
    }
    return false;
}

inline bool DataflowGraph::addGetter( PortBase* out ) {
    if ( out->is_input() ) { return false; }
    // This is very similar to addOutput, except the data can't be set, they will be in the init
    // of any Sink
    bool found = false;
    // TODO check if this verification is needed ?
    for ( auto& output : m_outputs ) {
        if ( output->getName() == out->getName() ) { found = true; }
    }
    if ( !found ) { m_outputs.emplace_back( out ); }
    return !found;
}

bool DataflowGraph::releaseDataSetter( const std::string& portName ) {
    auto setter = m_dataSetters.find( portName );
    if ( setter != m_dataSetters.end() ) {
        auto [desc, in] = setter->second;
        in->disconnect();
        return true;
    }
    return false;
}

// Why is this method useful if it is the same than getDataSetter ?
bool DataflowGraph::activateDataSetter( const std::string& portName ) {
    return getDataSetter( portName ) != nullptr;
}

std::shared_ptr<PortBase> DataflowGraph::getDataSetter( const std::string& portName ) {
    auto setter = m_dataSetters.find( portName );
    if ( setter != m_dataSetters.end() ) {
        auto [desc, in] = setter->second;
        auto p          = std::get<0>( desc );
        in->disconnect();
        p->connect( in );
        return p;
    }
    return nullptr;
}

std::vector<DataflowGraph::DataSetterDesc> DataflowGraph::getAllDataSetters() {
    std::vector<DataflowGraph::DataSetterDesc> r;
    r.reserve( m_dataSetters.size() );
    for ( auto& s : m_dataSetters ) {
        r.push_back( s.second.first );
    }
    return r;
}

PortBase* DataflowGraph::getDataGetter( const std::string& portName ) {
    auto portIt = std::find_if( m_outputs.begin(), m_outputs.end(), [portName]( const auto& p ) {
        return p->getName() == portName;
    } );
    if ( portIt != m_outputs.end() ) { return portIt->get(); }
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

Node* DataflowGraph::getNode( const std::string& instanceNameNode ) const {
    auto nodeIt =
        std::find_if( m_nodes.begin(), m_nodes.end(), [instanceNameNode]( const auto& n ) {
            return n->getInstanceName() == instanceNameNode;
        } );
    if ( nodeIt != m_nodes.end() ) { return nodeIt->get(); }
    LOG( logERROR ) << "DataflowGraph::getNode : The node with the instance name "
                    << instanceNameNode << " has not been found";
    return nullptr;
}

DataflowGraph* DataflowGraph::loadGraphFromJsonFile( const std::string& filename ) {
    if ( !nlohmann::json::accept( std::ifstream( filename ) ) ) {
        LOG( logERROR ) << filename << " is not a valid json file !!";
        return nullptr;
    }

    std::ifstream jsonFile( filename );
    nlohmann::json j;
    jsonFile >> j;

    bool valid = false;
    if ( j.contains( "model" ) ) {
        if ( j["model"].contains( "name" ) && j["model"].contains( "instance" ) ) { valid = true; }
    }
    if ( !valid ) {
        LOG( logERROR ) << "loadGraphFromJsonFile :" << filename
                        << " does not contain a valid json NodeGraph\n";
        return nullptr;
    }
    std::string instanceName = j["model"]["instance"];
    std::string graphType    = j["model"]["name"];
    LOG( logINFO ) << "Loading the graph " << instanceName << ", with type " << graphType << "\n";

    auto& fctMngr = Ra::Dataflow::Core::NodeFactoriesManager::getFactoryManager();
    auto ndldd    = fctMngr.createNode( graphType, j );
    if ( ndldd == nullptr ) {
        LOG( logERROR ) << "Unable to load a graph with type " << graphType << "\n";
        return nullptr;
    }

    auto graph = dynamic_cast<DataflowGraph*>( ndldd );
    if ( graph != nullptr ) {
        graph->m_shouldBeSaved = false;
        return graph;
    }

    LOG( logERROR ) << "Loaded graph not inheriting from DataflowGraph " << graphType << "\n";
    delete ndldd;
    return nullptr;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
