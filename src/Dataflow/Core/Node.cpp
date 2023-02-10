#include <Dataflow/Core/Node.hpp>

#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

using namespace Ra::Core::Utils;

bool Node::s_uuidGeneratorInitialized { false };
std::unique_ptr<uuids::uuid_random_generator> Node::s_uidGenerator { nullptr };
std::unique_ptr<std::mt19937> Node::s_uuidSeeds { nullptr };

void Node::createUuidGenerator() {
    if ( s_uuidGeneratorInitialized ) { return; }
    std::random_device rd;
    auto seed_data = std::array<int, std::mt19937::state_size> {};
    std::generate( std::begin( seed_data ), std::end( seed_data ), std::ref( rd ) );
    std::seed_seq seq( std::begin( seed_data ), std::end( seed_data ) );
    s_uuidSeeds    = std::make_unique<std::mt19937>( seq );
    s_uidGenerator = std::make_unique<uuids::uuid_random_generator>( s_uuidSeeds.get() );
    // delete generator;
    s_uuidGeneratorInitialized = true;
}

/// Generates the uuid of the node
void Node::generateUuid() {
    if ( !s_uuidGeneratorInitialized ) { createUuidGenerator(); }
    m_uuid = ( *s_uidGenerator )();
}
/// Gets the UUID of the node as a string
std::string Node::getUuid() const {
    return std::string { "{" } + uuids::to_string( m_uuid ) + "}";
}

Node::Node( const std::string& instanceName, const std::string& typeName ) :
    m_typeName { typeName }, m_instanceName { instanceName } {
    generateUuid();
}

bool Node::fromJson( const nlohmann::json& data ) {
    if ( data.empty() ) {
        // This is to avoid wrong error message when creating node from the editor
        return true;
    }
    bool hasIdOrName = false;
    if ( data.contains( "instance" ) ) {
        hasIdOrName    = true;
        m_instanceName = data["instance"];
    }
    // get the common content of the Node from the json data
    if ( data.contains( "id" ) ) {
        hasIdOrName         = true;
        std::string struuid = data["id"];
        m_uuid              = uuids::uuid::from_string( struuid ).value();
    }
    if ( !hasIdOrName ) {
        LOG( logERROR ) << "Missing required uuid or instance name when loading node "
                        << m_instanceName;
        return false;
    }

    bool loaded = false;
    if ( data.contains( "model" ) ) {
        // get the specific concrete node information
        const auto& datamodel = data["model"];
        loaded                = fromJsonInternal( datamodel );
    }
    else {
        LOG( logERROR ) << "Missing required model when loading a Dataflow::Node";
        loaded = false;
    }
    // get the supplemental information related to application/gui/...
    for ( auto& [key, value] : data.items() ) {
        if ( key != "id" && key != "instance" && key != "model" ) {
            m_extraJsonData.emplace( key, value );
        }
    }
    return loaded;
}

void Node::toJson( nlohmann::json& data ) const {

    // write the common content of the Node to the json data
#if 0
    // id is only needed for QtNodeEditor, do not save it, it will be regenerated when needed
    std::string struuid = "{" + uuids::to_string( m_uuid ) + "}";
    data["id"]          = struuid;
#endif
    data["instance"] = m_instanceName;

    nlohmann::json model;
    model["name"] = m_typeName;

    // Fill the specific concrete node information (model instance)
    toJsonInternal( model );
    data.emplace( "model", model );

    // store the supplemental information related to application/gui/...
    for ( auto& [key, value] : m_extraJsonData.items() ) {
        if ( key != "id" && key != "instance" && key != "model" ) { data.emplace( key, value ); }
    }
}

void Node::addJsonMetaData( const nlohmann::json& data ) {
    for ( auto& [key, value] : data.items() ) {
        m_extraJsonData[key] = value;
    }
}

PortBase* Node::getPortByName( const std::string& type, const std::string& name ) const {
    const auto& ports = ( type == "in" ) ? m_inputs : m_outputs;
    auto itp          = std::find_if(
        ports.begin(), ports.end(), [n = name]( const auto& p ) { return p->getName() == n; } );
    PortBase* fprt { nullptr };
    if ( itp != ports.cend() ) { fprt = itp->get(); }
    return fprt;
}

PortBase* Node::getPortByIndex( const std::string& type, int idx ) const {
    const auto& ports = ( type == "in" ) ? m_inputs : m_outputs;
    if ( 0 <= idx && size_t( idx ) < ports.size() ) { return ports[idx].get(); }
    return nullptr;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
