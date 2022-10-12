/**
 * Demonstrate how to define custom nodes anduse factory to serialize graphs with custom nodes
 */
#include <catch2/catch.hpp>

#include <string>
#include <vector>

#include <iostream>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Nodes/CoreBuiltInsNodes.hpp>

using namespace Ra::Dataflow::Core;

namespace Customs {
using CustomStringSource = Sources::SingleDataSourceNode<std::string>;
using CustomStringSink   = Sinks::SinkNode<std::string>;

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

    void execute() override {
        // get operator parameters
        if ( m_portName->isLinked() ) { m_operatorName = m_portName->getData(); }
        if ( m_portThreshold->isLinked() ) { m_threshold = m_portThreshold->getData(); }
        // compute the result associated to the output port
        m_currentFunction = m_functions[m_operatorName];
    }

    /** \brief Set the function name used to select the function to deliver.
     * this name will be used if the input port "name" is not linked
     * @param name
     */
    void setOperatorName( const std::string& name ) { m_operatorName = name; }
    /**
     * \brief Get the delivered data
     * @return The non owning pointer (alias) to the delivered data.
     */
    function_type* getOperator() const { return m_functions[m_operatorName]; }

    /** \brief Set the threshold - will copy the value into the node
     * @param name
     */
    void setThreshold( const T& t ) { m_threshold = t; }

    /** \brief Get the threshold
     */
    T getThreshold() const { return m_threshold; }

  protected:
    void fromJsonInternal( const nlohmann::json& jsonData ) override {
        if ( jsonData.contains( "operator" ) ) { m_operatorName = jsonData["operator"]; }
        else {
            m_operatorName = "true";
        }
        if ( jsonData.contains( "threshold" ) ) { m_threshold = jsonData["threshold"]; }
        else {
            m_threshold = T {};
        }
    }

    void toJsonInternal( nlohmann::json& jsonData ) const override {
        jsonData["operator"]  = m_operatorName;
        jsonData["threshold"] = m_threshold;
    }

  public:
    static const std::string& getTypename() {
        static std::string demangledTypeName = std::string { "FilterSelector<" } +
                                               Ra::Dataflow::Core::simplifiedDemangledType<T>() +
                                               ">";
        return demangledTypeName;
    }

  private:
    FilterSelector( const std::string& instanceName, const std::string& typeName ) :
        Node( instanceName, typeName ) {
        // Adding ports to node
        addInput( m_portName );
        addInput( m_portThreshold );
        addOutput( m_operatourOut, &m_currentFunction );
        addOutput( m_nameOut, &m_operatorName );
    }

    /// Alias to the output port
    PortOut<function_type>* m_operatourOut { new PortOut<function_type>( "f", this ) };
    PortOut<std::string>* m_nameOut { new PortOut<std::string>( "name", this ) };
    /// Alias for the input ports
    PortIn<std::string>* m_portName { new PortIn<std::string>( "name", this ) };
    PortIn<T>* m_portThreshold { new PortIn<T>( "threshold", this ) };

    /// The data provided by the node
    std::map<std::string, function_type> m_functions {
        { "true", []( const T& ) { return true; } },
        { "false", []( const T& ) { return false; } },
        { "<", [this]( const T& v ) { return v < this->m_threshold; } } };

    std::string m_operatorName { "true" };
    function_type m_currentFunction = m_functions[m_operatorName];
    T m_threshold {};
};

} // namespace Customs

// Reusable function to create a graph
template <typename DataType>
DataflowGraph* buildgraph( const std::string& name ) {
    auto ds = new Sources::SingleDataSourceNode<Ra::Core::VectorArray<DataType>>( "ds" );
    auto rs = new Sinks::SinkNode<Ra::Core::VectorArray<DataType>>( "rs" );
    auto ts = new Sources::SingleDataSourceNode<DataType>( "ts" );
    auto ss = new Customs::CustomStringSource( "ss" );
    auto nm = new Customs::CustomStringSink( "nm" );
    auto fs = new Customs::FilterSelector<DataType>( "fs" );
    auto fl = new Functionals::FilterNode<Ra::Core::VectorArray<DataType>>( "fl" );

    auto g = new DataflowGraph( name );
    g->addNode( ds );
    g->addNode( rs );
    g->addNode( ts );
    g->addNode( ss );
    g->addNode( nm );
    g->addNode( fs );
    g->addNode( fl );

    bool ok;
    ok = g->addLink( ds, "to", fl, "in" );
    REQUIRE( ok );
    ok = g->addLink( fl, "out", rs, "from" );
    REQUIRE( ok );
    ok = g->addLink( ss, "to", fs, "name" );
    REQUIRE( ok );
    ok = g->addLink( ts, "to", fs, "threshold" );
    REQUIRE( ok );
    ok = g->addLink( fs, "f", fl, "f" );
    REQUIRE( ok );
    ok = g->addLink( fs, "name", nm, "from" );
    REQUIRE( ok );
    return g;
}

// test sections
TEST_CASE( "Dataflow/Core/Custom nodes", "[Dataflow][Core][Custom nodes]" ) {
    SECTION( "Build graph with custom nodes" ) {
        // build a graph
        auto g = buildgraph<Scalar>( "testCustomNodes" );

        //! [Inspect the graph interface : inputs and outputs port]
        auto inputs = g->getAllDataSetters();
        std::cout << "Input ports (" << inputs.size() << ") are :\n";
        for ( auto& [ptrPort, portName, portType] : inputs ) {
            std::cout << "\t\"" << portName << "\" accepting type " << portType << "\n";
        }
        auto outputs = g->getAllDataGetters();
        std::cout << "Output ports (" << outputs.size() << ") are :\n";
        for ( auto& [ptrPort, portName, portType] : outputs ) {
            std::cout << "\t\"" << portName << "\" generating type " << portType << "\n";
        }
        //! [Inspect the graph interface : inputs and outputs port]

        // get input and ouput of the graph
        auto inputCollection = g->getDataSetter( "ds_to" );
        REQUIRE( inputCollection != nullptr );
        auto inputOpName = g->getDataSetter( "ss_to" );
        REQUIRE( inputOpName != nullptr );
        auto inputThreshold = g->getDataSetter( "ts_to" );
        REQUIRE( inputThreshold != nullptr );

        auto filteredCollection = g->getDataGetter( "rs_from" );
        auto generatedOperator  = g->getDataGetter( "nm_from" );

        auto r = g->compile();
        REQUIRE( r );
        // parameterise the graph
        using CollectionType = Ra::Core::VectorArray<Scalar>;
        CollectionType testVector;
        testVector.reserve( 10 );
        std::mt19937 gen( 0 );
        std::uniform_real_distribution<> dis( 0.0, 1.0 );
        // Fill the vector with random numbers between 0 and 1
        for ( size_t n = 0; n < testVector.capacity(); ++n ) {
            testVector.push_back( dis( gen ) );
        }
        inputCollection->setData( &testVector );

        Scalar threshold { 0.5_ra };
        inputThreshold->setData( &threshold );

        std::string op { "true" };
        inputOpName->setData( &op );

        std::cout << "Data sent to graph : \n\toperator " << op << " : \n\t";
        for ( auto ord : testVector ) {
            std::cout << ord << ' ';
        }
        std::cout << '\n';

        // execute the graph that filter out nothing
        // execute
        g->execute();
        // Get results as references (ne need to get them again later)
        auto& vres = filteredCollection->getData<CollectionType>();
        auto& vop  = generatedOperator->getData<std::string>();

        REQUIRE( vres.size() == testVector.size() );
        std::cout << "Result after applying operator " << vop << " and threshold " << threshold
                  << ": \n\t";
        for ( auto ord : vres ) {
            std::cout << ord << ' ';
        }
        std::cout << '\n';

        // change operator to filter out everything
        op = "false";
        g->execute();
        REQUIRE( vres.size() == 0 );

        std::cout << "Result after applying operator " << vop << " and threshold " << threshold
                  << ": \n\t";
        for ( auto ord : vres ) {
            std::cout << ord << ' ';
        }
        std::cout << '\n';

        // Change operator to keep element less than threshold
        op = "<";
        g->execute();

        std::cout << "Result after applying operator " << vop << " and threshold " << threshold
                  << ": \n\t";
        for ( auto ord : vres ) {
            std::cout << ord << ' ';
        }
        std::cout << '\n';
        REQUIRE( *( std::max_element( vres.begin(), vres.end() ) ) < threshold );
    }
    SECTION( "Serialization of a custom graph" ) {
        // Create and fill the factory for the custom nodes
        NodeFactorySet::mapped_type customFactory {
            new NodeFactorySet::mapped_type::element_type( "CustomNodesUnitTests" ) };

        // add node creators to the factory
        bool registered;
        registered = customFactory->registerNodeCreator<Customs::CustomStringSource>(
            Customs::CustomStringSource::getTypename() + "_", "Custom" );
        REQUIRE( registered == true );
        registered = customFactory->registerNodeCreator<Customs::CustomStringSink>(
            Customs::CustomStringSink::getTypename() + "_", "Custom" );
        REQUIRE( registered == true );
        registered = customFactory->registerNodeCreator<Customs::FilterSelector<Scalar>>(
            Customs::FilterSelector<Scalar>::getTypename() + "_", "Custom" );
        REQUIRE( registered == true );
        // The same node can't be register twice in the same factory
        registered = customFactory->registerNodeCreator<Customs::FilterSelector<Scalar>>(
            Customs::FilterSelector<Scalar>::getTypename() + "_", "Custom" );
        REQUIRE( registered == false );
        // register the factory into the system to enable loading any graph that use these nodes
        NodeFactoriesManager::registerFactory( customFactory );

        // build a graph
        auto g = buildgraph<Scalar>( "testCustomNodes" );
        g->addFactory( customFactory->getName(), customFactory );

        std::string tmpdir { "customGraphExport/" };
        std::filesystem::create_directories( tmpdir );

        // save the graph without factory
        // save the graph with factory
        g->saveToJson( tmpdir + "customGraph.json" );

        g->destroy();
        delete g;
        g = new DataflowGraph( "" );

        bool loaded = g->loadFromJson( tmpdir + "customGraph.json" );

        REQUIRE( loaded == true );
        g->destroy();
        delete g;

        /// try to load the graph without custom factory
        NodeFactoriesManager::unregisterFactory( customFactory->getName() );

        g      = new DataflowGraph( "" );
        loaded = g->loadFromJson( tmpdir + "customGraph.json" );
        REQUIRE( loaded == false );

        delete g;
        std::filesystem::remove_all( tmpdir );
    }
}
