#pragma once
#include <Dataflow/Core/Nodes/Functionals/ReduceNode.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

template <typename coll_t, typename v_t>
ReduceNode<coll_t, v_t>::ReduceNode( const std::string& instanceName ) :
    ReduceNode(
        instanceName,
        getTypename(),
        []( const v_t& a, const v_t& ) -> v_t { return a; },
        v_t {} ) {}

template <typename coll_t, typename v_t>
ReduceNode<coll_t, v_t>::ReduceNode( const std::string& instanceName,
                                     ReduceOperator op,
                                     v_t initialValue ) :
    ReduceNode( instanceName, getTypename(), op, initialValue ) {}

template <typename coll_t, typename v_t>
void ReduceNode<coll_t, v_t>::setOperator( ReduceOperator op, v_t initialValue ) {
    m_operator = op;
    m_init     = initialValue;
}

template <typename coll_t, typename v_t>
void ReduceNode<coll_t, v_t>::init() {
    Node::init();
    m_result = m_init;
}

template <typename coll_t, typename v_t>
void ReduceNode<coll_t, v_t>::execute() {
    auto f   = m_operator;
    auto iv  = m_init;
    auto ivp = static_cast<PortIn<v_t>*>( m_inputs[2].get() );
    if ( ivp->isLinked() ) { iv = ivp->getData(); }
    m_result      = iv;
    auto predPort = static_cast<PortIn<ReduceOperator>*>( m_inputs[1].get() );
    if ( predPort->isLinked() ) { f = predPort->getData(); }
    auto input = static_cast<PortIn<coll_t>*>( m_inputs[0].get() );
    if ( input->isLinked() ) {
        const auto& inData = input->getData();
        m_result           = std::accumulate( inData.begin(), inData.end(), iv, f );
#ifdef GRAPH_CALL_TRACE
        std::cout << "\e[36m\e[1mMReduceNode \e[0m \"" << m_instanceName << "\": execute, from "
                  << input->getData().size() << " " << typeid( T ).name() << "." << std::endl;
#endif
    }
}

template <typename coll_t, typename v_t>
const std::string& ReduceNode<coll_t, v_t>::getTypename() {
    static std::string demangledName =
        std::string { "Reduce<" } + Ra::Dataflow::Core::simplifiedDemangledType<coll_t>() + ">";
    return demangledName;
}

template <typename coll_t, typename v_t>
ReduceNode<coll_t, v_t>::ReduceNode( const std::string& instanceName,
                                     const std::string& typeName,
                                     ReduceOperator op,
                                     v_t initialValue ) :
    Node( instanceName, typeName ), m_operator( op ), m_init( initialValue ) {
    auto in = new PortIn<coll_t>( "in", this );
    addInput( in );
    in->mustBeLinked();

    auto f = new PortIn<ReduceOperator>( "f", this );
    addInput( f );

    auto iv = new PortIn<v_t>( "init", this );
    addInput( iv );

    auto out = new PortOut<v_t>( "out", this );
    addOutput( out, &m_result );
}

template <typename coll_t, typename v_t>
void ReduceNode<coll_t, v_t>::toJsonInternal( nlohmann::json& data ) const {
    data["comment"] =
        std::string { "Reduce operator could not be serialized for " } + getTypeName();
    LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
        << "Unable to save data when serializing a " << getTypeName() << ".";
}

template <typename coll_t, typename v_t>
void ReduceNode<coll_t, v_t>::fromJsonInternal( const nlohmann::json& ) {
    LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
        << "Unable to read data when un-serializing a " << getTypeName() << ".";
}

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra
