#include <Core/Utils/Log.hpp>
#include <Dataflow/Core/Node.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
class PortBase;

using namespace Ra::Core::Utils;

// display_name is instanceName unless reset afterward
Node::Node( const std::string& instanceName, const std::string& typeName ) :
    m_model_name { typeName }, m_instance_name { instanceName }, m_display_name { instanceName } {}

bool Node::fromJson( const nlohmann::json& data ) {
    // This is to avoid wrong error message when creating node from the editor
    if ( data.empty() ) { return true; }

    auto it_instance = data.find( "instance" );
    if ( it_instance != data.end() ) { m_instance_name = *it_instance; }
    else {
        LOG( logERROR ) << "Missing required instance name when loading node " << m_instance_name;
        return false;
    }
    // get the common content of the Node from the json data
    bool loaded = false;

    auto it_model = data.find( "model" );
    if ( it_model != data.end() ) {
        ///\todo check model name ...
        // get the specific concrete node information
        const auto& datamodel = *it_model;
        loaded                = fromJsonInternal( datamodel );
        if ( !loaded ) { LOG( logERROR ) << "Fail to load model " << datamodel; }
        auto it_display_name = datamodel.find( "display_name" );
        if ( it_display_name != datamodel.end() ) { set_display_name( *it_display_name ); }
        else { set_display_name( m_instance_name ); }
    }
    else {
        LOG( logERROR ) << "Missing required model when loading a Dataflow::Node";
        loaded = false;
    }
    // get the supplemental information related to application/gui/...
    for ( auto& [key, value] : data.items() ) {
        if ( key != "instance" && key != "model" ) { m_metadata.emplace( key, value ); }
    }
    return loaded;
}

void Node::toJson( nlohmann::json& data ) const {

    // write the common content of the Node to the json data
    data["instance"] = m_instance_name;

    nlohmann::json model;
    model["name"]         = m_model_name;
    model["display_name"] = display_name();
    // Fill the specific concrete node information (model instance)
    toJsonInternal( model );
    data.emplace( "model", model );

    // store the supplemental information related to application/gui/...
    for ( auto& [key, value] : m_metadata.items() ) {
        if ( key != "instance" && key != "model" ) { data.emplace( key, value ); }
    }
}

void Node::add_metadata( const nlohmann::json& data ) {
    m_metadata.merge_patch( data );
}

auto Node::port_by_name( const std::string& type, const std::string& name ) const
    -> IndexAndPort<PortBaseRawPtr> {
    if ( type == "in" ) { return port_by_name( m_inputs, name ); }
    return port_by_name( m_outputs, name );
}

auto Node::input_by_name( const std::string& name ) const -> IndexAndPort<PortBaseInRawPtr> {
    return port_by_name( m_inputs, name );
}

auto Node::output_by_name( const std::string& name ) const -> IndexAndPort<PortBaseOutRawPtr> {
    return port_by_name( m_outputs, name );
}

PortBase* Node::port_by_index( const std::string& type, PortIndex idx ) const {
    if ( type == "in" ) return port_base( m_inputs, idx );
    return port_base( m_outputs, idx );
}

bool Node::fromJsonInternal( const nlohmann::json& data ) {
    LOG( Ra::Core::Utils::logDEBUG )
        << "default deserialization for " << instance_name() + " " + model_name() << ".";
    if ( const auto& ports = data.find( "inputs" ); ports != data.end() ) {
        for ( const auto& port : *ports ) {
            int index = port["port_index"];
            m_inputs[index]->from_json( port );
        }
    }
    if ( const auto& ports = data.find( "outputs" ); ports != data.end() ) {
        for ( const auto& port : *ports ) {
            int index = port["port_index"];
            m_outputs[index]->from_json( port );
        }
    }
    return true;
}

void Node::toJsonInternal( nlohmann::json& data ) const {
    std::string message =
        std::string { "default serialization for " } + instance_name() + " " + model_name();

    for ( size_t i = 0; i < m_inputs.size(); ++i ) {
        const auto& p = m_inputs[i];
        nlohmann::json port;
        p->to_json( port );
        port["port_index"] = i;
        port["type"]       = Ra::Core::Utils::simplifiedDemangledType( p->type() );
        data["inputs"].push_back( port );
    }
    for ( size_t i = 0; i < m_outputs.size(); ++i ) {
        const auto& p = m_outputs[i];
        nlohmann::json port;
        p->to_json( port );
        port["port_index"] = i;
        port["type"]       = Ra::Core::Utils::simplifiedDemangledType( p->type() );
        data["outputs"].push_back( port );
    }
    LOG( Ra::Core::Utils::logDEBUG ) << message;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
