#pragma once
#include <Dataflow/Core/Node.hpp>

#include <Dataflow/Core/TypeDemangler.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

inline void Node::init() {
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[34m\e[1m" << getTypeName() << "\e[0m \"" << m_instanceName
              << "\": initialization." << std::endl;
#endif
    m_initialized = true;
}

inline void Node::destroy() {
#ifdef GRAPH_CALL_TRACE
    std::cout << "\e[34m\e[1m" << getTypeName() << "\e[0m \"" << m_instanceName << "\": destroy."
              << std::endl;
#endif
    m_interface.clear();
    m_inputs.clear();
    m_outputs.clear();
    m_editableParameters.clear();
}

inline const nlohmann::json& Node::getJsonMetaData() {
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

inline const std::vector<std::unique_ptr<PortBase>>& Node::getInputs() {
    return m_inputs;
}

inline const std::vector<std::unique_ptr<PortBase>>& Node::getOutputs() {
    return m_outputs;
}

inline const std::vector<PortBase*>& Node::buildInterfaces( Node* parent ) {
    m_interface.clear();
    m_interface.shrink_to_fit();
    std::vector<std::unique_ptr<PortBase>>* readFrom;
    if ( m_inputs.empty() ) { readFrom = &m_outputs; }
    else {
        readFrom = &m_inputs;
    }
    m_interface.reserve( readFrom->size() );
    for ( const auto& p : *readFrom ) {
        // Todo, ensure thereis no twice the same port ....
        m_interface.emplace_back( p->reflect( parent, getInstanceName() + '_' + p->getName() ) );
    }
    return m_interface;
}
inline const std::vector<PortBase*>& Node::getInterfaces() {
    return m_interface;
}

inline const std::vector<std::unique_ptr<EditableParameterBase>>& Node::getEditableParameters() {
    return m_editableParameters;
}

inline bool Node::operator==( const Node& o_node ) {
    return ( m_typeName == o_node.getTypeName() ) && ( m_instanceName == o_node.getInstanceName() );
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

inline bool Node::addEditableParameter( EditableParameterBase* editableParameter ) {
    bool found = false;
    for ( auto& edit : m_editableParameters ) {
        if ( edit.get()->getName() == editableParameter->getName() ) { found = true; }
    }
    if ( !found ) { m_editableParameters.emplace_back( editableParameter ); }
    return !found;
}

inline bool Node::removeEditableParameter( const std::string& name ) {
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

template <typename E>
inline EditableParameter<E>* Node::getEditableParameter( const std::string& name ) {
    auto it = m_editableParameters.begin();
    while ( it != m_editableParameters.end() ) {
        if ( ( *it ).get()->getName() == name ) {
            auto p = dynamic_cast<EditableParameter<E>*>( ( *it ).get() );
            if ( p != nullptr ) { return *p; }
        }
        ++it;
    }
    return nullptr;
}

inline const std::string& Node::getTypename() {
    static std::string demangledTypeName { "Abstract Node" };
    return demangledTypeName;
}

inline bool Node::compile() {
    return true;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
