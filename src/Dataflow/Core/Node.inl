#pragma once
#include <Dataflow/Core/Node.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

#if 0
template <class T>
constexpr std::string_view type_name() {
#    ifdef __clang__
    std::string_view p = __PRETTY_FUNCTION__;
    return std::string_view( p.data() + 34, p.size() - 34 - 1 );
#    elif defined( __GNUC__ )
    std::string_view p = __PRETTY_FUNCTION__;
#        if __cplusplus < 201402
    return std::string_view( p.data() + 36, p.size() - 36 - 1 );
#        else
    return std::string_view( p.data() + 49, p.find( ';', 49 ) - 49 );
#        endif
#    elif defined( _MSC_VER )
    std::string_view p = __FUNCSIG__;
    return std::string_view( p.data() + 84, p.size() - 84 - 7 );
#    else
    return type_id( T ).type_name();
#    endif
}

inline std::size_t
replace_all_in_string( std::string& inout, std::string_view what, std::string_view with ) {
    std::size_t count {};
    for ( std::string::size_type pos {};
          inout.npos != ( pos = inout.find( what.data(), pos, what.length() ) );
          pos += with.length(), ++count ) {
        inout.replace( pos, what.length(), with.data(), with.length() );
    }
    return count;
}

inline std::size_t remove_all_in_string( std::string& inout, std::string_view what ) {
    return replace_all_in_string( inout, what, "" );
}
#endif

inline void Node::init() {
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[34m\e[1m" << getTypeName() << "\e[0m \"" << m_instanceName
              << "\": initialization." << std::endl;
#endif
}

inline void Node::destroy() {
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[34m\e[1m" << getTypeName() << "\e[0m \"" << m_instanceName << "\": destroy."
              << std::endl;
#endif
}

inline nlohmann::json& Node::getJsonMetaData() {
    return m_extraJsonData;
}

inline const std::string& Node::getTypeName() const {
    return m_typeName;
}

inline const std::string& Node::getInstanceName() const {
    return m_instanceName;
}

inline void Node::setInstanceName( const std::string& newName ) {
    m_instanceName = newName;
}

inline bool Node::isDeletable() {
    return m_isDeletable;
}

inline void Node::setDeletableStatus( bool deletable ) {
    m_isDeletable = deletable;
}

inline const std::vector<std::unique_ptr<PortBase>>& Node::getInputs() {
    return m_inputs;
}

inline const std::vector<std::unique_ptr<PortBase>>& Node::getOutputs() {
    return m_outputs;
}

inline const std::vector<PortBase*>& Node::getInterface() {
    return m_interface;
}

inline const std::vector<std::unique_ptr<EditableParameterBase>>& Node::getEditableParameters() {
    return m_editableParameters;
}

inline void Node::setResourcesDir( std::string resourcesRootDir ) {
    m_resourceDir = std::move( resourcesRootDir );
}

inline bool Node::operator==( const Node& o_node ) {
    return m_typeName == o_node.getTypeName();
}

inline bool Node::addInput( PortBase* in ) {
    if ( !in->is_input() ) { return false; }
    bool found = false;
    for ( auto& input : m_inputs ) {
        if ( input->getName() == in->getName() ) { found = true; }
    }
    if ( !found ) { m_inputs.emplace_back( in ); }
    return !found;
}

inline bool Node::addOutput( PortBase* out ) {
    if ( out->is_input() ) { return false; }
    bool found = false;
    for ( auto& output : m_outputs ) {
        if ( output->getName() == out->getName() ) { found = true; }
    }
    if ( !found ) { m_outputs.emplace_back( out ); }
    return !found;
}

template <typename T>
void Node::addOutput( PortOut<T>* out, T* data ) {
    bool found = false;
    for ( auto& output : m_outputs ) {
        if ( output->getName() == out->getName() ) { found = true; }
    }
    if ( !found ) {
        m_outputs.emplace_back( out );
        out->setData( data );
    }
}

inline bool Node::addInterface( PortBase* ports ) {
    bool found = false;
    for ( auto& port : m_interface ) {
        if ( port->getName() == ports->getName() ) { found = true; }
    }
    if ( !found ) { m_interface.emplace_back( ports ); }
    return !found;
}

template <typename T>
bool Node::addEditableParameter( EditableParameter<T>* editableParameter ) {
    bool found = false;
    for ( auto& edit : m_editableParameters ) {
        if ( edit.get()->getName() == editableParameter->getName() ) { found = true; }
    }
    if ( !found ) { m_editableParameters.emplace_back( editableParameter ); }
    return !found;
}

template <typename T>
bool Node::removeEditableParameter( const std::string& name ) {
    bool found = false;
    auto it    = m_editableParameters.begin();
    while ( it != m_editableParameters.end() ) {
        if ( ( *it ).get()->getName() == name ) {
            m_editableParameters.erase( it );
            found = true;
            break;
        }
        ++it;
    }
    return found;
}

inline const std::string Node::getTypename() {
    return "Node";
}

inline bool Node::compile() {
    return true;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
