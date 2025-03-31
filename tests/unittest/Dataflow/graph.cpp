#include <catch2/catch.hpp>

#include <iostream>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Nodes/Functionals/CoreDataFunctionals.hpp>
#include <Dataflow/Core/Nodes/Sinks/CoreDataSinks.hpp>
#include <Dataflow/Core/Nodes/Sources/CoreDataSources.hpp>

using namespace Ra::Dataflow::Core;

void inspectGraph( const DataflowGraph& g ) {
    // Nodes of the graph
    const auto& nodes = g.getNodes();
    std::cout << "Nodes of the graph " << g.getInstanceName() << " (" << nodes.size() << ") :\n";
    for ( const auto& n : nodes ) {
        std::cout << "\t\"" << n->getInstanceName() << "\" of type \"" << n->getModelName()
                  << "\"\n";
        // Inspect input, output and interfaces of the node
        std::cout << "\t\tInput ports :\n";
        for ( const auto& p : n->getInputs() ) {
            std::cout << "\t\t\t\"" << p->getName() << "\" with type " << p->getTypeName();
            if ( p->isLinked() ) {
                std::cout << " linked to " << p->getLink()->getNode()->display_name() << " "
                          << p->getLink()->getName();
            }
            std::cout << "\n";
        }
        std::cout << "\t\tOutput ports :\n";
        for ( const auto& p : n->getOutputs() ) {
            std::cout << "\t\t\t\"" << p->getName() << "\" with type " << p->getTypeName();
            std::cout << "\n";
        }
    }

    // Nodes by level after the compilation
    if ( g.isCompiled() ) {
        auto& cn = g.getNodesByLevel();
        std::cout << "Nodes of the graph, sorted by level after compiling the graph :\n";
        for ( size_t i = 0; i < cn.size(); ++i ) {
            std::cout << "\tLevel " << i << " :\n";
            for ( const auto n : cn[i] ) {
                std::cout << "\t\t\"" << n->getInstanceName() << "\"\n";
            }
        }
    }

    // describe the graph interface : inputs and outputs port of the whole graph (not of the
    // nodes)
    std::cout << "Inputs and output ports of the graph " << g.getInstanceName() << " :\n";
    const auto& inputs = g.getInputs();
    std::cout << "\tInput ports (" << inputs.size() << ") are :\n";
    for ( const auto& inp : inputs ) {
        std::cout << "\t\t\"" << inp->getName() << "\" accepting type \"" << inp->getTypeName()
                  << "\"\n";
    }
    const auto& outputs = g.getOutputs();
    std::cout << "\tOutput ports (" << outputs.size() << ") are :\n";
    for ( const auto& outp : outputs ) {
        std::cout << "\t\t\"" << outp->getName() << "\" accepting type \"" << outp->getTypeName()
                  << "\"\n";
    }

    std::cout << "DataSetters and DataGetters port of the graph " << g.getInstanceName() << " :\n";
}

TEST_CASE( "Dataflow/Core/Graph", "[Dataflow][Core][Graph]" ) {
    DataflowGraph g( "Test Graph" );
    SECTION( "not a json" ) {
        auto result = g.loadFromJson( "data/Dataflow/NotAJsonFile.json" );
        REQUIRE( !result );
    }
    SECTION( "loading empty graph" ) {
        nlohmann::json emptyJson = {};
        auto result              = g.fromJson( emptyJson );
        REQUIRE( result );
    }
    SECTION( "missing instance" ) {
        nlohmann::json noId = { { "model", { "name", "Core DataflowGraph" } } };
        auto result         = g.fromJson( noId );
        REQUIRE( !result );
    }
    SECTION( "missing model" ) {
        nlohmann::json noModel = { { "instance", "No model in this node" } };
        auto result            = g.fromJson( noModel );
        REQUIRE( !result );
    }
    SECTION( "missing instance data -> loads an empty graph" ) {
        nlohmann::json noGraph = { { "instance", "Missing instance data for model" },
                                   { "model", { "name", "Core DataflowGraph" } } };
        auto result            = g.fromJson( noGraph );
        REQUIRE( result );
    }
    SECTION( "trying to instance an unknown node type" ) {
        nlohmann::json NotANode = {
            { "instance", "graph with unknown node" },
            { "model",
              { { "name", "Core DataflowGraph" },
                { "graph",
                  { { "nodes",
                      { { { "instance", "NotANode" },
                          { "model", { { "name", "NotANode" } } } } } } } } } } };
        auto result = g.fromJson( NotANode );
        REQUIRE( !result );
    }
    SECTION( "trying to instance an unknown node type" ) {
        nlohmann::json NoModelName = {
            { "instance", "graph with missing node model information" },
            { "model",
              { { "name", "Core DataflowGraph" },
                { "graph",
                  { { "nodes",
                      { { { "instance", "Unknown model" },
                          { "model", { { "extra", "NotaTypeName" } } } } } } } } } } };
        auto result = g.fromJson( NoModelName );
        REQUIRE( !result );
    }
    SECTION( "trying to instance an unknown node type" ) {
        nlohmann::json noInstanceIdentification = {
            { "instance", "graph with missing node model information" },
            { "model",
              { { "name", "Core DataflowGraph" },
                { "graph",
                  { { "nodes", { { { "model", { { "name", "Source<float>" } } } } } } } } } } };
        auto result = g.fromJson( noInstanceIdentification );
        REQUIRE( !result );
    }
    SECTION( "errors in the connection description" ) {
        nlohmann::json reusingNodeIdentification = {
            { "instance", "graph with wrong connection" },
            { "model",
              { { "name", "Core DataflowGraph" },
                { "graph",
                  { { "nodes",
                      { { { "instance", "Source" }, { "model", { { "name", "Source<float>" } } } },
                        { { "instance", "Source" }, { "model", { { "name", "Sink<int>" } } } } } },
                    { "connections", { { { "out_node", "wrongId" } } } } } } } } };
        auto result = g.fromJson( reusingNodeIdentification );
        REQUIRE( !result );
    }
    SECTION( "wrong connection 0" ) {
        nlohmann::json reusingNodeIdentification = {
            { "instance", "graph with wrong connection" },
            { "model",
              { { "name", "Core DataflowGraph" },
                { "graph",
                  { { "nodes",
                      { { { "model", { { "name", "Source<float>" } } } },
                        { { "model", { { "name", "Sink<int>" } } } } } },
                    { "connections", { { { "out_node", "wrongId" } } } } } } } } };
        auto result = g.fromJson( reusingNodeIdentification );
        REQUIRE( !result );
    }
    SECTION( "wrong connection 1" ) {
        nlohmann::json wrongConnection = {
            { "instance", "graph with wrong connection" },
            { "model",
              { { "name", "Core DataflowGraph" },
                { "graph",
                  { { "nodes",
                      { { { "instance", "SourceFloat" },
                          { "model", { { "name", "Source<float>" } } } },
                        { { "instance", "SinkInt" }, { "model", { { "name", "Sink<int>" } } } } } },
                    { "connections", { { { "out_node", "wrongId" } } } } } } } } };
        auto result = g.fromJson( wrongConnection );
        REQUIRE( !result );
    }
    SECTION( "wrong connection 2" ) {
        nlohmann::json wrongConnection = {
            { "instance", "Test Graph Inline" },
            { "model",
              { { "name", "Core DataflowGraph" },
                { "graph",
                  { { "nodes",
                      { { { "instance", "SourceFloat" },
                          { "model", { { "name", "Source<float>" } } } },
                        { { "instance", "SinkInt" }, { "model", { { "name", "Sink<int>" } } } } } },
                    { "connections",
                      { { { "out_node", "SourceFloat" }, { "out_index", 2 } } } } } } } } };
        auto result = g.fromJson( wrongConnection );
        REQUIRE( !result );
    }
    SECTION( "wrong connection 3" ) {
        nlohmann::json wrongConnection = {
            { "instance", "Test Graph Inline" },
            { "model",
              { { "name", "Core DataflowGraph" },
                { "graph",
                  { { "nodes",
                      { { { "instance", "SourceFloat" },
                          { "model", { { "name", "Source<float>" } } } },
                        { { "instance", "SinkInt" }, { "model", { { "name", "Sink<int>" } } } } } },
                    { "connections",
                      { { { "out_node", "SourceFloat" },
                          { "out_index", 0 },
                          { "in_node", "Sink" },
                          { "in_port", "from" } } } } } } } } };
        auto result = g.fromJson( wrongConnection );
        REQUIRE( !result );
    }
    SECTION( "wrong connection 4" ) {
        nlohmann::json wrongConnection = {
            { "instance", "Test Graph Inline" },
            { "model",
              { { "name", "Core DataflowGraph" },
                { "graph",
                  { { "nodes",
                      { { { "instance", "SourceFloat" },
                          { "model", { { "name", "Source<float>" } } } },
                        { { "instance", "SinkInt" }, { "model", { { "name", "Sink<int>" } } } } } },
                    { "connections",
                      { { { "out_node", "SourceFloat" },
                          { "out_index", 0 },
                          { "in_node", "SinkInt" },
                          { "in_port", "from" } } } } } } } } };
        auto result = g.fromJson( wrongConnection );
        REQUIRE( !result );
    }
    SECTION( "correct graph" ) {
        nlohmann::json goodSimpleGraph = {
            { "instance", "Test Graph Inline" },
            { "model",
              { { "name", "Core DataflowGraph" },
                { "graph",
                  { { "nodes",
                      { { { "instance", "SourceFloat" },
                          { "model", { { "name", "Source<float>" } } } },
                        { { "instance", "SinkFloat" },
                          { "model", { { "name", "Sink<float>" } } } } } },
                    { "connections",
                      { { { "out_node", "SourceFloat" },
                          { "out_port", "to" },
                          { "in_node", "SinkFloat" },
                          { "in_index", 0 } } } } } } } } };

        REQUIRE( g.fromJson( goodSimpleGraph ) );

        // trying to add a duplicated node
        auto duplicatedNodeName =
            std::make_shared<Sources::SingleDataSourceNode<float>>( "SourceFloat" );
        auto r = g.addNode( duplicatedNodeName );
        REQUIRE( !r );

        // get unknown node
        auto sinkFloatNode = g.getNode( "Sink" );
        REQUIRE( sinkFloatNode == nullptr );
        // get known node
        sinkFloatNode = g.getNode( "SinkFloat" );
        REQUIRE( sinkFloatNode != nullptr );

        auto sourceFloatNode = g.getNode( "SourceFloat" );
        REQUIRE( sourceFloatNode != nullptr );

        auto sourceIntNode = std::make_shared<Sources::IntSource>( "SourceInt" );
        auto sinkIntNode   = std::make_shared<Sinks::IntSink>( "SinkInt" );
        // node not found
        REQUIRE( !g.removeLink( sinkIntNode, "from" ) );

        // "from" node not found
        REQUIRE( !g.addLink( sourceIntNode, "out", sinkIntNode, "in" ) );

        REQUIRE( g.addNode( sourceIntNode ) );
        // "to" node not found
        REQUIRE( !g.addLink( sourceIntNode, "out", sinkIntNode, "in" ) );

        REQUIRE( g.addNode( sinkIntNode ) );

        // output port of "from" node not found
        REQUIRE( !g.addLink( sourceIntNode, "out", sinkIntNode, "in" ) );

        // input port of "to" node not found
        REQUIRE( !g.addLink( sourceIntNode, "to", sinkIntNode, "in" ) );

        // link OK
        REQUIRE( g.addLink( sourceIntNode, "to", sinkIntNode, "from" ) );

        // from port of "to" node already linked
        REQUIRE( !g.addLink( sourceIntNode, "to", sinkIntNode, "from" ) );

        // type mismatch
        REQUIRE( !g.addLink( sourceIntNode, "to", sinkFloatNode, "from" ) );

        // protect the graph to prevent link removal
        g.setNodesAndLinksProtection( true );
        REQUIRE( g.getNodesAndLinksProtection() );
        // unable to remove links from protected graph ...
        REQUIRE( !g.removeLink( sinkIntNode, "from" ) );
        g.setNodesAndLinksProtection( false );
        REQUIRE( !g.getNodesAndLinksProtection() );
        // remove link OK

        REQUIRE( g.removeLink( sinkIntNode, "from" ) );

        // input port not found to remove its link

        REQUIRE( !g.removeLink( sinkIntNode, "in" ) );

        // compile the graph
        inspectGraph( g );

        REQUIRE( g.compile() );
        REQUIRE( g.isCompiled() );

        // clear the graph
        g.clearNodes();

        // Nodes can't be found
        auto nullNode = g.getNode( "SourceInt" );
        REQUIRE( nullNode == nullptr );
        nullNode = g.getNode( "SinkInt" );
        REQUIRE( nullNode == nullptr );
        // Nodes can't be found
        nullNode = g.getNode( "SourceFloat" );
        REQUIRE( nullNode == nullptr );
        nullNode = g.getNode( "SinkFloat" );
        REQUIRE( nullNode == nullptr );
    }
    // destroy everything
    g.destroy();
}

TEST_CASE( "Node failed exeution" ) {
    DataflowGraph g( "Test Graph" );
    auto sourceIntNode = g.addNode<Sources::IntSource>( "SourceInt" );
    auto sinkIntNode   = g.addNode<Sinks::IntSink>( "SinkInt" );
    class FailFunction : public Functionals::TransformInt
    {
      public:
        explicit FailFunction( const std::string& instanceName ) : FunctionNode( instanceName ) {}
        bool execute() { return false; }
        static const std::string& getTypename() {
            static std::string demangledName = std::string { "FailFunction" };
            return demangledName;
        }
    };
    auto failNode = g.addNode<FailFunction>( "FailNode" );

    REQUIRE( g.addLink( sourceIntNode, "to", failNode, "in" ) );
    REQUIRE( g.addLink( failNode, "out", sinkIntNode, "from" ) );
    REQUIRE( g.compile() );
    REQUIRE( !g.execute() );
}

TEST_CASE( "Inspection of a graph" ) {
    auto coreFactory = NodeFactoriesManager::getDataFlowBuiltInsFactory();

    using namespace Ra::Dataflow::Core;

    // add some nodes to factory
    using FloatFilterSource    = Sources::FunctionSourceNode<float, const float&>;
    using FloatFunctionSource  = Sources::FunctionSourceNode<float, const float&, const float&>;
    using FloatPredicateSource = Sources::FunctionSourceNode<bool, const float&, const float&>;
    using ReduceNode           = Functionals::ReduceNode<Ra::Core::VectorArray<float>>;
    using TransformNode        = Functionals::TransformNode<Ra::Core::VectorArray<float>>;
    using FloatVectorSource    = Sources::SingleDataSourceNode<Ra::Core::VectorArray<float>>;
    REGISTER_TYPE_TO_FACTORY( coreFactory, FloatVectorSource, Sources );
    REGISTER_TYPE_TO_FACTORY( coreFactory, FloatFilterSource, Sources );
    REGISTER_TYPE_TO_FACTORY( coreFactory, FloatFunctionSource, Sources );
    REGISTER_TYPE_TO_FACTORY( coreFactory, FloatPredicateSource, Sources );
    REGISTER_TYPE_TO_FACTORY( coreFactory, ReduceNode, Functionals );
    REGISTER_TYPE_TO_FACTORY( coreFactory, TransformNode, Functionals );

    std::cout << "Loading graph data/Dataflow/ExampleGraph.json\n";
    auto g = DataflowGraph::loadGraphFromJsonFile( "data/Dataflow/ExampleGraph.json" );
    REQUIRE( g );
    // Factories used by the graph
    const auto& nodes = g->getNodes();
    REQUIRE( nodes.size() == g->getNodesCount() );
    auto c = g->compile();
    REQUIRE( c == true );
    REQUIRE( g->isCompiled() );
    // Prints the graph content
    inspectGraph( *g );
    g->needsRecompile();
    REQUIRE( !g->isCompiled() );

    // removing the boolean sink from the graph
    auto n        = g->getNode( "validation value" );
    auto useCount = n.use_count();
    REQUIRE( n->getInstanceName() == "validation value" );
    c = g->removeNode( n );
    REQUIRE( c == true );
    REQUIRE( n );
    REQUIRE( n.use_count() == useCount - 1 );
    c = g->compile();
    REQUIRE( c == true );

    // Simplified graph after compilation
    auto& cn = g->getNodesByLevel();
    // the source "Validator" is no more in level 0 as it is not reachable from a sink in the
    // graph.
    auto found = std::find_if( cn[0].begin(), cn[0].end(), []( const auto& nn ) {
        return nn->getInstanceName() == "Validator";
    } );
    REQUIRE( found == cn[0].end() );

    // removing the source "Validator"
    n = g->getNode( "Validator" );
    REQUIRE( n->getInstanceName() == "Validator" );
    // protect the graph to prevent node removal
    g->setNodesAndLinksProtection( true );
    c = g->removeNode( n );
    REQUIRE( !c );
    g->setNodesAndLinksProtection( false );
    c = g->removeNode( n );
    REQUIRE( c );

    std::cout << "####### Graph after sink and source removal\n";
    inspectGraph( *g );
}
