#include <catch2/catch.hpp>

#include <string>
#include <vector>

#include <iostream>

#include <Core/Utils/StdFilesystem.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Nodes/Functionals/CoreDataFunctionals.hpp>
#include <Dataflow/Core/Nodes/Sinks/CoreDataSinks.hpp>
#include <Dataflow/Core/Nodes/Sources/CoreDataSources.hpp>

TEST_CASE( "Dataflow/Core/DataflowGraph", "[Dataflow][Core][DataflowGraph]" ) {
    SECTION( "Serialization of a graph" ) {

        using namespace Ra::Dataflow::Core;
        using DataType = Scalar;
        DataflowGraph g { "original graph" };

        auto source_a = new Sources::SingleDataSourceNode<DataType>( "a" );
        g.addNode( std::unique_ptr<Node>( source_a ) );
        auto a        = g.getDataSetter( "a_to" );
        auto source_b = new Sources::SingleDataSourceNode<DataType>( "b" );
        g.addNode( std::unique_ptr<Node>( source_b ) );
        auto b    = g.getDataSetter( "b_to" );
        auto sink = new Sinks::SinkNode<DataType>( "r" );
        g.addNode( std::unique_ptr<Node>( sink ) );
        auto r                       = g.getDataGetter( "r_from" );
        using TestNode               = Functionals::BinaryOpNode<DataType, DataType, DataType>;
        TestNode::BinaryOperator add = []( TestNode::Arg1_type a,
                                           TestNode::Arg2_type b ) -> TestNode::Res_type {
            return a + b;
        };
        auto op = new TestNode( "addition" );
        op->setOperator( add );
        g.addNode( std::unique_ptr<Node>( op ) );
        g.addLink( source_a, "to", op, "a" );
        g.addLink( op, "r", sink, "from" );
        g.addLink( source_b, "to", op, "b" );

        // execution of the original graph
        DataType x { 1_ra };
        a->setData( &x );
        DataType y { 2_ra };
        b->setData( &y );
        // Execute initial graph";
        g.execute();
        auto z = r->getData<DataType>();
        REQUIRE( z == x + y );

        // Save the graph
        std::string tmpdir { "tmpDir4Tests" };

        std::cout << "Graph tmp dir : " << tmpdir << "\n";
        std::filesystem::create_directories( tmpdir );
        g.saveToJson( tmpdir + "/GraphSerializationTest.json" );
        g.destroy();
        // this does nothing as g was destroyed
        g.execute();

        // Create a new graph and load from the saved graph
        DataflowGraph g1 { "loaded graph" };
        g1.loadFromJson( tmpdir + "/GraphSerializationTest.json" );

        // Setting the unserializable data on nodes (functions)
        auto addition = g1.getNode( "addition" );
        REQUIRE( addition != nullptr );
        REQUIRE( addition->getTypeName() == Functionals::BinaryOpScalar::getTypename() );
        auto typedAddition = dynamic_cast<Functionals::BinaryOpScalar*>( addition );
        REQUIRE( typedAddition != nullptr );
        if ( typedAddition != nullptr ) { typedAddition->setOperator( add ); }

        // Execute loaded graph
        // Data delivered by the source nodes are the one saved by the original graph
        g1.execute();
        auto r_loaded  = g1.getDataGetter( "r_from" );
        auto& z_loaded = r_loaded->getData<DataType>();
        REQUIRE( z_loaded == z );

        auto a_loaded = g1.getDataSetter( "a_to" );
        auto b_loaded = g1.getDataSetter( "b_to" );
        DataType xp { 2_ra };
        a_loaded->setData( &xp );
        DataType yp { 3_ra };
        b_loaded->setData( &yp );
        g1.execute();
        REQUIRE( z_loaded == 5 );

        // Reset sources to use the loaded data loaded
        g1.releaseDataSetter( "a_to" );
        g1.releaseDataSetter( "b_to" );
        g1.execute();
        REQUIRE( z_loaded == z );

        // reactivate the dataSetter and change the data delivered by a (copy data into a)
        g1.activateDataSetter( "b_to" );
        auto loadedSource_a =
            dynamic_cast<Sources::SingleDataSourceNode<DataType>*>( g1.getNode( "a" ) );
        Scalar newX = 3_ra;
        loadedSource_a->setData( &newX );
        g1.execute();
        REQUIRE( z_loaded == 6 );
        std::filesystem::remove_all( tmpdir );
    }
}
