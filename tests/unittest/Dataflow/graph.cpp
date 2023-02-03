#include <catch2/catch.hpp>

#include <iostream>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Nodes/Sinks/CoreDataSinks.hpp>
#include <Dataflow/Core/Nodes/Sources/CoreDataSources.hpp>

using namespace Ra::Dataflow::Core;

TEST_CASE( "Dataflow/Core/Graph", "[Dataflow][Core][Graph]" ) {
    SECTION( "Creation of a graph" ) {
        DataflowGraph g( "Test Graph" );
        auto result = g.loadFromJson( "data/Dataflow/NotAJsonFile.json" );
        REQUIRE( !result );
        nlohmann::json emptyJson = {};
        result                   = g.fromJson( emptyJson );
        REQUIRE( result );
        nlohmann::json noId = { { "model", { "name", "Core DataflowGraph" } } };
        result              = g.fromJson( noId );
        REQUIRE( !result );
        g.destroy();

        nlohmann::json noModel = { { "id", "{d3c4d86a-49d9-496e-b01b-1c142538d8ef}" } };
        result                 = g.fromJson( noModel );
        REQUIRE( !result );
        g.destroy();

        nlohmann::json noGraph = { { "id", "{d3c4d86a-49d9-496e-b01b-1c142538d8ef}" },
                                   { "model", { "name", "Core DataflowGraph" } } };
        result                 = g.fromJson( noGraph );
        REQUIRE( result );
        g.destroy();

        nlohmann::json wrongFactory = {
            { "id", "{d3c4d86a-49d9-496e-b01b-1c142538d8ef}" },
            { "model",
              { { "instance", "Test Graph Inline" },
                { "name", "Core DataflowGraph" },
                { "graph", { { "factories", { "NotAFactory" } } } } } } };
        result = g.fromJson( wrongFactory );
        REQUIRE( !result );
        g.destroy();

        nlohmann::json NotANode = { { "id", "{d3c4d86a-49d9-496e-b01b-1c142538d8ef}" },
                                    { "model",
                                      { { "instance", "Test Graph Inline" },
                                        { "name", "Core DataflowGraph" },
                                        { "graph",
                                          { { "factories", {} },
                                            { "nodes",
                                              { { { "id", "tototo" },
                                                  { "model",
                                                    { { "instance", "NotANode" },
                                                      { "name", "NotANode" } } } } } } } } } } };
        result                  = g.fromJson( NotANode );
        REQUIRE( !result );
        g.destroy();

        nlohmann::json wrongConnection = {
            { "id", "{d3c4d86a-49d9-496e-b01b-1c142538d8ef}" },
            { "model",
              { { "instance", "Test Graph Inline" },
                { "name", "Core DataflowGraph" },
                { "graph",
                  { { "factories", {} },
                    { "nodes",
                      { { { "id", "{1111111a-2222-3333-4444-555555555555}" },
                          { "model",
                            { { "instance", "SourceFloat" }, { "name", "Source<float>" } } } },
                        { { "id", "{1111111b-2222-3333-4444-555555555555}" },
                          { "model", { { "instance", "SinkInt" }, { "name", "Sink<int>" } } } } } },
                    { "connections", { { { "out_id", "wrongId" } } } } } } } } };
        result = g.fromJson( wrongConnection );
        REQUIRE( !result );
        g.destroy();

        wrongConnection = {
            { "id", "{d3c4d86a-49d9-496e-b01b-1c142538d8ef}" },
            { "model",
              { { "instance", "Test Graph Inline" },
                { "name", "Core DataflowGraph" },
                { "graph",
                  { { "factories", {} },
                    { "nodes",
                      { { { "id", "{1111111a-2222-3333-4444-555555555555}" },
                          { "model",
                            { { "instance", "SourceFloat" }, { "name", "Source<float>" } } } },
                        { { "id", "{1111111b-2222-3333-4444-555555555555}" },
                          { "model", { { "instance", "SinkInt" }, { "name", "Sink<int>" } } } } } },
                    { "connections",
                      { { { "out_id", "{1111111a-2222-3333-4444-555555555555}" },
                          { "out_index", 2 } } } } } } } } };
        result = g.fromJson( wrongConnection );
        REQUIRE( !result );
        g.destroy();

        wrongConnection = {
            { "id", "{d3c4d86a-49d9-496e-b01b-1c142538d8ef}" },
            { "model",
              { { "instance", "Test Graph Inline" },
                { "name", "Core DataflowGraph" },
                { "graph",
                  { { "factories", {} },
                    { "nodes",
                      { { { "id", "{1111111a-2222-3333-4444-555555555555}" },
                          { "model",
                            { { "instance", "SourceFloat" }, { "name", "Source<float>" } } } },
                        { { "id", "{1111111b-2222-3333-4444-555555555555}" },
                          { "model", { { "instance", "SinkInt" }, { "name", "Sink<int>" } } } } } },
                    { "connections",
                      { { { "out_id", "{1111111a-2222-3333-4444-555555555555}" },
                          { "out_index", 0 },
                          { "in_id", "wrongId" },
                          { "in_index", 2 } } } } } } } } };
        result = g.fromJson( wrongConnection );
        REQUIRE( !result );
        g.destroy();

        wrongConnection = {
            { "id", "{d3c4d86a-49d9-496e-b01b-1c142538d8ef}" },
            { "model",
              { { "instance", "Test Graph Inline" },
                { "name", "Core DataflowGraph" },
                { "graph",
                  { { "factories", {} },
                    { "nodes",
                      { { { "id", "{1111111a-2222-3333-4444-555555555555}" },
                          { "model",
                            { { "instance", "SourceFloat" }, { "name", "Source<float>" } } } },
                        { { "id", "{1111111b-2222-3333-4444-555555555555}" },
                          { "model", { { "instance", "SinkInt" }, { "name", "Sink<int>" } } } } } },
                    { "connections",
                      { { { "out_id", "{1111111a-2222-3333-4444-555555555555}" },
                          { "out_index", 0 },
                          { "in_id", "{1111111b-2222-3333-4444-555555555555}" },
                          { "in_index", 2 } } } } } } } } };
        result = g.fromJson( wrongConnection );
        REQUIRE( !result );
        g.destroy();

        wrongConnection = {
            { "id", "{d3c4d86a-49d9-496e-b01b-1c142538d8ef}" },
            { "model",
              { { "instance", "Test Graph Inline" },
                { "name", "Core DataflowGraph" },
                { "graph",
                  { { "factories", {} },
                    { "nodes",
                      { { { "id", "{1111111a-2222-3333-4444-555555555555}" },
                          { "model",
                            { { "instance", "SourceFloat" }, { "name", "Source<float>" } } } },
                        { { "id", "{1111111b-2222-3333-4444-555555555555}" },
                          { "model", { { "instance", "SinkInt" }, { "name", "Sink<int>" } } } } } },
                    { "connections",
                      { { { "out_id", "{1111111a-2222-3333-4444-555555555555}" },
                          { "out_index", 0 },
                          { "in_id", "{1111111b-2222-3333-4444-555555555555}" },
                          { "in_index", 0 } } } } } } } } };
        result = g.fromJson( wrongConnection );
        REQUIRE( !result );
        g.destroy();

        nlohmann::json goodSimpleGraph = {
            { "id", "{d3c4d86a-49d9-496e-b01b-1c142538d8ef}" },
            { "model",
              { { "instance", "Test Graph Inline" },
                { "name", "Core DataflowGraph" },
                { "graph",
                  { { "factories", {} },
                    { "nodes",
                      { { { "id", "{1111111a-2222-3333-4444-555555555555}" },
                          { "model",
                            { { "instance", "SourceFloat" }, { "name", "Source<float>" } } } },
                        { { "id", "{1111111b-2222-3333-4444-555555555555}" },
                          { "model",
                            { { "instance", "SinkFloat" }, { "name", "Sink<float>" } } } } } },
                    { "connections",
                      { { { "out_id", "{1111111a-2222-3333-4444-555555555555}" },
                          { "out_index", 0 },
                          { "in_id", "{1111111b-2222-3333-4444-555555555555}" },
                          { "in_index", 0 } } } } } } } } };
        result = g.fromJson( goodSimpleGraph );
        REQUIRE( result );

        // trying to add a duplicated node
        auto duplicatedNodeName = new Sources::SingleDataSourceNode<float>( "SourceFloat" );
        auto [r, rejectedNode]  = g.addNode( std::unique_ptr<Node>( duplicatedNodeName ) );
        REQUIRE( !r );
        REQUIRE( rejectedNode == duplicatedNodeName );
        delete duplicatedNodeName;

        auto sinkFloatNode = g.getNode( "Sink" );
        REQUIRE( sinkFloatNode == nullptr );
        sinkFloatNode = g.getNode( "SinkFloat" );
        REQUIRE( sinkFloatNode != nullptr );
        auto sourceFloatNode = g.getNode( "SourceFloat" );
        REQUIRE( sourceFloatNode != nullptr );

        auto sourceIntNode = new Sources::IntSource( "SourceInt" );
        auto sinkIntNode   = new Sinks::IntSink( "SinkInt" );
        // node not found
        result = g.removeLink( sinkIntNode, "from" );
        REQUIRE( !result );

        // "from" node not found
        result = g.addLink( sourceIntNode, "out", sinkIntNode, "in" );
        REQUIRE( !result );

        auto resPair = g.addNode( std::unique_ptr<Node>( sourceIntNode ) );
        REQUIRE( resPair.first );
        // "to" node not found
        result = g.addLink( sourceIntNode, "out", sinkIntNode, "in" );
        REQUIRE( !result );

        resPair = g.addNode( std::unique_ptr<Node>( sinkIntNode ) );
        REQUIRE( resPair.first );
        // output port of "from" node not found
        result = g.addLink( sourceIntNode, "out", sinkIntNode, "in" );
        REQUIRE( !result );

        // input port of "to" node not found
        result = g.addLink( sourceIntNode, "to", sinkIntNode, "in" );
        REQUIRE( !result );

        // link OK
        result = g.addLink( sourceIntNode, "to", sinkIntNode, "from" );
        REQUIRE( result );

        // from por of "to" node already linked
        result = g.addLink( sourceIntNode, "to", sinkIntNode, "from" );
        REQUIRE( !result );

        // type mismatch
        result = g.addLink( sourceIntNode, "to", sinkFloatNode, "from" );
        REQUIRE( !result );

        // remove link OK
        result = g.removeLink( sinkIntNode, "from" );
        REQUIRE( result );

        // input port not found to remove its link
        result = g.removeLink( sinkIntNode, "in" );
        REQUIRE( !result );

        // compile the graph
        result = g.compile();
        REQUIRE( result );

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

        // destroy everything
        g.destroy();
    }

    SECTION( "Inspection of a graph" ) {
        DataflowGraph g( "" );
        g.loadFromJson( "data/Dataflow/ExampleGraph.json" );

        // Factories used by the graph
        auto factories = g.getNodeFactories();
        REQUIRE( factories != nullptr );
        std::cout << "Used factories by the graph \"" << g.getInstanceName() << "\" with type \""
                  << g.getTypeName() << "\" :\n";
        for ( const auto& f : *( factories.get() ) ) {
            std::cout << "\t" << f.first << "\n";
        }

        // Nodes of the graph
        auto nodes = g.getNodes();
        REQUIRE( nodes != nullptr );
        std::cout << "Nodes of the graph " << g.getInstanceName() << " (" << nodes->size()
                  << ") :\n";
        REQUIRE( nodes->size() == g.getNodesCount() );
        for ( const auto& n : *( nodes ) ) {
            std::cout << "\t\"" << n->getInstanceName() << "\" of type \"" << n->getTypeName()
                      << "\"\n";
            // Inspect input, output and interfaces of the node
            std::cout << "\t\tInput ports :\n";
            for ( const auto& p : n->getInputs() ) {
                std::cout << "\t\t\t\"" << p->getName() << "\" with type " << p->getTypeName()
                          << "\n";
            }
            std::cout << "\t\tOutput ports :\n";
            for ( const auto& p : n->getOutputs() ) {
                std::cout << "\t\t\t\"" << p->getName() << "\" with type " << p->getTypeName()
                          << "\n";
            }
            std::cout << "\t\tInterface ports :\n";
            for ( const auto& p : n->getInterfaces() ) {
                std::cout << "\t\t\t\"" << p->getName() << "\" with type " << p->getTypeName()
                          << "\n";
            }
        }

        // Nodes by level after the compilation
        auto c = g.compile();
        REQUIRE( c == true );
        auto cn = g.getNodesByLevel();
        std::cout << "Nodes of the graph, sorted by level when compiling the graph :\n";
        for ( size_t i = 0; i < cn->size(); ++i ) {
            std::cout << "\tLevel " << i << " :\n";
            for ( const auto n : ( *cn )[i] ) {
                std::cout << "\t\t\"" << n->getInstanceName() << "\"\n";
            }
        }

        // describe the graph interface : inputs and outputs port of the whole graph (not of the
        // nodes)
        std::cout << "Inputs and output nodes of the graph " << g.getInstanceName() << " :\n";
        auto inputs = g.getAllDataSetters();
        std::cout << "\tInput ports (" << inputs.size() << ") are :\n";
        for ( auto& [ptrPort, portName, portType] : inputs ) {
            std::cout << "\t\t\"" << portName << "\" accepting type \"" << portType << "\"\n";
        }
        auto outputs = g.getAllDataGetters();
        std::cout << "\tOutput ports (" << outputs.size() << ") are :\n";
        for ( auto& [ptrPort, portName, portType] : outputs ) {
            std::cout << "\t\t\"" << portName << "\" generating type \"" << portType << "\"\n";
        }

        // removing the boolean sink from the graph
        auto n = g.getNode( "validation value" );
        REQUIRE( n->getInstanceName() == "validation value" );
        c = g.removeNode( n );
        REQUIRE( c == true );
        REQUIRE( n == nullptr );
        c = g.compile();
        REQUIRE( c == true );
        // Simplified graph after compilation
        cn = g.getNodesByLevel();
        std::cout << "Nodes of the graph, sorted by level, after sink deletion :\n";
        for ( size_t i = 0; i < cn->size(); ++i ) {
            std::cout << "\tLevel " << i << " :\n";
            for ( const auto nn : ( *cn )[i] ) {
                std::cout << "\t\t\"" << nn->getInstanceName() << "\"\n";
            }
        }

        // the source "Validator" is no more in level 0 as it is non reachable from a sink in the
        // graph.
        bool found = false;
        for ( const auto nn : ( *cn )[0] ) {
            if ( nn->getInstanceName() == "Validator" ) { found = true; }
        }
        REQUIRE( found == false );
    }
}
