#include <catch2/catch.hpp>

#include <iostream>

#include <Dataflow/Rendering/RenderingGraph.hpp>

using namespace Ra::Dataflow::Core;
using namespace Ra::Dataflow::Rendering;

TEST_CASE( "Dataflow/Rendering/RenderingGraph", "[Dataflow][Rendering][RenderingGraph]" ) {
#if defined( OS_LINUX )
    {
        // This is required on Linux to force loading the libDataflowRendering.so so that the
        // node factory for Rendering is initialized.
        // If not present, as no symbols are explicitly used from this lib, the linker
        // optimize out the lib.
        // All the test below use only the general interface of a DataflowGraph, the dependency to
        // symbols exported by the rendering lib is only managed by the node factory.
        RenderingGraph gr( "Forcing libDataflowRendering.so to be loaded" );
    }
#endif
    SECTION( "Loads and inspect a rendering graph graph" ) {
        auto g = DataflowGraph::loadGraphFromJsonFile( "data/Dataflow/fullRenderingGraph.json" );

        // Factories used by the graph
        auto factories = g->getNodeFactories();
        REQUIRE( factories != nullptr );
        std::cout << "Used factories by the graph \"" << g->getInstanceName() << "\" with type \""
                  << g->getTypeName() << "\" :\n";
        for ( const auto& f : *( factories.get() ) ) {
            std::cout << "\t" << f.first << "\n";
        }

        // Nodes of the graph
        auto nodes = g->getNodes();
        REQUIRE( nodes != nullptr );
        std::cout << "Nodes of the graph " << g->getInstanceName() << " (" << nodes->size()
                  << " nodes) " << g->getNodesCount() << ":\n";

        REQUIRE( nodes->size() == g->getNodesCount() );

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

        // describe the graph interface : inputs and outputs port of the whole graph (not of the
        // nodes)
        std::cout << "Inputs and output nodes of the graph " << g->getInstanceName() << " :\n";
        auto inputs = g->getAllDataSetters();
        std::cout << "\tInput ports (" << inputs.size() << ") are :\n";
        for ( auto& [ptrPort, portName, portType] : inputs ) {
            std::cout << "\t\t\"" << portName << "\" accepting type \"" << portType << "\"\n";
        }
        auto outputs = g->getAllDataGetters();
        std::cout << "\tOutput ports (" << outputs.size() << ") are :\n";
        for ( auto& [ptrPort, portName, portType] : outputs ) {
            std::cout << "\t\t\"" << portName << "\" generating type \"" << portType << "\"\n";
        }

// Can't compile the graph without OpenGL context (it's a rendering graph, compilation needs openGL
// context active)
#if 0
        // Nodes by level after the compilation
        auto c = g->compile();
        REQUIRE( c == true );
        auto cn = g->getNodesByLevel();
        std::cout << "Nodes of the graph, sorted by level when compiling the graph :\n";
        for ( size_t i = 0; i < cn->size(); ++i ) {
            std::cout << "\tLevel " << i << " :\n";
            for ( const auto n : ( *cn )[i] ) {
                std::cout << "\t\t\"" << n->getInstanceName() << "\"\n";
            }
        }
#endif
    }
}
