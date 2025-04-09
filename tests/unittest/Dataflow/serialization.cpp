#include <catch2/catch_test_macros.hpp>

#include <string>

#include <Core/Utils/StdFilesystem.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Functionals/Types.hpp>
#include <Dataflow/Core/Sinks/Types.hpp>
#include <Dataflow/Core/Sources/Types.hpp>

TEST_CASE( "Dataflow/Core/DataflowGraph/Serialization",
           "[unittests][Dataflow][Core][DataflowGraph]" ) {
    SECTION( "Execution and modification of a graph" ) {
        using namespace Ra::Dataflow::Core;
        using DataType = Scalar;
        DataflowGraph g { "original graph" };
        g.add_metadata( { { "extra", { { "info", "missing operators on functional node" } } } } );

        REQUIRE( g.metadata().contains( "extra" ) );
        REQUIRE( g.metadata()["extra"].contains( "info" ) );
        REQUIRE( g.metadata()["extra"]["info"] == "missing operators on functional node" );

        auto source_a                = g.add_node<Sources::SingleDataSourceNode<DataType>>( "a" );
        auto a                       = g.input_node_port( "a", "from" );
        auto source_b                = g.add_node<Sources::SingleDataSourceNode<DataType>>( "b" );
        auto b                       = g.input_node_port( "b", "from" );
        auto sink                    = g.add_node<Sinks::SinkNode<DataType>>( "r" );
        auto r                       = g.output_node_port( "r", "data" );
        using TestNode               = Functionals::BinaryOpNode<DataType, DataType, DataType>;
        TestNode::BinaryOperator add = []( TestNode::Arg1_type pa,
                                           TestNode::Arg2_type pb ) -> TestNode::Res_type {
            return pa + pb;
        };
        auto op_unique = g.add_node<TestNode>( "addition" );
        op_unique->setOperator( add );

        REQUIRE( g.add_link( source_a, "to", op_unique, "a" ) );
        REQUIRE( g.add_link( op_unique, "result", sink, "from" ) );
        REQUIRE( g.add_link( source_b, "to", op_unique, "b" ) );

        // execution of the original graph
        DataType x { 1_ra };
        a->setDefaultValue( x );
        DataType y { 2_ra };
        b->setDefaultValue( y );
        // Execute initial graph";
        REQUIRE( g.execute() );
        auto z = r->getData<DataType>();
        REQUIRE( z == x + y );

        // Save the graph
        std::string tmpdir { "tmpDir4Tests" };
        std::filesystem::create_directories( tmpdir );
        g.saveToJson( tmpdir + "/GraphSerializationTest.json" );
        g.destroy();
        // this does nothing as g was destroyed
        REQUIRE( g.execute() );

        // Create a new graph and load from the saved graph
        DataflowGraph g1 { "loaded graph" };
        REQUIRE( g1.loadFromJson( tmpdir + "/GraphSerializationTest.json" ) );

        // Setting the unserializable data on nodes (functions)
        auto addition = g1.node( "addition" );
        REQUIRE( addition != nullptr );
        REQUIRE( addition->model_name() == Functionals::BinaryOpScalar::node_typename() );
        auto typedAddition = std::dynamic_pointer_cast<Functionals::BinaryOpScalar>( addition );
        REQUIRE( typedAddition != nullptr );
        if ( typedAddition != nullptr ) { typedAddition->setOperator( add ); }

        // Execute loaded graph
        // Data delivered by the source nodes are the one saved by the original graph
        REQUIRE( g1.execute() );
        auto r_loaded  = g1.output_node_port( "r", "data" );
        auto& z_loaded = r_loaded->getData<DataType>();
        REQUIRE( z_loaded == z );

        auto a_loaded = g1.input_node_port( "a", "from" );
        auto b_loaded = g1.input_node_port( "b", "from" );
        DataType xp { 2_ra };
        a_loaded->setDefaultValue( xp );
        DataType yp { 3_ra };
        b_loaded->setDefaultValue( yp );
        REQUIRE( g1.execute() );
        REQUIRE( z_loaded == 5 );

        // change the data delivered by a
        auto loadedSource_a =
            std::dynamic_pointer_cast<Sources::SingleDataSourceNode<DataType>>( g1.node( "a" ) );
        Scalar newX = 3_ra;
        loadedSource_a->setData( newX );

        REQUIRE( g1.execute() );
        REQUIRE( z_loaded == 6 );
        std::filesystem::remove_all( tmpdir );
    }
}
