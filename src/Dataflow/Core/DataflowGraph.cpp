#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Port.hpp>

#include <fstream>
#include <map>

#include <Core/Utils/Log.hpp>
#include <memory>

namespace Ra {
namespace Dataflow {
namespace Core {

RA_SINGLETON_IMPLEMENTATION( PortFactory );

using namespace Ra::Core::Utils;

DataflowGraph::DataflowGraph( const std::string& name ) : DataflowGraph( name, getTypename() ) {}

DataflowGraph::DataflowGraph( const std::string& instanceName, const std::string& typeName ) :
    Node( instanceName, typeName ) {}

void DataflowGraph::init() {
    if ( m_ready ) {
        Node::init();
        std::for_each( m_nodesByLevel.begin(), m_nodesByLevel.end(), []( const auto& level ) {
            std::for_each( level.begin(), level.end(), []( auto node ) {
                if ( !node->isInitialized() ) { node->init(); }
            } );
        } );
    }
}

bool DataflowGraph::execute() {
    if ( m_inputs.size() > 0 || m_outputs.size() > 0 ) return true;

    if ( !m_ready ) {
        if ( !compile() ) { return false; }
    }
    bool result = true;
    std::cerr << "exec " << display_name() << "\n";
    std::for_each( m_nodesByLevel.begin(), m_nodesByLevel.end(), [&result]( const auto& level ) {
        std::for_each( level.begin(), level.end(), [&result]( auto node ) {
            std::cerr << "exec " << node->display_name() << "\n";

            bool executed = node->execute();
            if ( !executed ) {
                LOG( logERROR ) << "Execution failed with node " << node->getInstanceName() << " ("
                                << node->getModelName() << ").";
            }
            result = result && executed;
        } );
    } );
    return result;
}

void DataflowGraph::destroy() {
    std::for_each(
        m_nodesByLevel.begin(), m_nodesByLevel.end(), []( auto& level ) { level.clear(); } );
    m_nodesByLevel.clear();
    m_nodes.clear();
    Node::destroy();
    needsRecompile();
}

void DataflowGraph::saveToJson( const std::string& jsonFilePath ) {
    if ( !jsonFilePath.empty() ) {
        nlohmann::json data;
        std::cerr << "DatafloweGraph::saveToJson instance " << m_instanceName << "\n";
        toJson( data );
        std::ofstream file( jsonFilePath );
        file << std::setw( 4 ) << data << std::endl;
        m_shouldBeSaved = false;
    }
}

void DataflowGraph::toJsonInternal( nlohmann::json& data ) const {
    nlohmann::json nodes       = nlohmann::json::array();
    nlohmann::json connections = nlohmann::json::array();
    nlohmann::json graph;

    for ( const auto& n : m_nodes ) {
        nlohmann::json nodeData;
        n->toJson( nodeData );
        nodes.push_back( nodeData );
        // skip input_node's input connection
        if ( n != m_input_node ) {
            for ( const auto& input : n->getInputs() ) {
                if ( input->isLinked() ) {
                    nlohmann::json link = nlohmann::json::object();
                    auto portOut        = input->getLink();
                    auto nodeOut        = portOut->getNode();
                    if ( auto casted = dynamic_cast<GraphOutputNode*>( nodeOut ); casted ) {
                        nodeOut = casted->graph();
                    }

                    link["out_node"] = nodeOut->getInstanceName();
                    link["out_port"] = portOut->getName();
                    link["in_node"]  = n->getInstanceName();
                    link["in_port"]  = input->getName();
                    connections.push_back( link );
                }
            }
        }
    }

    // write the common content of the Node to the json data
    graph["nodes"]       = nodes;
    graph["connections"] = connections;
    // Fill the specific concrete node information
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

std::pair<std::shared_ptr<Node>, std::string>
getLinkInfo( const std::string& which,
             const nlohmann::json& linkData,
             const std::map<std::string, std::shared_ptr<Node>>& nodeByName ) {
    std::string field = which + "_node";
    std::shared_ptr<Node> node { nullptr };

    auto itNode = nodeByName.find( linkData[field] );
    if ( itNode != nodeByName.end() ) { node = itNode->second; }
    else {
        // Error, could not find the node
        std::string msg = std::string { "Node " } + which + " named " +
                          std::string( linkData[field] ) + " not found in cache " + " : " +
                          linkData.dump();
        return { nullptr, msg };
    }

    std::string port;
    field = which + "_port";
    if ( linkData.contains( field ) ) {
        auto p = node->getPortByName( which, linkData[field] ).second;
        if ( p != nullptr ) { port = p->getName(); }
    }
    else {
        field = which + "_index";
        if ( linkData.contains( field ) ) {
            auto p = node->getPortByIndex( which, Node::PortIndex { int { linkData[field] } } );
            if ( p != nullptr ) { port = p->getName(); }
        }
    }
    if ( port.empty() ) {
        std::string msg = std::string { "Port " } + which + " not found in node " +
                          node->getInstanceName() + " : " + linkData.dump();
        return { nullptr, msg };
    }
    return { node, port };
}

bool DataflowGraph::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "graph" ) ) {
        // indicate that the graph must be recompiled after loading
        needsRecompile();
        // load the graph
        auto factories = NodeFactoriesManager::getFactoryManager();

        std::map<std::string, std::shared_ptr<Node>> nodeByName;
        if ( auto nodes_itr = data["graph"].find( "nodes" ); nodes_itr != data["graph"].end() ) {
            auto nodes = *nodes_itr;
            for ( auto& n : nodes ) {
                if ( !n["model"].contains( "name" ) ) {
                    LOG( logERROR ) << "Found a node without model description." << n.dump()
                                    << "Unable to build an instance.";
                    return false;
                }
                std::string nodeTypeName = n["model"]["name"];
                std::string instanceName;

                if ( auto instance_itr = n.find( "instance" ); instance_itr != n.end() ) {
                    instanceName = *instance_itr;
                }
                else {
                    LOG( logERROR )
                        << "Found a node of type " << nodeTypeName << " without identification ";
                    return false;
                }
                // create and adds node to this
                auto newNode = factories.createNode( nodeTypeName, n, this );
                if ( newNode ) {
                    if ( !instanceName.empty() ) {
                        auto [it, inserted] = nodeByName.insert( { instanceName, newNode } );
                        if ( !inserted ) {
                            LOG( logERROR ) << "DataflowGraph::loadFromJson : duplicated node name "
                                            << nodeTypeName;
                            return false;
                        }
                    }
                    if ( nodeTypeName == GraphInputNode::getTypename() ) {
                        m_input_node = std::dynamic_pointer_cast<GraphInputNode>( newNode );
                        m_input_node->set_graph( this );
                    }
                    if ( nodeTypeName == GraphOutputNode::getTypename() ) {
                        m_output_node = std::dynamic_pointer_cast<GraphOutputNode>( newNode );
                        m_output_node->set_graph( this );
                    }
                }
                else {
                    LOG( logERROR ) << "Unable to create the node " << nodeTypeName;
                    return false;
                }
            }
        }
        if ( auto links_itr = data["graph"].find( "connections" );
             links_itr != data["graph"].end() ) {
            auto links = *links_itr;
            for ( auto& l : links ) {
                auto [nodeFrom, fromOutput] = getLinkInfo( "out", l, nodeByName );
                if ( nodeFrom == nullptr ) {
                    LOG( logERROR ) << "DataflowGraph::loadFromJson: error when parsing JSON."
                                    << " Could not find the link source (" << fromOutput
                                    << "). Link not added.";
                    return false;
                }
                auto [nodeTo, toInput] = getLinkInfo( "in", l, nodeByName );
                if ( nodeTo == nullptr ) {
                    LOG( logERROR )
                        << "DataflowGraph::loadFromJson: error when parsing JSON."
                        << " Could not find the link target (" << toInput << "). Link not added.";
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
        generate_ports();
    }
    return true;
}

bool DataflowGraph::canAdd( const Node* newNode ) const {
    return findNode( newNode ) == -1;
}

bool DataflowGraph::addNode( std::shared_ptr<Node> newNode ) {
    // Check if the new node already exists (= same name and type)
    if ( canAdd( newNode.get() ) ) {
        m_nodes.emplace_back( std::move( newNode ) );
        needsRecompile();
        return true;
    }
    else { return false; }
}

bool DataflowGraph::removeNode( std::shared_ptr<Node> node ) {
    // This is to prevent graph destruction from the graph editor, depending on how it is used
    if ( m_nodesAndLinksProtected ) { return false; }

    // Check if the node is in the list already exists (= same name)
    int index = -1;
    if ( ( index = findNode( node.get() ) ) == -1 ) { return false; }
    else {
        m_nodes.erase( m_nodes.begin() + index );
        needsRecompile();
        return true;
    }
}

bool DataflowGraph::checkPortCompatibility( const Node* nodeFrom,
                                            Node::PortIndex portOutIdx,
                                            const PortBaseOut* portOut,
                                            const Node* nodeTo,
                                            Node::PortIndex portInIdx,
                                            const PortBaseIn* portIn ) {
    // Compare types
    if ( !( portIn->getType() == portOut->getType() ) ) {
        Log::addLinkTypeMismatch( nodeFrom, portOutIdx, portOut, nodeTo, portInIdx, portIn );
        return false;
    }

    // Check if input is connected
    if ( portIn->isLinked() ) {
        Log::alreadyLinked( nodeTo, portIn );
        return false;
    }
    return true;
}

void nodeNotFoundMessage( const std::string& type, const std::string& name, const Node* node ) {
    LOG( logERROR ) << "DataflowGraph::addLink Unable to find " << type << "input port " << name
                    << " from destination node " << node->getInstanceName() << " ("
                    << node->getModelName() << ")";
}

bool DataflowGraph::addLink( const std::shared_ptr<Node>& nodeFrom,
                             const std::string& nodeFromOutputName,
                             const std::shared_ptr<Node>& nodeTo,
                             const std::string& nodeToInputName ) {
    if ( !checkNodeValidity( nodeFrom.get(), nodeTo.get() ) ) { return false; }

    auto [inputIdx, inputPort] = nodeTo->getInputByName( nodeToInputName );
    if ( !inputPort ) {
        nodeNotFoundMessage( "input", nodeToInputName, nodeTo.get() );
        return false;
    }
    auto [outputIdx, outputPort] = nodeFrom->getOutputByName( nodeFromOutputName );
    if ( !outputPort ) {
        nodeNotFoundMessage( "output", nodeFromOutputName, nodeFrom.get() );
        return false;
    }

    // Compare types
    if ( !checkPortCompatibility(
             nodeFrom.get(), outputIdx, outputPort, nodeTo.get(), inputIdx, inputPort ) ) {
        return false;
    }

    // port can be connected
    inputPort->connect( outputPort );
    // The state of the graph changes, set it to not ready
    needsRecompile();
    return true;
}

bool DataflowGraph::addLink( const std::shared_ptr<Node>& nodeFrom,
                             Node::PortIndex portOutIdx,
                             const std::shared_ptr<Node>& nodeTo,
                             Node::PortIndex portInIdx ) {
    // here
    if ( ( nodeFrom == m_input_node || nodeFrom == m_output_node ) &&
         ( nodeTo == m_input_node || nodeTo == m_output_node ) )
        return false;

    if ( nodeFrom == m_input_node && portOutIdx == m_input_node->getOutputs().size() ) {
        auto portIn = nodeTo->getInputByIndex( portInIdx );
        if ( !portIn ) {
            Log::badPortIdx( "input", nodeTo->getModelName(), portInIdx );
            return false;
        }

        m_input_node->add_output_port( portIn );
        return true;
    }
    if ( nodeTo == m_output_node && portInIdx == m_output_node->getInputs().size() ) {

        auto portOut = nodeFrom->getOutputByIndex( portOutIdx );

        if ( !portOut ) {
            Log::badPortIdx( "output", nodeFrom->getModelName(), portOutIdx );
            return false;
        }

        m_output_node->add_input_port( portOut );
        return true;
    }

    if ( !checkNodeValidity( nodeFrom.get(), nodeTo.get() ) ) { return false; }

    auto portOut = nodeFrom->getOutputByIndex( portOutIdx );
    auto portIn  = nodeTo->getInputByIndex( portInIdx );

    if ( !portOut ) {
        Log::badPortIdx( "output", nodeFrom->getModelName(), portOutIdx );
        return false;
    }
    if ( !portIn ) {
        Log::badPortIdx( "input", nodeTo->getModelName(), portInIdx );
        return false;
    }

    // Compare types
    if ( !checkPortCompatibility(
             nodeFrom.get(), portOutIdx, portOut, nodeTo.get(), portInIdx, portIn ) )
        return false;

    // port can be connected
    portIn->connect( portOut );
    // The state of the graph changes, set it to not ready
    needsRecompile();
    return true;
}

bool DataflowGraph::addLink( Node::PortBaseOutRawPtr outputPort,
                             Node::PortBaseInRawPtr inputPort ) {
    auto nodeFrom = outputPort->getNode();
    auto nodeTo   = inputPort->getNode();
    if ( !checkNodeValidity( nodeFrom, nodeTo ) ) { return false; }
    // Compare types
    if ( !checkPortCompatibility(
             nodeFrom, Node::PortIndex {}, outputPort, nodeTo, Node::PortIndex {}, inputPort ) ) {
        return false;
    }
    // port can be connected
    inputPort->connect( outputPort );
    // The state of the graph changes, set it to not ready
    needsRecompile();
    return true;
}

bool DataflowGraph::removeLink( std::shared_ptr<Node> node, const std::string& nodeInputName ) {
    // This is to prevent graph destruction from the graph editor, depending on how it is used
    if ( m_nodesAndLinksProtected ) { return false; }

    // Check node's existence in the graph
    if ( findNode( node.get() ) == -1 ) { return false; }

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
    removeLink( node, found );
    return true;
}

int DataflowGraph::findNode( const Node* node ) const {
    auto foundIt = std::find_if(
        m_nodes.begin(), m_nodes.end(), [node]( const auto& p ) { return *p == *node; } );
    if ( foundIt != m_nodes.end() ) { return std::distance( m_nodes.begin(), foundIt ); }
    else { return -1; }
}

bool DataflowGraph::findNode2( const Node* node ) const {
    if ( !node ) return false;

    for ( const auto& n : m_nodes ) {
        if ( n.get() == node ) return true;
        auto g = dynamic_cast<DataflowGraph*>( n.get() );
        if ( g ) {
            if ( g->findNode2( node ) ) return true;
        }
    }
    return false;
}
void DataflowGraph::generate_ports() {
    if ( m_input_node ) m_inputs = m_input_node->getInputs();
    if ( m_output_node ) m_outputs = m_output_node->getOutputs();
}

bool DataflowGraph::compile() {

    // Find useful nodes (directly or indirectly connected to a Sink)
    std::unordered_map<Node*, std::pair<int, std::vector<Node*>>> infoNodes;

    if ( m_output_node ) {
        backtrackGraph( m_output_node.get(), infoNodes );
        infoNodes.emplace( m_output_node.get(), std::pair<int, std::vector<Node*>>( 0, {} ) );
    }
    for ( auto const& n : m_nodes ) {
        // Find all active sinks, skip m_output_node
        if ( n->isOutputNode() && n != m_output_node ) {
            // if a linked port exists, backtrace
            if ( std::any_of( n->getInputs().begin(), n->getInputs().end(), []( const auto& p ) {
                     return p->isLinked();
                 } ) ) {

                infoNodes.emplace( n.get(), std::pair<int, std::vector<Node*>>( 0, {} ) );
                // recursively add the predecessors of the sink
                backtrackGraph( n.get(), infoNodes );
            }
            else {
                LOG( logWARNING ) << "Sink Node " << n->getInstanceName()
                                  << " is inactive (belog to the graph but not connected)";
            }
        }
    }
    // Compute the level (rank of execution) of useful nodes
    int maxLevel = 0;
    for ( auto& infNode : infoNodes ) {
        auto n = infNode.first;
        // Compute the nodes' level starting from sources
        if ( n->isInputNode() || n == m_input_node.get() ) {
            // set level to 0 because node is source
            infNode.second.first = 0;
            // Tag successors
            for ( auto const successor : infNode.second.second ) {
                // Successors is a least +1 level
                infoNodes[successor].first =
                    std::max( infoNodes[successor].first, infNode.second.first + 1 );
                maxLevel = std::max( maxLevel,
                                     std::max( infoNodes[successor].first,
                                               goThroughGraph( successor, infoNodes ) ) );
            }
        }
    }
    m_nodesByLevel.clear();
    m_nodesByLevel.resize( infoNodes.size() != 0 ? maxLevel + 1 : 0 );
    for ( auto& infNode : infoNodes ) {
        if ( size_t( infNode.second.first ) >= m_nodesByLevel.size() ) {
            LOG( logERROR ) << "Node " << infNode.first->getInstanceName() << " is at level "
                            << infNode.second.first << " but level max is " << maxLevel;
            std::abort();
        }
        m_nodesByLevel[infNode.second.first].push_back( infNode.first );
    }

    // For each level
    for ( auto& lvl : m_nodesByLevel ) {
        // For each node
        for ( size_t j = 0; j < lvl.size(); j++ ) {
            if ( !lvl[j]->compile() ) { return m_ready = false; }
            // For each input
            for ( size_t k = 0; k < lvl[j]->getInputs().size(); k++ ) {
                if ( lvl[j] != m_input_node.get() && lvl[j]->getInputs()[k]->isLinkMandatory() &&
                     !lvl[j]->getInputs()[k]->isLinked() ) {
                    LOG( logERROR )
                        << "Node <" << lvl[j]->getInstanceName() << "> is not ready" << std::endl;
                    return m_ready = false;
                }
            }
        }
    }
    generate_ports();
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
    m_inputs.erase( m_inputs.begin(), m_inputs.end() );
    m_inputs.shrink_to_fit();
    m_outputs.erase( m_outputs.begin(), m_outputs.end() );
    m_outputs.shrink_to_fit();
    m_shouldBeSaved = true;
}

void DataflowGraph::backtrackGraph(
    Node* current,
    std::unordered_map<Node*, std::pair<int, std::vector<Node*>>>& infoNodes ) {
    std::cerr << "backtrace " << current->display_name() << "\n";
    for ( auto& input : current->getInputs() ) {
        std::cerr << "input " << input->getName() << "\n";
        if ( input->getLink() ) {
            Node* previous = input->getLink()->getNode();
            if ( previous && previous != m_input_node.get() ) {
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
            maxLevel = std::max(
                maxLevel,
                std::max( infoNodes[successor].first, goThroughGraph( successor, infoNodes ) ) );
        }
    }
    return maxLevel;
}

std::shared_ptr<Node> DataflowGraph::getNode( const std::string& instanceNameNode ) const {
    auto nodeIt =
        std::find_if( m_nodes.begin(), m_nodes.end(), [instanceNameNode]( const auto& n ) {
            return n->getInstanceName() == instanceNameNode;
        } );
    if ( nodeIt != m_nodes.end() ) { return *nodeIt; }
    LOG( logERROR ) << "DataflowGraph::getNode : The node with the instance name \""
                    << instanceNameNode << "\" has not been found";
    return { nullptr };
}

std::shared_ptr<DataflowGraph> DataflowGraph::loadGraphFromJsonFile( const std::string& filename ) {
    if ( !nlohmann::json::accept( std::ifstream( filename ) ) ) {
        LOG( logERROR ) << filename << " is not a valid json file !!";
        return nullptr;
    }

    std::ifstream jsonFile( filename );
    nlohmann::json j;
    jsonFile >> j;

    bool valid = false;
    if ( j.contains( "instance" ) && j.contains( "model" ) ) {
        valid = j["model"].contains( "name" );
    }
    if ( !valid ) {
        LOG( logERROR ) << "loadGraphFromJsonFile :" << filename
                        << " does not contain a valid json NodeGraph\n";
        return nullptr;
    }
    std::string instanceName = j["instance"];
    std::string graphType    = j["model"]["name"];
    LOG( logINFO ) << "Loading the graph " << instanceName << ", with type " << graphType << "\n";

    auto& factories = Ra::Dataflow::Core::NodeFactoriesManager::getFactoryManager();
    auto node       = factories.createNode( graphType, j );
    if ( node == nullptr ) {
        LOG( logERROR ) << "Unable to load a graph with type " << graphType << "\n";
        return nullptr;
    }

    auto graph = std::dynamic_pointer_cast<DataflowGraph>( node );
    if ( graph != nullptr ) {
        graph->m_shouldBeSaved = false;
        return graph;
    }

    LOG( logERROR ) << "Loaded graph failed (not derived from DataflowGraph) " << graphType << "\n";

    return nullptr;
}

bool DataflowGraph::checkNodeValidity( const Node* nodeFrom, const Node* nodeTo ) {
    using namespace Ra::Core::Utils; // Check node "from" existence in the graph
    if ( !findNode2( nodeFrom ) ) {
        Log::unableToFind( "initial node", nodeFrom->getInstanceName() );
        return false;
    }

    // Check node "to" existence in the graph
    if ( !findNode2( nodeTo ) ) {
        Log::unableToFind( "destination node", nodeTo->getInstanceName() );
        return false;
    }
    return true;
}

void DataflowGraph::Log::alreadyLinked( const Node* node, const PortBase* port ) {
    LOG( logERROR ) << "DataflowGraph::addLink destination port not available (already linked) for "
                    << node->getInstanceName() << " (" << node->getModelName() << "), port "
                    << port->getName();
}

void DataflowGraph::Log::addLinkTypeMismatch( const Node* nodeFrom,
                                              Node::PortIndex portOutIdx,
                                              const PortBase* portOut,
                                              const Node* nodeTo,
                                              Node::PortIndex portInIdx,
                                              const PortBase* portIn ) {
    LOG( logERROR ) << "DataflowGraph::addLink type mismatch from " << nodeFrom->getInstanceName()
                    << " (" << nodeFrom->getModelName() << ") / " << portOut->getName() << " ("
                    << portOutIdx << " with type " << portOut->getTypeName() << ")"
                    << " to " << nodeTo->getInstanceName() << " (" << nodeTo->getModelName()
                    << ") / " << portIn->getName() << " (" << portInIdx << " with type "
                    << portIn->getTypeName() << ") ";
}

void DataflowGraph::Log::unableToFind( const std::string& type, const std::string& instanceName ) {
    LOG( logERROR ) << "DataflowGraph::addLink Unable to find " << type << " " << instanceName;
}

void DataflowGraph::Log::badPortIdx( const std::string& type,
                                     const std::string& instanceName,
                                     Node::PortIndex idx ) {
    LOG( logERROR ) << "DataflowGraph::addLink node " << instanceName << " as no " << type
                    << " port with index " << idx;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
