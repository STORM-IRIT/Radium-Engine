#include <catch2/catch.hpp>

#include <iostream>

#include <Dataflow/Core/DataflowGraph.hpp>
using namespace Ra::Dataflow::Core;

TEST_CASE( "Dataflow/Core/Graph", "[Dataflow][Core][Graph]" ) {
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
