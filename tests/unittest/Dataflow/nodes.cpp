#include <catch2/catch.hpp>

#include <string>
#include <vector>

#include <iostream>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Nodes/CoreBuiltInsNodes.hpp>

using namespace Ra::Dataflow::Core;
template <typename DataType_a, typename DataType_b = DataType_a, typename DataType_r = DataType_a>
std::tuple<DataflowGraph*, std::shared_ptr<PortBase>, std::shared_ptr<PortBase>, PortBase*>
createGraph(
    const std::string& name,
    typename Functionals::BinaryOpNode<DataType_a, DataType_b, DataType_r>::BinaryOperator f ) {
    using TestNode = Functionals::BinaryOpNode<DataType_a, DataType_b, DataType_r>;
    auto g         = new DataflowGraph { name };

    auto source_a = new Sources::SingleDataSourceNode<DataType_a>( "a" );
    g->addNode( source_a );
    auto a = g->getDataSetter( "a_to" );
    REQUIRE( a->getNode() == g );

    auto source_b = new Sources::SingleDataSourceNode<DataType_b>( "b" );
    g->addNode( source_b );
    auto b = g->getDataSetter( "b_to" );
    REQUIRE( b->getNode() == g );

    auto sink = new Sinks::SinkNode<DataType_r>( "r" );
    g->addNode( sink );
    auto r = g->getDataGetter( "r_from" );
    REQUIRE( r->getNode() == g );

    auto op = new TestNode( "operator", f );
    // op->setOperator( f );
    g->addNode( op );

    REQUIRE( g->addLink( source_a, "to", op, "a" ) );
    REQUIRE( g->addLink( op, "r", sink, "from" ) );
    REQUIRE( !g->compile() );
    // this will not execute the graph as it do not compiles
    g->execute();
    REQUIRE( !g->m_ready );
    // add missing link
    REQUIRE( g->addLink( source_b, "to", op, "b" ) );

    return { g, a, b, r };
}

TEST_CASE( "Dataflow/Core/Nodes", "[Dataflow][Core][Nodes]" ) {
    SECTION( "Operations on Scalar" ) {
        using DataType = Scalar;
        using TestNode = Functionals::BinaryOpNode<DataType, DataType, DataType>;
        typename TestNode::BinaryOperator add = []( typename TestNode::Arg1_type a,
                                                    typename TestNode::Arg2_type b ) ->
            typename TestNode::Res_type { return a + b; };

        auto [g, a, b, r] = createGraph<DataType>( "test scalar binary op", add );

        DataType x { 1_ra };
        a->setData( &x );
        REQUIRE( a->getData<DataType>() == x );

        DataType y { 2_ra };
        b->setData( &y );
        REQUIRE( b->getData<DataType>() == y );

        // As graph was modified since last compilation, this will recompile the graph
        g->execute();

        auto& z = r->getData<DataType>();
        REQUIRE( z == x + y );

        std::cout << x << " + " << y << " == " << z << "\n";

        g->destroy();
        delete g;
    }

    SECTION( "Operations on Vectors" ) {
        using DataType = Ra::Core::Vector3;
        using TestNode = Functionals::BinaryOpNode<DataType, DataType, DataType>;
        typename TestNode::BinaryOperator add = []( typename TestNode::Arg1_type a,
                                                    typename TestNode::Arg2_type b ) ->
            typename TestNode::Res_type { return a + b; };

        auto [g, a, b, r] = createGraph<DataType>( "test Vector3 binary op", add );

        DataType x { 1_ra, 2_ra, 3_ra };
        a->setData( &x );
        REQUIRE( a->getData<DataType>() == x );

        DataType y { 3_ra, 2_ra, 1_ra };
        b->setData( &y );
        REQUIRE( b->getData<DataType>() == y );

        g->execute();

        auto& z = r->getData<DataType>();
        REQUIRE( z == x + y );

        std::cout << "[" << x.transpose() << "] + [" << y.transpose() << "] == [" << z.transpose()
                  << "]\n";

        g->destroy();
        delete g;
    }

    SECTION( "Operations on VectorArrays" ) {
        using DataType = Ra::Core::VectorArray<Ra::Core::Vector2>;
        using TestNode = Functionals::BinaryOpNode<DataType, DataType, DataType>;
        typename TestNode::BinaryOperator add = []( typename TestNode::Arg1_type a,
                                                    typename TestNode::Arg2_type b ) ->
            typename TestNode::Res_type { return a + b; };

        auto [g, a, b, r] = createGraph<DataType>( "test Vector3 binary op", add );

        DataType x { { 1_ra, 2_ra }, { 3_ra, 4_ra } };
        a->setData( &x );
        REQUIRE( a->getData<DataType>() == x );

        DataType y { { 5_ra, 6_ra }, { 7_ra, 8_ra } };
        b->setData( &y );
        REQUIRE( b->getData<DataType>() == y );

        g->execute();

        auto& z = r->getData<DataType>();
        for ( size_t i = 0; i < z.size(); i++ ) {
            REQUIRE( z[i] == x[i] + y[i] );
        }

        std::cout << "{ ";
        for ( const auto& t : x ) {
            std::cout << "[" << t.transpose() << "] ";
        }
        std::cout << "} + { ";
        for ( const auto& t : y ) {
            std::cout << "[" << t.transpose() << "] ";
        }
        std::cout << "} = { ";
        for ( const auto& t : z ) {
            std::cout << "[" << t.transpose() << "] ";
        }
        std::cout << "}\n";

        g->destroy();
        delete g;
    }

    SECTION( "Operations between VectorArray and Scalar" ) {
        using DataType_a = Ra::Core::VectorArray<Ra::Core::Vector2>;
        using DataType_b = Scalar;
        // How to do this ? Eigen generates an error due to align allocation
        // using DataType_r = Ra::Core::VectorArray< decltype(  std::declval<Ra::Core::Vector2>() *
        // std::declval<Scalar>() ) >;
        using DataType_r = Ra::Core::VectorArray<Ra::Core::Vector2>;
        using TestNode   = Functionals::BinaryOpNode<DataType_a, DataType_b, DataType_r>;
        typename TestNode::BinaryOperator op = []( typename TestNode::Arg1_type a,
                                                   typename TestNode::Arg2_type b ) ->
            typename TestNode::Res_type { return a * b; };
        auto [g, a, b, r] = createGraph<DataType_a, DataType_b, DataType_r>(
            "test Vector2 x Scalar binary op", op );

        DataType_a x { { 1_ra, 2_ra }, { 3_ra, 4_ra } };
        a->setData( &x );
        REQUIRE( a->getData<DataType_a>() == x );

        DataType_b y { 5_ra };
        b->setData( &y );
        REQUIRE( b->getData<DataType_b>() == y );

        g->execute();

        auto& z = r->getData<DataType_r>();
        for ( size_t i = 0; i < z.size(); i++ ) {
            REQUIRE( z[i] == x[i] * y );
        }

        std::cout << "{ ";
        for ( const auto& t : x ) {
            std::cout << "[" << t.transpose() << "] ";
        }
        std::cout << "} * " << y << " = { ";
        for ( const auto& t : z ) {
            std::cout << "[" << t.transpose() << "] ";
        }
        std::cout << "}\n";

        // change operator
        auto opNode = dynamic_cast<TestNode*>( g->getNode( "operator" ) );
        REQUIRE( opNode != nullptr );
        if ( opNode ) {
            typename TestNode::BinaryOperator f = []( typename TestNode::Arg1_type a,
                                                      typename TestNode::Arg2_type b ) ->
                typename TestNode::Res_type { return a / b; };
            opNode->setOperator( f );
        }
        g->execute();

        for ( size_t i = 0; i < z.size(); i++ ) {
            REQUIRE( z[i] == x[i] / y );
        }

        std::cout << "{ ";
        for ( const auto& t : x ) {
            std::cout << "[" << t.transpose() << "] ";
        }
        std::cout << "} / " << y << " = { ";
        for ( const auto& t : z ) {
            std::cout << "[" << t.transpose() << "] ";
        }
        std::cout << "}\n";
        g->destroy();
        delete g;
    }

    SECTION( "Operations between Scalar and VectorArray" ) {
        using namespace Ra::Dataflow::Core;
        using DataType_a = Scalar;
        using DataType_b = Ra::Core::VectorArray<Ra::Core::Vector2>;
        using DataType_r = Ra::Core::VectorArray<Ra::Core::Vector2>;
        using TestNode   = Functionals::BinaryOpNode<DataType_a, DataType_b, DataType_r>;
        typename TestNode::BinaryOperator op = []( typename TestNode::Arg1_type a,
                                                   typename TestNode::Arg2_type b ) ->
            typename TestNode::Res_type { return a * b; };
        auto [g, a, b, r] = createGraph<DataType_a, DataType_b, DataType_r>(
            "test Vector2 x Scalar binary op", op );

        DataType_a x { 4_ra };
        a->setData( &x );
        REQUIRE( a->getData<DataType_a>() == x );

        DataType_b y { { 1_ra, 2_ra }, { 3_ra, 4_ra } };
        b->setData( &y );
        REQUIRE( b->getData<DataType_b>() == y );

        g->execute();

        auto& z = r->getData<DataType_r>();
        for ( size_t i = 0; i < z.size(); i++ ) {
            REQUIRE( z[i] == x * y[i] );
        }

        std::cout << x << " * { ";
        for ( const auto& t : y ) {
            std::cout << "[" << t.transpose() << "] ";
        }
        std::cout << "} = { ";
        for ( const auto& t : z ) {
            std::cout << "[" << t.transpose() << "] ";
        }
        std::cout << "}\n";
        g->destroy();
        delete g;
    }
}
