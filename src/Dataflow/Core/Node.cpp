#include <Dataflow/Core/Node.hpp>

#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

using namespace Ra::Core::Utils;

bool Node::s_uuidGeneratorInitialized { false };
uuids::uuid_random_generator* Node::s_uidGenerator { nullptr };

void Node::createUuidGenerator() {
    std::random_device rd;
    auto seed_data = std::array<int, std::mt19937::state_size> {};
    std::generate( std::begin( seed_data ), std::end( seed_data ), std::ref( rd ) );
    std::seed_seq seq( std::begin( seed_data ), std::end( seed_data ) );
    auto generator             = new std::mt19937( seq );
    s_uidGenerator             = new uuids::uuid_random_generator( *generator );
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
    if ( data.contains( "model" ) ) {
        if ( data["model"].contains( "instance" ) ) { m_instanceName = data["model"]["instance"]; }
    }
    else {
        LOG( logERROR ) << "Missing required model when loading a Dataflow::Node";
        return false;
    }
    // get the common content of the Node from the json data
    if ( data.contains( "id" ) ) {
        std::string struuid = data["id"];
        m_uuid              = uuids::uuid::from_string( struuid ).value();
    }
    else {
        LOG( logERROR ) << "Missing required uuid when loading node " << m_instanceName;
        return false;
    }

    // get the specific concrete node information
    const auto& datamodel = data["model"];
    auto loaded           = fromJsonInternal( datamodel );

    // get the supplemental informations related to application/gui/...
    for ( auto& [key, value] : data.items() ) {
        if ( key != "id" && key != "model" ) { m_extraJsonData.emplace( key, value ); }
    }
    return loaded;
}

void Node::toJson( nlohmann::json& data ) const {
    // write the common content of the Node to the json data
    std::string struuid = "{" + uuids::to_string( m_uuid ) + "}";
    data["id"]          = struuid;

    nlohmann::json model;
    model["instance"] = m_instanceName;
    model["name"]     = m_typeName;

    // Fill the specific concrete node informations
    toJsonInternal( model );
    data.emplace( "model", model );

    // store the supplemental informations related to application/gui/...
    for ( auto& [key, value] : m_extraJsonData.items() ) {
        if ( key != "id" && key != "model" ) { data.emplace( key, value ); }
    }
}

void Node::addJsonMetaData( const nlohmann::json& data ) {
    for ( auto& [key, value] : data.items() ) {
        m_extraJsonData[key] = value;
    }
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
