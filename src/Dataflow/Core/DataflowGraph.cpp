
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

DataflowGraph::DataflowGraph( const std::string& name ) : DataflowGraph( name, node_typename() ) {}

DataflowGraph::DataflowGraph( const std::string& instanceName, const std::string& typeName ) :
    Node( instanceName, typeName ) {}

void DataflowGraph::init() {
    if ( m_ready ) {
        Node::init();
        std::for_each( m_nodes_by_level.begin(), m_nodes_by_level.end(), []( const auto& level ) {
            std::for_each( level.begin(), level.end(), []( auto node ) {
                if ( !node->is_initialized() ) { node->init(); }
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
    std::for_each(
        m_nodes_by_level.begin(), m_nodes_by_level.end(), [&result]( const auto& level ) {
            std::for_each( level.begin(), level.end(), [&result]( auto node ) {
                bool executed = node->execute();
                if ( !executed ) {
                    LOG( logERROR ) << "Execution failed with node " << node->instance_name()
                                    << " (" << node->model_name() << ").";
                }
                result = result && executed;
            } );
        } );
    return result;
}

void DataflowGraph::destroy() {
    std::for_each(
        m_nodes_by_level.begin(), m_nodes_by_level.end(), []( auto& level ) { level.clear(); } );
    m_nodes_by_level.clear();
    m_nodes.clear();
    Node::destroy();
    needs_recompile();
}

void DataflowGraph::saveToJson( const std::string& jsonFilePath ) {
    if ( !jsonFilePath.empty() ) {
        nlohmann::json data;
        toJson( data );
        std::ofstream file( jsonFilePath );
        file << std::setw( 4 ) << data << std::endl;
        m_should_save = false;
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
            for ( const auto& input : n->inputs() ) {
                if ( input->isLinked() ) {
                    nlohmann::json link = nlohmann::json::object();
                    auto portOut        = input->getLink();
                    auto nodeOut        = portOut->node();
                    if ( auto casted = dynamic_cast<GraphOutputNode*>( nodeOut ); casted ) {
                        nodeOut = casted->graph();
                    }

                    link["out_node"] = nodeOut->instance_name();
                    link["out_port"] = portOut->getName();
                    link["in_node"]  = n->instance_name();
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

std::optional<nlohmann::json> read_json( const std::string& jsonFilePath ) {
    std::ifstream f( jsonFilePath );
    nlohmann::json j = nlohmann::json::parse( f, nullptr, false );

    if ( j.is_discarded() ) {
        LOG( logERROR ) << jsonFilePath << " is not a valid json file !!";
        return std::nullopt;
    }
    return j;
}

bool DataflowGraph::loadFromJson( const std::string& jsonFilePath ) {
    auto j = read_json( jsonFilePath );
    if ( !j ) return false;

    m_should_save = false;
    return fromJson( *j );
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
    std::string err;

    field = which + "_port";
    if ( linkData.contains( field ) ) {
        auto p = node->port_by_name( which, linkData[field] ).second;
        if ( p != nullptr ) { port = p->getName(); }
        else { err = linkData[field]; }
    }
    else {
        field = which + "_index";
        if ( linkData.contains( field ) ) {
            auto p = node->port_by_index( which, Node::PortIndex { int { linkData[field] } } );
            if ( p != nullptr ) { port = p->getName(); }
            else { err = std::to_string( int { linkData[field] } ); }
        }
    }
    if ( port.empty() ) {
        std::string msg = std::string { "Port " } + which + " " + err + " not found in node " +
                          node->instance_name() + " : " + linkData.dump();
        return { nullptr, msg };
    }
    return { node, port };
}

bool DataflowGraph::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "graph" ) ) {
        // indicate that the graph must be recompiled after loading
        needs_recompile();
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
                    if ( nodeTypeName == GraphInputNode::node_typename() ) {
                        m_input_node = std::dynamic_pointer_cast<GraphInputNode>( newNode );
                        m_input_node->set_graph( this );
                    }
                    if ( nodeTypeName == GraphOutputNode::node_typename() ) {
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
                if ( !add_link( nodeFrom, fromOutput, nodeTo, toInput ) ) {
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

bool DataflowGraph::add_node( std::shared_ptr<Node> newNode ) {
    // Check if the new node already exists (= same name and type)
    if ( !has_node_by_name( newNode->instance_name(), newNode->model_name() ) ) {
        m_nodes.emplace_back( std::move( newNode ) );
        needs_recompile();
        return true;
    }
    else { return false; }
}

bool DataflowGraph::remove_node( std::shared_ptr<Node> node ) {
    // This is to prevent graph destruction from the graph editor, depending on how it is used
    if ( m_nodesAndLinksProtected ) { return false; }

    if ( auto itr = std::find( m_nodes.begin(), m_nodes.end(), node ); itr != m_nodes.end() ) {
        m_nodes.erase( itr );
        needs_recompile();
        return true;
    }
    return false;
}

bool DataflowGraph::are_ports_compatible( const Node* nodeFrom,
                                          const PortBaseOut* portOut,
                                          const Node* nodeTo,
                                          const PortBaseIn* portIn ) {
    // Compare types
    if ( !( portIn->getType() == portOut->getType() ) ) {
        Log::link_type_mismatch( nodeFrom, portOut, nodeTo, portIn );
        return false;
    }

    // Check if input is connected
    if ( portIn->isLinked() ) {
        Log::already_linked( nodeTo, portIn );
        return false;
    }
    return true;
}

void nodeNotFoundMessage( const std::string& type, const std::string& name, const Node* node ) {
    LOG( logERROR ) << "DataflowGraph::add_link Unable to find " << type << "input port " << name
                    << " from destination node " << node->instance_name() << " ("
                    << node->model_name() << ")";
}

bool DataflowGraph::add_link( const std::shared_ptr<Node>& nodeFrom,
                              const std::string& nodeFromOutputName,
                              const std::shared_ptr<Node>& nodeTo,
                              const std::string& nodeToInputName ) {
    if ( !are_nodes_valids( nodeFrom.get(), nodeTo.get(), true ) ) { return false; }

    auto [inputIdx, inputPort] = nodeTo->input_by_name( nodeToInputName );
    if ( !inputPort ) {
        nodeNotFoundMessage( "input", nodeToInputName, nodeTo.get() );
        return false;
    }
    auto [outputIdx, outputPort] = nodeFrom->output_by_name( nodeFromOutputName );
    if ( !outputPort ) {
        nodeNotFoundMessage( "output", nodeFromOutputName, nodeFrom.get() );
        return false;
    }

    return add_link( outputPort, inputPort );
}

bool DataflowGraph::add_link( const std::shared_ptr<Node>& nodeFrom,
                              Node::PortIndex portOutIdx,
                              const std::shared_ptr<Node>& nodeTo,
                              Node::PortIndex portInIdx ) {

    if ( !are_nodes_valids( nodeFrom.get(), nodeTo.get(), true ) ) { return false; }
    if ( check_last_port_io_nodes( nodeFrom.get(), portOutIdx, nodeTo.get(), portInIdx ) ) {
        if ( m_input_node && nodeFrom == m_input_node &&
             portOutIdx == m_input_node->outputs().size() ) {
            auto portIn = nodeTo->input_by_index( portInIdx );
            if ( !portIn ) {
                Log::bad_port_index( "input", nodeTo->model_name(), portInIdx );
                return false;
            }
            auto idx = m_input_node->add_output_port( portIn );
            return idx.isValid();
        }
        if ( nodeTo && nodeTo == m_output_node && portInIdx == m_output_node->inputs().size() ) {
            auto portOut = nodeFrom->output_by_index( portOutIdx );
            if ( !portOut ) {
                Log::bad_port_index( "output", nodeFrom->model_name(), portOutIdx );
                return false;
            }
            auto idx = m_output_node->add_input_port( portOut );
            return idx.isValid();
        }
    }

    auto portOut = nodeFrom->output_by_index( portOutIdx );
    auto portIn  = nodeTo->input_by_index( portInIdx );

    if ( !portOut ) {
        Log::bad_port_index( "output", nodeFrom->model_name(), portOutIdx );
        return false;
    }
    if ( !portIn ) {
        Log::bad_port_index( "input", nodeTo->model_name(), portInIdx );
        return false;
    }

    return add_link( portOut, portIn );
}

bool DataflowGraph::add_link( Node::PortBaseOutRawPtr outputPort,
                              Node::PortBaseInRawPtr inputPort ) {
    auto nodeFrom = outputPort->node();
    auto nodeTo   = inputPort->node();
    if ( !are_nodes_valids( nodeFrom, nodeTo ) ) { return false; }
    // Compare types
    if ( !are_ports_compatible( nodeFrom, outputPort, nodeTo, inputPort ) ) { return false; }
    // port can be connected
    inputPort->connect( outputPort );
    // The state of the graph changes, set it to not ready
    needs_recompile();
    return true;
}

bool DataflowGraph::remove_link( std::shared_ptr<Node> node, const std::string& nodeInputName ) {
    auto [idx, port] = node->input_by_name( nodeInputName );
    return remove_link( node, idx );
}

bool DataflowGraph::remove_link( std::shared_ptr<Node> node, const PortIndex& in_port_index ) {
    // This is to prevent graph destruction from the graph editor, depending on how it is used
    if ( m_nodesAndLinksProtected ) { return false; }

    // Check node's existence in the graph
    bool ret = false;
    if ( // node in graph
        contains_node_recursive( node.get() ) &&
        // port index valid
        in_port_index.isValid() &&
        // port index less than input size
        static_cast<size_t>( in_port_index ) < node->inputs().size() ) {
        ret = node->inputs()[in_port_index]->disconnect();
        if ( ret ) needs_recompile();
    }
    return ret;
}

bool DataflowGraph::has_node_by_name( const std::string& instance,
                                      const std::string& model ) const {
    return std::find_if( m_nodes.begin(), m_nodes.end(), [instance, model]( const auto& p ) {
               return p->model_name() == model && p->instance_name() == instance;
           } ) != m_nodes.end();
}

bool DataflowGraph::contains_node_recursive( const Node* node ) const {
    if ( !node ) return false;
    for ( const auto& n : m_nodes ) {
        if ( n.get() == node ) return true;
        auto g = dynamic_cast<DataflowGraph*>( n.get() );
        if ( g ) {
            if ( g->contains_node_recursive( node ) ) return true;
        }
    }
    return false;
}

void DataflowGraph::generate_ports() {
    if ( m_input_node ) m_inputs = m_input_node->inputs();
    if ( m_output_node ) m_outputs = m_output_node->outputs();
}

bool DataflowGraph::compile() {

    // Find useful nodes (directly or indirectly connected to a Sink)

    /// Node -> level, linked nodes
    std::unordered_map<Node*, std::pair<int, std::vector<Node*>>> infoNodes;

    if ( m_output_node ) {
        backtrack_graph( m_output_node.get(), infoNodes );
        infoNodes.emplace( m_output_node.get(), std::pair<int, std::vector<Node*>>( 0, {} ) );
    }
    for ( auto const& n : m_nodes ) {
        // Find all active sinks, skip m_output_node
        if ( n->is_output() && n != m_output_node ) {
            // if a linked port exists, backtrace
            if ( std::any_of( n->inputs().begin(), n->inputs().end(), []( const auto& p ) {
                     return p->isLinked();
                 } ) ) {

                infoNodes.emplace( n.get(), std::pair<int, std::vector<Node*>>( 0, {} ) );
                // recursively add the predecessors of the sink
                backtrack_graph( n.get(), infoNodes );
            }
            else {
                LOG( logWARNING ) << "Sink Node " << n->instance_name()
                                  << " is inactive (belongs to the graph but not connected)";
            }
        }
    }
    // Compute the level (rank of execution) of useful nodes
    int maxLevel = 0;
    for ( auto& infNode : infoNodes ) {
        auto n = infNode.first;
        // Compute the nodes' level starting from sources
        if ( n->is_input() || n == m_input_node.get() ) {

            // set level to 0 because node is source
            infNode.second.first = 0;
            // Tag successors (go through graph)
            maxLevel = std::max( maxLevel, traverse_graph( n, infoNodes ) );
        }
    }
    m_nodes_by_level.clear();
    m_nodes_by_level.resize( infoNodes.size() != 0 ? maxLevel + 1 : 0 );
    for ( auto& infNode : infoNodes ) {
        CORE_ASSERT( size_t( infNode.second.first ) < m_nodes_by_level.size(),
                     std::string( "Node " ) + infNode.first->instance_name() + " is at level " +
                         std::to_string( infNode.second.first ) + " but level max is " +
                         std::to_string( maxLevel ) );

        m_nodes_by_level[infNode.second.first].push_back( infNode.first );
    }

    // For each level
    for ( auto& lvl : m_nodes_by_level ) {
        // For each node
        for ( size_t j = 0; j < lvl.size(); j++ ) {
            if ( !lvl[j]->compile() ) { return m_ready = false; }
            // For each input
            for ( size_t k = 0; k < lvl[j]->inputs().size(); k++ ) {
                if ( lvl[j] != m_input_node.get() && lvl[j]->inputs()[k]->isLinkMandatory() &&
                     !lvl[j]->inputs()[k]->isLinked() ) {
                    LOG( logERROR )
                        << "Node <" << lvl[j]->instance_name() << "> is not ready" << std::endl;
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

void DataflowGraph::clear_nodes() {
    for ( size_t i = 0; i < m_nodes_by_level.size(); i++ ) {
        m_nodes_by_level[i].clear();
        m_nodes_by_level[i].shrink_to_fit();
    }
    m_nodes_by_level.clear();
    m_nodes_by_level.shrink_to_fit();
    m_nodes.erase( m_nodes.begin(), m_nodes.end() );
    m_nodes.shrink_to_fit();
    m_inputs.erase( m_inputs.begin(), m_inputs.end() );
    m_inputs.shrink_to_fit();
    m_outputs.erase( m_outputs.begin(), m_outputs.end() );
    m_outputs.shrink_to_fit();
    m_should_save = true;
}

void DataflowGraph::backtrack_graph(
    Node* current,
    std::unordered_map<Node*, std::pair<int, std::vector<Node*>>>& infoNodes ) {
    for ( auto& input : current->inputs() ) {
        if ( input->getLink() ) {
            Node* previous = input->getLink()->node();
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
                    backtrack_graph( previous, infoNodes );
                }
            }
        }
    }
}

int DataflowGraph::traverse_graph(
    Node* current,
    std::unordered_map<Node*, std::pair<int, std::vector<Node*>>>& infoNodes ) {

    int maxLevel = 0;
    if ( infoNodes.find( current ) != infoNodes.end() ) {

        for ( auto const& successor : infoNodes[current].second ) {
            // Successors is a least +1 level
            infoNodes[successor].first =
                std::max( infoNodes[successor].first, infoNodes[current].first + 1 );
            maxLevel = std::max(
                maxLevel,
                std::max( infoNodes[successor].first, traverse_graph( successor, infoNodes ) ) );
        }
    }

    return maxLevel;
}

std::shared_ptr<Node> DataflowGraph::node( const std::string& instanceNameNode ) const {
    auto nodeIt =
        std::find_if( m_nodes.begin(), m_nodes.end(), [instanceNameNode]( const auto& n ) {
            return n->instance_name() == instanceNameNode;
        } );
    if ( nodeIt != m_nodes.end() ) { return *nodeIt; }
    LOG( logERROR ) << "DataflowGraph::node : The node with the instance name \""
                    << instanceNameNode << "\" has not been found";
    return { nullptr };
}

std::shared_ptr<DataflowGraph> DataflowGraph::loadGraphFromJsonFile( const std::string& filename ) {

    auto oj = read_json( filename );
    if ( !oj ) return nullptr;
    const auto& j = *oj;

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
        graph->m_should_save = false;
        return graph;
    }

    LOG( logERROR ) << "Loaded graph failed (not derived from DataflowGraph) " << graphType << "\n";

    return nullptr;
}

bool DataflowGraph::are_nodes_valids( const Node* nodeFrom,
                                      const Node* nodeTo,
                                      bool verbose ) const {
    using namespace Ra::Core::Utils;
    // Check node "from" existence in the graph
    if ( !contains_node_recursive( nodeFrom ) ) {
        if ( verbose ) Log::unable_to_find( "initial node", nodeFrom->instance_name() );
        return false;
    }

    // Check node "to" existence in the graph
    if ( !contains_node_recursive( nodeTo ) ) {
        if ( verbose ) Log::unable_to_find( "destination node", nodeTo->instance_name() );
        return false;
    }

    if ( ( nodeFrom == m_input_node.get() || nodeFrom == m_output_node.get() ) &&
         ( nodeTo == m_input_node.get() || nodeTo == m_output_node.get() ) ) {
        if ( verbose ) Log::try_to_link_input_to_output();
        return false;
    }
    return true;
}

void DataflowGraph::Log::already_linked( const Node* node, const PortBase* port ) {
    LOG( logERROR )
        << "DataflowGraph::add_link destination port not available (already linked) for "
        << node->instance_name() << " (" << node->model_name() << "), port " << port->getName();
}

void DataflowGraph::Log::link_type_mismatch( const Node* nodeFrom,
                                             const PortBase* portOut,
                                             const Node* nodeTo,
                                             const PortBase* portIn ) {
    LOG( logERROR ) << "DataflowGraph link type mismatch from " << nodeFrom->display_name() << " ("
                    << nodeFrom->model_name() << ") / " << portOut->getName() << " with type "
                    << portOut->getTypeName() << ")"
                    << " to " << nodeTo->display_name() << " (" << nodeTo->model_name() << ") / "
                    << portIn->getName() << " ( with type " << portIn->getTypeName() << ") ";
}

void DataflowGraph::Log::unable_to_find( const std::string& type,
                                         const std::string& instanceName ) {
    LOG( logERROR ) << "DataflowGraph::add_link Unable to find " << type << " " << instanceName;
}

void DataflowGraph::Log::bad_port_index( const std::string& type,
                                         const std::string& instanceName,
                                         Node::PortIndex idx ) {
    LOG( logERROR ) << "DataflowGraph::add_link node " << instanceName << " as no " << type
                    << " port with index " << idx;
}

void DataflowGraph::Log::try_to_link_input_to_output() {
    LOG( logERROR ) << "DataflowGraph could not link input to ouput directrly";
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
