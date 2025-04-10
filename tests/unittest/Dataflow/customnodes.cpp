/**
 * Demonstrate how to define custom nodes anduse factory to serialize graphs with custom nodes
 */
#include <catch2/catch_test_macros.hpp>

#include <string>

#include <iostream>

#include <Core/Utils/StdFilesystem.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Functionals/Types.hpp>
#include <Dataflow/Core/Sinks/Types.hpp>
#include <Dataflow/Core/Sources/Types.hpp>

using namespace Ra::Dataflow::Core;

namespace Customs {
using CustomStringSource = Sources::SingleDataSourceNode<std::string>;
using CustomStringSink   = Sinks::SinkNode<std::string>;

//! [Develop a custom node]
/**
 * \brief generate a predicate that compare a value wrt a threshold.
 * The name of the operator is fetched from input port "name" or the internal data set using
 * setFunctionName. Available operator are "=", ">", ">=", "<", "<=", "!=", "true", "false".
 *
 * The threshold is fetched from input port "threshold" or the internal data set using setThreshold.
 *
 * The operator is sent on the output port "f".
 *
 * \tparam T the type of the parameter to evaluate
 */
template <class T>
class FilterSelector final : public Node
{
  public:
    using function_type = std::function<bool( const T& )>;

    explicit FilterSelector( const std::string& name ) : FilterSelector( name, node_typename() ) {}

    bool execute() override {
        if ( !m_portName->has_data() ) return false;
        m_nameOut->set_data( &m_portName->data() );
        m_currentFunction = m_functions.at( m_portName->data() );
        return true;
    }

  protected:
    bool fromJsonInternal( const nlohmann::json& data ) override {
        if ( data.contains( "operator" ) ) { m_portName->set_default_value( data["operator"] ); }
        else { m_portName->set_default_value( "true" ); }
        if ( data.contains( "threshold" ) ) {
            m_portThreshold->set_default_value( data["threshold"] );
        }
        else { m_portThreshold->set_default_value( {} ); }
        return true;
    }

    void toJsonInternal( nlohmann::json& data ) const override {
        data["operator"]  = m_portName->data();
        data["threshold"] = m_portThreshold->data();
    }

  public:
    static const std::string& node_typename() {
        static std::string demangledTypeName =
            std::string { "FilterSelector<" } + Ra::Core::Utils::simplifiedDemangledType<T>() + ">";
        return demangledTypeName;
    }

  private:
    FilterSelector( const std::string& instanceName, const std::string& typeName ) :
        Node( instanceName, typeName ) {}
    /// map name's string to function
    std::map<std::string, function_type> m_functions {
        { "true", []( const T& ) { return true; } },
        { "false", []( const T& ) { return false; } },
        { "<", [this]( const T& v ) { return v < this->m_portThreshold->data(); } },
        { ">", [this]( const T& v ) { return v > this->m_portThreshold->data(); } } };

    function_type m_currentFunction = m_functions["true"];

    /// Alias to the output port
    PortOutPtr<function_type> m_operatourOut {
        add_output<function_type>( &m_currentFunction, "f" ) };
    PortOutPtr<std::string> m_nameOut { add_output<std::string>( "name" ) };
    /// Alias for the input ports
    PortInPtr<std::string> m_portName { add_input<std::string>( "name", "true" ) };
    PortInPtr<T> m_portThreshold { add_input<T>( "threshold", T {} ) };
};
//! [Develop a custom node]
} // namespace Customs

template <typename DataType>
using CollectionType = Ra::Core::VectorArray<DataType>;
template <typename DataType>
using CollectionInputType = Sources::SingleDataSourceNode<CollectionType<DataType>>;
template <typename DataType>
using CollectionOutputType = Sinks::SinkNode<CollectionType<DataType>>;
template <typename DataType>
using FilterCollectionType = Functionals::FilterNode<Ra::Core::VectorArray<DataType>>;

// Reusable function to create a graph
template <typename DataType>
DataflowGraph* buildgraph( const std::string& name ) {
    auto g = new DataflowGraph( name );

    auto ds = std::make_shared<CollectionInputType<DataType>>( "ds" );
    REQUIRE( g->add_node( ds ) );

    auto rs = std::make_shared<CollectionOutputType<DataType>>( "rs" );
    REQUIRE( g->add_node( rs ) );

    auto ts = std::make_shared<Sources::SingleDataSourceNode<DataType>>( "ts" );
    REQUIRE( g->add_node( ts ) );

    auto ss = std::make_shared<Customs::CustomStringSource>( "ss" );
    REQUIRE( g->add_node( ss ) );

    auto nm = std::make_shared<Customs::CustomStringSink>( "nm" );
    REQUIRE( g->add_node( nm ) );

    auto fs = std::make_shared<Customs::FilterSelector<DataType>>( "fs" );
    REQUIRE( g->add_node( fs ) );

    auto fl = std::make_shared<FilterCollectionType<DataType>>( "fl" );
    REQUIRE( g->add_node( fl ) );

    auto coreFactory = NodeFactoriesManager::dataFlowBuiltInsFactory();

    REGISTER_TYPE_TO_FACTORY( coreFactory, FilterCollectionType<DataType>, Functionals );
    REGISTER_TYPE_TO_FACTORY( coreFactory, CollectionInputType<DataType>, Functionals );
    REGISTER_TYPE_TO_FACTORY( coreFactory, CollectionOutputType<DataType>, Functionals );

    REQUIRE( g->add_link( ds, "to", fl, "data" ) );
    REQUIRE( g->add_link( fl, "result", rs, "from" ) );
    REQUIRE( g->add_link( ss, "to", fs, "name" ) );
    REQUIRE( g->add_link( ts, "to", fs, "threshold" ) );
    REQUIRE( g->add_link( fs, "f", fl, "predicate" ) );
    REQUIRE( g->add_link( fs, "name", nm, "from" ) );
    return g;
}

// test sections
TEST_CASE( "Dataflow/Core/Custom nodes", "[unittests][Dataflow][Core][Custom nodes]" ) {
    SECTION( "Build graph with custom nodes" ) {
        // build a graph
        auto g = buildgraph<Scalar>( "testCustomNodes" );

        // get input and ouput of the graph
        auto inputCollection =
            std::dynamic_pointer_cast<CollectionInputType<Scalar>>( g->node( "ds" ) );
        REQUIRE( inputCollection != nullptr );
        auto inputOpName =
            std::dynamic_pointer_cast<Customs::CustomStringSource>( g->node( "ss" ) );
        REQUIRE( inputOpName != nullptr );
        auto inputThreshold =
            std::dynamic_pointer_cast<Sources::SingleDataSourceNode<Scalar>>( g->node( "ts" ) );
        REQUIRE( inputThreshold != nullptr );

        auto filteredCollection = g->node( "rs" );
        REQUIRE( filteredCollection != nullptr );
        auto generatedOperator = g->node( "nm" );
        REQUIRE( generatedOperator != nullptr );

        // parameterize the graph
        using CollectionType = Ra::Core::VectorArray<Scalar>;
        CollectionType testVector;
        testVector.reserve( 10 );
        std::mt19937 gen( 0 );
        std::uniform_real_distribution<Scalar> dis( 0.0_ra, 1.0_ra );
        // Fill the vector with random numbers between 0 and 1
        for ( size_t n = 0; n < testVector.capacity(); ++n ) {
            testVector.push_back( dis( gen ) );
        }

        inputCollection->set_data( testVector );
        inputThreshold->set_data( .5_ra );
        inputOpName->set_data( "true" );

        // execute the graph that filter out nothing
        REQUIRE( g->execute() );

        // Getters are usable only after successful compilation/execution of the graph
        // Get results as references (no need to get them again later if the graph does
        // not change)
        auto& vres = filteredCollection->input_by_name( "from" ).second->data<CollectionType>();
        auto& vop  = generatedOperator->input_by_name( "from" ).second->data<std::string>();

        REQUIRE( vop == "true" );
        REQUIRE( vres.size() == testVector.size() );

        // change operator to filter out everything
        inputOpName->set_data( "false" );

        REQUIRE( g->execute() );
        REQUIRE( vop == "false" );
        REQUIRE( vres.size() == 0 );

        // Change operator to keep element less than threshold
        inputOpName->set_data( "<" );

        REQUIRE( g->execute() );

        REQUIRE( *( std::max_element( vres.begin(), vres.end() ) ) < *inputThreshold->data() );

        // Change operator to keep element greater than threshold
        inputOpName->set_data( ">" );
        REQUIRE( g->execute() );
        REQUIRE( *( std::max_element( vres.begin(), vres.end() ) ) > *inputThreshold->data() );
    }
    SECTION( "Serialization of a custom graph" ) {
        // Create and fill the factory for the custom nodes
        auto customFactory = NodeFactoriesManager::create_factory( "CustomNodesUnitTests" );

        // add node creators to the factory

        REQUIRE( customFactory->register_node_creator<Customs::CustomStringSource>(
            Customs::CustomStringSource::node_typename() + "_", "Custom" ) );
        REQUIRE( customFactory->register_node_creator<Customs::CustomStringSink>(
            Customs::CustomStringSink::node_typename() + "_", "Custom" ) );
        REQUIRE( customFactory->register_node_creator<Customs::FilterSelector<Scalar>>(
            Customs::FilterSelector<Scalar>::node_typename() + "_", "Custom" ) );
        // The same node can't be register twice in the same factory
        REQUIRE( !customFactory->register_node_creator<Customs::FilterSelector<Scalar>>(
            Customs::FilterSelector<Scalar>::node_typename() + "_", "Custom" ) );

        nlohmann::json emptyData;
        auto customSource = customFactory->create_node(
            Customs::CustomStringSource::node_typename(), emptyData, nullptr );
        REQUIRE( customSource );

        // build a graph
        auto g = buildgraph<Scalar>( "testCustomNodes" );

        std::string tmpdir { "customGraphExport/" };
        std::filesystem::create_directories( tmpdir );

        // save the graph with factory
        g->saveToJson( tmpdir + "customGraph.json" );

        g->destroy();
        delete g;
        g = new DataflowGraph( "" );

        REQUIRE( g->loadFromJson( tmpdir + "customGraph.json" ) );
        g->destroy();
        delete g;

        /// try to load the graph without custom factory
        auto unregistered = NodeFactoriesManager::unregister_factory( customFactory->name() );
        REQUIRE( unregistered == true );

        g = new DataflowGraph( "" );
        REQUIRE( !g->loadFromJson( tmpdir + "customGraph.json" ) );
        delete g;

        std::filesystem::remove_all( tmpdir );
    }
}
