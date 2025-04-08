/**
 * Demonstrate how to define custom nodes anduse factory to serialize graphs with custom nodes
 */
#include <catch2/catch_test_macros.hpp>

#include <string>

#include <iostream>

#include <Core/Utils/StdFilesystem.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Functionals/CoreDataFunctionals.hpp>
#include <Dataflow/Core/Sinks/CoreDataSinks.hpp>
#include <Dataflow/Core/Sources/CoreDataSources.hpp>

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

    explicit FilterSelector( const std::string& name ) : FilterSelector( name, getTypename() ) {}

    bool execute() override {
        if ( !m_portName->hasData() ) return false;
        m_nameOut->setData( &m_portName->getData() );
        m_currentFunction = m_functions.at( m_portName->getData() );
        return true;
    }

  protected:
    bool fromJsonInternal( const nlohmann::json& data ) override {
        if ( data.contains( "operator" ) ) { m_portName->setDefaultValue( data["operator"] ); }
        else { m_portName->setDefaultValue( "true" ); }
        if ( data.contains( "threshold" ) ) {
            m_portThreshold->setDefaultValue( data["threshold"] );
        }
        else { m_portThreshold->setDefaultValue( {} ); }
        return true;
    }

    void toJsonInternal( nlohmann::json& data ) const override {
        data["operator"]  = m_portName->getData();
        data["threshold"] = m_portThreshold->getData();
    }

  public:
    static const std::string& getTypename() {
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
        { "<", [this]( const T& v ) { return v < this->m_portThreshold->getData(); } },
        { ">", [this]( const T& v ) { return v > this->m_portThreshold->getData(); } } };

    function_type m_currentFunction = m_functions["true"];

    /// Alias to the output port
    PortOutPtr<function_type> m_operatourOut {
        addOutputPort<function_type>( &m_currentFunction, "f" ) };
    PortOutPtr<std::string> m_nameOut { addOutputPort<std::string>( "name" ) };
    /// Alias for the input ports
    PortInPtr<std::string> m_portName { addInputPort<std::string>( "name", "true" ) };
    PortInPtr<T> m_portThreshold { addInputPort<T>( "threshold", T {} ) };
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
    REQUIRE( g->addNode( ds ) );

    auto rs = std::make_shared<CollectionOutputType<DataType>>( "rs" );
    REQUIRE( g->addNode( rs ) );

    auto ts = std::make_shared<Sources::SingleDataSourceNode<DataType>>( "ts" );
    REQUIRE( g->addNode( ts ) );

    auto ss = std::make_shared<Customs::CustomStringSource>( "ss" );
    REQUIRE( g->addNode( ss ) );

    auto nm = std::make_shared<Customs::CustomStringSink>( "nm" );
    REQUIRE( g->addNode( nm ) );

    auto fs = std::make_shared<Customs::FilterSelector<DataType>>( "fs" );
    REQUIRE( g->addNode( fs ) );

    auto fl = std::make_shared<FilterCollectionType<DataType>>( "fl" );
    REQUIRE( g->addNode( fl ) );

    auto coreFactory = NodeFactoriesManager::getDataFlowBuiltInsFactory();

    REGISTER_TYPE_TO_FACTORY( coreFactory, FilterCollectionType<DataType>, Functionals );
    REGISTER_TYPE_TO_FACTORY( coreFactory, CollectionInputType<DataType>, Functionals );
    REGISTER_TYPE_TO_FACTORY( coreFactory, CollectionOutputType<DataType>, Functionals );

    REQUIRE( g->addLink( ds, "to", fl, "data" ) );
    REQUIRE( g->addLink( fl, "result", rs, "from" ) );
    REQUIRE( g->addLink( ss, "to", fs, "name" ) );
    REQUIRE( g->addLink( ts, "to", fs, "threshold" ) );
    REQUIRE( g->addLink( fs, "f", fl, "predicate" ) );
    REQUIRE( g->addLink( fs, "name", nm, "from" ) );
    return g;
}

// test sections
TEST_CASE( "Dataflow/Core/Custom nodes", "[unittests][Dataflow][Core][Custom nodes]" ) {
    SECTION( "Build graph with custom nodes" ) {
        // build a graph
        auto g = buildgraph<Scalar>( "testCustomNodes" );

        // get input and ouput of the graph
        auto inputCollection =
            std::dynamic_pointer_cast<CollectionInputType<Scalar>>( g->getNode( "ds" ) );
        REQUIRE( inputCollection != nullptr );
        auto inputOpName =
            std::dynamic_pointer_cast<Customs::CustomStringSource>( g->getNode( "ss" ) );
        REQUIRE( inputOpName != nullptr );
        auto inputThreshold =
            std::dynamic_pointer_cast<Sources::SingleDataSourceNode<Scalar>>( g->getNode( "ts" ) );
        REQUIRE( inputThreshold != nullptr );

        auto filteredCollection = g->getNode( "rs" );
        REQUIRE( filteredCollection != nullptr );
        auto generatedOperator = g->getNode( "nm" );
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

        inputCollection->setData( testVector );
        inputThreshold->setData( .5_ra );
        inputOpName->setData( "true" );

        // execute the graph that filter out nothing
        REQUIRE( g->execute() );

        // Getters are usable only after successful compilation/execution of the graph
        // Get results as references (no need to get them again later if the graph does
        // not change)
        auto& vres = filteredCollection->getInputByName( "from" ).second->getData<CollectionType>();
        auto& vop  = generatedOperator->getInputByName( "from" ).second->getData<std::string>();

        REQUIRE( vop == "true" );
        REQUIRE( vres.size() == testVector.size() );

        // change operator to filter out everything
        inputOpName->setData( "false" );

        REQUIRE( g->execute() );
        REQUIRE( vop == "false" );
        REQUIRE( vres.size() == 0 );

        // Change operator to keep element less than threshold
        inputOpName->setData( "<" );

        REQUIRE( g->execute() );

        REQUIRE( *( std::max_element( vres.begin(), vres.end() ) ) < *inputThreshold->getData() );

        // Change operator to keep element greater than threshold
        inputOpName->setData( ">" );
        REQUIRE( g->execute() );
        REQUIRE( *( std::max_element( vres.begin(), vres.end() ) ) > *inputThreshold->getData() );
    }
    SECTION( "Serialization of a custom graph" ) {
        // Create and fill the factory for the custom nodes
        auto customFactory = NodeFactoriesManager::createFactory( "CustomNodesUnitTests" );

        // add node creators to the factory

        REQUIRE( customFactory->registerNodeCreator<Customs::CustomStringSource>(
            Customs::CustomStringSource::getTypename() + "_", "Custom" ) );
        REQUIRE( customFactory->registerNodeCreator<Customs::CustomStringSink>(
            Customs::CustomStringSink::getTypename() + "_", "Custom" ) );
        REQUIRE( customFactory->registerNodeCreator<Customs::FilterSelector<Scalar>>(
            Customs::FilterSelector<Scalar>::getTypename() + "_", "Custom" ) );
        // The same node can't be register twice in the same factory
        REQUIRE( !customFactory->registerNodeCreator<Customs::FilterSelector<Scalar>>(
            Customs::FilterSelector<Scalar>::getTypename() + "_", "Custom" ) );

        nlohmann::json emptyData;
        auto customSource = customFactory->createNode(
            Customs::CustomStringSource::getTypename(), emptyData, nullptr );
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
        auto unregistered = NodeFactoriesManager::unregisterFactory( customFactory->getName() );
        REQUIRE( unregistered == true );

        g = new DataflowGraph( "" );
        REQUIRE( !g->loadFromJson( tmpdir + "customGraph.json" ) );
        delete g;

        std::filesystem::remove_all( tmpdir );
    }
}
