#include <catch2/catch_test_macros.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Functionals/FunctionNode.hpp>
#include <Dataflow/Core/Functionals/ReduceNode.hpp>
#include <Dataflow/Core/Functionals/TransformNode.hpp>
#include <Dataflow/Core/Functionals/Types.hpp>
#include <Dataflow/Core/Node.hpp>
#include <Dataflow/Core/NodeFactory.hpp>
#include <Dataflow/Core/PortIn.hpp>
#include <Dataflow/Core/PortOut.hpp>
#include <Dataflow/Core/Sinks/Types.hpp>
#include <Dataflow/Core/Sources/FunctionSource.hpp>
#include <Dataflow/Core/Sources/SingleDataSourceNode.hpp>
#include <Dataflow/Core/Sources/Types.hpp>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

using namespace Ra::Dataflow::Core;

void inspectGraph( const DataflowGraph& g ) {
    // Nodes of the graph
    const auto& nodes = g.nodes();
    std::cout << "Nodes of the graph " << g.instance_name() << " (" << nodes.size() << ") :\n";
    for ( const auto& n : nodes ) {
        std::cout << "\t\"" << n->instance_name() << "\" of type \"" << n->model_name() << "\"\n";
        // Inspect input, output and interfaces of the node
        std::cout << "\t\tInput ports :\n";
        for ( const auto& p : n->inputs() ) {
            std::cout << "\t\t\t\"" << p->name() << "\" with type " << p->port_typename();
            if ( p->is_linked() ) {
                std::cout << " linked to " << p->link()->node()->display_name() << " "
                          << p->link()->name();
            }
            std::cout << "\n";
        }
        std::cout << "\t\tOutput ports :\n";
        for ( const auto& p : n->outputs() ) {
            std::cout << "\t\t\t\"" << p->name() << "\" with type " << p->port_typename();
            std::cout << "\n";
        }
    }

    // Nodes by level after the compilation
    if ( g.is_compiled() ) {
        auto& cn = g.nodes_by_level();
        std::cout << "Nodes of the graph, sorted by level after compiling the graph :\n";
        for ( size_t i = 0; i < cn.size(); ++i ) {
            std::cout << "\tLevel " << i << " :\n";
            for ( const auto n : cn[i] ) {
                std::cout << "\t\t\"" << n->instance_name() << "\"\n";
            }
        }
    }
}
using PortIndex = Ra::Dataflow::Core::Node::PortIndex;

TEST_CASE( "Dataflow/Core/Graph/Json", "[unittests][Dataflow][Core][Graph]" ) {
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
                  { { "nodes", { { { "model", { { "name", "Source<Scalar>" } } } } } } } } } } };
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
                      { { { "instance", "Source" }, { "model", { { "name", "Source<Scalar>" } } } },
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
                      { { { "model", { { "name", "Source<Scalar>" } } } },
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
                      { { { "instance", "SourceScalar" },
                          { "model", { { "name", "Source<Scalar>" } } } },
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
                      { { { "instance", "SourceScalar" },
                          { "model", { { "name", "Source<Scalar>" } } } },
                        { { "instance", "SinkInt" }, { "model", { { "name", "Sink<int>" } } } } } },
                    { "connections",
                      { { { "out_node", "SourceScalar" }, { "out_index", 2 } } } } } } } } };
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
                      { { { "instance", "SourceScalar" },
                          { "model", { { "name", "Source<Scalar>" } } } },
                        { { "instance", "SinkInt" }, { "model", { { "name", "Sink<int>" } } } } } },
                    { "connections",
                      { { { "out_node", "SourceScalar" },
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
                      { { { "instance", "SourceScalar" },
                          { "model", { { "name", "Source<Scalar>" } } } },
                        { { "instance", "SinkInt" }, { "model", { { "name", "Sink<int>" } } } } } },
                    { "connections",
                      { { { "out_node", "SourceScalar" },
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
                      { { { "instance", "SourceScalar" },
                          { "model", { { "name", "Source<Scalar>" } } } },
                        { { "instance", "SinkScalar" },
                          { "model", { { "name", "Sink<Scalar>" } } } } } },
                    { "connections",
                      { { { "out_node", "SourceScalar" },
                          { "out_port", "to" },
                          { "in_node", "SinkScalar" },
                          { "in_index", 0 } } } } } } } } };

        REQUIRE( g.fromJson( goodSimpleGraph ) );

        // trying to add a duplicated node
        auto duplicatedNodeName =
            std::make_shared<Sources::SingleDataSourceNode<Scalar>>( "SourceScalar" );
        REQUIRE( !g.add_node( duplicatedNodeName ) );
        REQUIRE( !g.add_node<Sources::SingleDataSourceNode<Scalar>>( "SourceScalar" ) );

        // get unknown node
        auto sinkScalarNode = g.node( "Sink" );
        REQUIRE( sinkScalarNode == nullptr );
        // get known node
        sinkScalarNode = g.node( "SinkScalar" );
        REQUIRE( sinkScalarNode != nullptr );

        auto sourceScalarNode = g.node( "SourceScalar" );
        REQUIRE( sourceScalarNode != nullptr );

        auto sourceIntNode = std::make_shared<Sources::IntSource>( "SourceInt" );
        auto sinkIntNode   = std::make_shared<Sinks::IntSink>( "SinkInt" );
        // node not found
        REQUIRE( !g.remove_link( sinkIntNode, "from" ) );

        REQUIRE( !g.add_link( sourceIntNode, "to", sinkIntNode, "from" ) );
        REQUIRE( !g.can_link( sourceIntNode, PortIndex { 0 }, sinkIntNode, PortIndex { 0 } ) );
        REQUIRE( !g.add_link( sourceIntNode, PortIndex { 0 }, sinkIntNode, PortIndex { 0 } ) );

        REQUIRE( g.add_node( sourceIntNode ) );
        REQUIRE( !g.add_link( sourceIntNode, "to", sinkIntNode, "from" ) );
        REQUIRE( !g.can_link( sourceIntNode, PortIndex { 0 }, sinkIntNode, PortIndex { 0 } ) );
        REQUIRE( !g.add_link( sourceIntNode, PortIndex { 0 }, sinkIntNode, PortIndex { 0 } ) );

        REQUIRE( g.add_node( sinkIntNode ) );

        // output port of "in" node not found
        // input port of "to" node not found
        REQUIRE( !g.add_link( sourceIntNode, "out", sinkIntNode, "from" ) );
        REQUIRE( !g.add_link( sourceIntNode, PortIndex { 10 }, sinkIntNode, PortIndex { 0 } ) );
        REQUIRE( !g.add_link( sourceIntNode, PortIndex { 0 }, sinkIntNode, PortIndex { 10 } ) );
        REQUIRE( !g.can_link( sourceIntNode, PortIndex { 10 }, sinkIntNode, PortIndex { 0 } ) );
        REQUIRE( !g.can_link( sourceIntNode, PortIndex { 0 }, sinkIntNode, PortIndex { 10 } ) );
        REQUIRE( !g.add_link( sourceIntNode, "to", sinkIntNode, "in" ) );

        // link OK
        REQUIRE( g.add_link( sourceIntNode, "to", sinkIntNode, "from" ) );

        // from port of "to" node already linked
        REQUIRE( !g.add_link( sourceIntNode, "to", sinkIntNode, "from" ) );

        // type mismatch
        REQUIRE( !g.add_link( sourceIntNode, "to", sinkScalarNode, "from" ) );

        // protect the graph to prevent link removal
        g.setNodesAndLinksProtection( true );
        REQUIRE( g.nodesAndLinksProtection() );
        // unable to remove links from protected graph ...
        REQUIRE( !g.remove_link( sinkIntNode, "from" ) );
        g.setNodesAndLinksProtection( false );
        REQUIRE( !g.nodesAndLinksProtection() );
        // remove link OK

        REQUIRE( g.remove_link( sinkIntNode, "from" ) );
        // input port not found to remove its link
        REQUIRE( !g.remove_link( sinkIntNode, "in" ) );
        REQUIRE( !g.remove_link( sinkIntNode, PortIndex { 0 } ) );
        REQUIRE( g.add_link( sourceIntNode, PortIndex { 0 }, sinkIntNode, PortIndex { 0 } ) );
        REQUIRE( !g.remove_link( sinkIntNode, PortIndex { 10 } ) );
        REQUIRE( g.remove_link( sinkIntNode, PortIndex { 0 } ) );

        // compile the graph
        REQUIRE( g.compile() );
        REQUIRE( g.is_compiled() );
        inspectGraph( g );

        // clear the graph
        g.clear_nodes();

        // Nodes can't be found
        auto nullNode = g.node( "SourceInt" );
        REQUIRE( nullNode == nullptr );
        nullNode = g.node( "SinkInt" );
        REQUIRE( nullNode == nullptr );
        // Nodes can't be found
        nullNode = g.node( "SourceScalar" );
        REQUIRE( nullNode == nullptr );
        nullNode = g.node( "SinkScalar" );
        REQUIRE( nullNode == nullptr );
    }
    // destroy everything
    g.destroy();
}

TEST_CASE( "Dataflow/Core/Graph/Node failed execution", "[unittests][Dataflow][Core][Graph]" ) {
    DataflowGraph g( "Test Graph" );
    auto sourceIntNode = g.add_node<Sources::IntSource>( "SourceInt" );
    auto sinkIntNode   = g.add_node<Sinks::IntSink>( "SinkInt" );
    class FailFunction : public Functionals::TransformInt
    {
      public:
        explicit FailFunction( const std::string& instanceName ) : FunctionNode( instanceName ) {}
        bool execute() { return false; }
    };
    auto failNode = g.add_node<FailFunction>( "FailNode" );

    REQUIRE( g.add_link( sourceIntNode, "to", failNode, "data" ) );
    REQUIRE( g.add_link( failNode, "result", sinkIntNode, "from" ) );
    REQUIRE( g.compile() );
    REQUIRE( !g.execute() );
}

TEST_CASE( "Dataflow/Core/Graph/Inspection of a graph", "[unittests][Dataflow][Core][Graph]" ) {
    auto coreFactory = NodeFactoriesManager::default_factory();

    using namespace Ra::Dataflow::Core;

    // add some nodes to factory
    using ScalarFilterSource    = Sources::FunctionSourceNode<Scalar, const Scalar&>;
    using ScalarFunctionSource  = Sources::FunctionSourceNode<Scalar, const Scalar&, const Scalar&>;
    using ScalarPredicateSource = Sources::FunctionSourceNode<bool, const Scalar&, const Scalar&>;
    using ReduceNode            = Functionals::ReduceNode<Ra::Core::VectorArray<Scalar>>;
    using TransformNode         = Functionals::TransformNode<Ra::Core::VectorArray<Scalar>>;
    using ScalarVectorSource    = Sources::SingleDataSourceNode<Ra::Core::VectorArray<Scalar>>;
    REGISTER_TYPE_TO_FACTORY( coreFactory, ScalarVectorSource, Sources );
    REGISTER_TYPE_TO_FACTORY( coreFactory, ScalarFilterSource, Sources );
    REGISTER_TYPE_TO_FACTORY( coreFactory, ScalarFunctionSource, Sources );
    REGISTER_TYPE_TO_FACTORY( coreFactory, ScalarPredicateSource, Sources );
    REGISTER_TYPE_TO_FACTORY( coreFactory, ReduceNode, Functionals );
    REGISTER_TYPE_TO_FACTORY( coreFactory, TransformNode, Functionals );

    std::cout << "Loading graph data/Dataflow/ExampleGraph.json\n";

    REQUIRE( !DataflowGraph::loadGraphFromJsonFile( "data/Dataflow/NotAJsonFile.json" ) );
    REQUIRE( !DataflowGraph::loadGraphFromJsonFile( "data/Dataflow/InvalidGraph.json" ) );
    REQUIRE( !DataflowGraph::loadGraphFromJsonFile( "data/Dataflow/UnknownTypeGraph.json" ) );
    REQUIRE( !DataflowGraph::loadGraphFromJsonFile( "data/Dataflow/Node.json" ) );

    auto g = DataflowGraph::loadGraphFromJsonFile( "data/Dataflow/ExampleGraph.json" );
    REQUIRE( g );
    // Factories used by the graph
    const auto& nodes = g->nodes();
    REQUIRE( nodes.size() == g->node_count() );

    REQUIRE( g->compile() );
    REQUIRE( g->is_compiled() );
    // Prints the graph content
    inspectGraph( *g );
    g->needs_recompile();
    REQUIRE( !g->is_compiled() );

    // removing the boolean sink from the graph
    auto n        = g->node( "validation value" );
    auto useCount = n.use_count();
    REQUIRE( n->instance_name() == "validation value" );

    REQUIRE( g->remove_node( n ) );
    REQUIRE( n );
    REQUIRE( n.use_count() == useCount - 1 );

    REQUIRE( g->compile() );

    // Simplified graph after compilation
    auto& cn = g->nodes_by_level();
    // the source "Validator" is no more in level 0 as it is not reachable from a sink in the
    // graph.
    auto found = std::find_if( cn[0].begin(), cn[0].end(), []( const auto& nn ) {
        return nn->instance_name() == "Validator";
    } );
    REQUIRE( found == cn[0].end() );

    // removing the source "Validator"
    n = g->node( "Validator" );
    REQUIRE( n->instance_name() == "Validator" );
    // protect the graph to prevent node removal
    g->setNodesAndLinksProtection( true );
    REQUIRE( !g->remove_node( n ) );
    g->setNodesAndLinksProtection( false );
    REQUIRE( g->remove_node( n ) );

    std::cout << "####### Graph after sink and source removal\n";
    inspectGraph( *g );
}

//! [Create a source to sink graph for type T]
using namespace Ra::Dataflow::Core;
template <typename DataType_a, typename DataType_b = DataType_a, typename DataType_r = DataType_a>
std::tuple<DataflowGraph*, Node::PortBaseInRawPtr, Node::PortBaseInRawPtr, Node::PortBaseOutRawPtr>
createGraph(
    const std::string& name,
    typename Functionals::BinaryOpNode<DataType_a, DataType_b, DataType_r>::BinaryOperator f ) {
    using TestNode = Functionals::BinaryOpNode<DataType_a, DataType_b, DataType_r>;
    auto g         = new DataflowGraph { name };

    auto source_a = std::make_shared<Sources::SingleDataSourceNode<DataType_a>>( "a" );
    g->add_node( source_a );
    auto a = g->input_node_port( "a", "from" );
    REQUIRE( a->node() == source_a.get() );

    auto source_b = std::make_shared<Sources::SingleDataSourceNode<DataType_b>>( "b" );
    g->add_node( source_b );
    auto b = g->input_node_port( "b", "from" );
    REQUIRE( b->node() == source_b.get() );

    auto sink = std::make_shared<Sinks::SinkNode<DataType_r>>( "r" );
    g->add_node( sink );
    auto r = g->output_node_port( "r", "data" );
    REQUIRE( r->node() == sink.get() );

    auto op = std::make_shared<TestNode>( "operator", f );
    // op->setOperator( f );
    g->add_node( op );

    REQUIRE( g->add_link( source_a, "to", op, "a" ) );
    REQUIRE( g->add_link( op, "result", sink, "from" ) );
    REQUIRE( !g->compile() );
    // this will not execute the graph as it does not compile
    REQUIRE( !g->execute() );
    REQUIRE( !g->is_compiled() );
    // add missing link
    REQUIRE( g->add_link( source_b, "to", op, "b" ) );

    return { g, a, b, r };
}

TEST_CASE( "Dataflow/Core/Nodes", "[unittests][Dataflow][Core][Nodes]" ) {
    SECTION( "Operations on Scalar" ) {
        using DataType = Scalar;
        using TestNode = Functionals::BinaryOpNode<DataType, DataType, DataType>;
        typename TestNode::BinaryOperator add = []( typename TestNode::Arg1_type a,
                                                    typename TestNode::Arg2_type b ) ->
            typename TestNode::Res_type { return a + b; };

        auto [g, a, b, r] = createGraph<DataType>( "test scalar binary op", add );

        DataType x { 1_ra };

        a->set_default_value( x );
        REQUIRE( a->data<DataType>() == x );

        DataType y { 2_ra };
        b->set_default_value( y );
        REQUIRE( b->data<DataType>() == y );

        // As graph was modified since last compilation, this will recompile the graph
        g->execute();

        auto& z = r->data<DataType>();
        REQUIRE( z == x + y );
        // could not get data as other type.
        REQUIRE_THROWS( r->data<int>() );

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
        a->set_default_value( x );
        REQUIRE( a->data<DataType>() == x );

        DataType y { 3_ra, 2_ra, 1_ra };
        b->set_default_value( y );
        REQUIRE( b->data<DataType>() == y );

        g->execute();

        auto& z = r->data<DataType>();
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
        a->set_default_value( x );
        REQUIRE( a->data<DataType>() == x );

        DataType y { { 5_ra, 6_ra }, { 7_ra, 8_ra } };
        b->set_default_value( y );
        REQUIRE( b->data<DataType>() == y );

        g->execute();

        auto& z = r->data<DataType>();
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
        a->set_default_value( x );
        REQUIRE( a->data<DataType_a>() == x );

        DataType_b y { 5_ra };
        b->set_default_value( y );
        REQUIRE( b->data<DataType_b>() == y );

        g->execute();

        auto& z = r->data<DataType_r>();
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
        auto opNode = std::dynamic_pointer_cast<TestNode>( g->node( "operator" ) );
        REQUIRE( opNode != nullptr );
        if ( opNode ) {
            typename TestNode::BinaryOperator f = []( typename TestNode::Arg1_type arg1,
                                                      typename TestNode::Arg2_type arg2 ) ->
                typename TestNode::Res_type { return arg1 / arg2; };
            opNode->set_operator( f );
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
        a->set_default_value( x );
        REQUIRE( a->data<DataType_a>() == x );

        DataType_b y { { 1_ra, 2_ra }, { 3_ra, 4_ra } };
        b->set_default_value( y );
        REQUIRE( b->data<DataType_b>() == y );

        g->execute();

        auto& z = r->data<DataType_r>();
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

    SECTION( "Transform/reduce/filter/test" ) {
        //! [Create a complex transform/reduce graph]
        auto g           = new DataflowGraph( "Complex graph" );
        using VectorType = Ra::Core::VectorArray<Scalar>;

        // Source of a vector of Scalar : random vector
        auto nodeS = std::make_shared<Sources::ScalarArraySource>( "s" );

        // Source of an operator on scalars : f(x) = 2*x
        using DoubleFunction    = Sources::FunctionSourceNode<Scalar, const Scalar&>::function_type;
        DoubleFunction doubleMe = []( const Scalar& x ) -> Scalar { return 2_ra * x; };
        auto nodeD = std::make_shared<Sources::FunctionSourceNode<Scalar, const Scalar&>>( "d" );
        nodeD->set_data( doubleMe );

        // Source of a Scalar : mean neutral element 0_ra
        auto nodeN = std::make_shared<Sources::ScalarSource>( "n" );
        nodeN->set_data( 0_ra );

        // Source of a reduction operator : compute the mean using Welford online algo
        using ReduceOperator = Sources::FunctionSourceNode<Scalar, const Scalar&, const Scalar&>;
        struct MeanOperator {
            size_t n { 0 };
            Scalar operator()( const Scalar& m, const Scalar& x ) {
                return m + ( ( x - m ) / ( ++n ) );
            }
        };
        auto nodeM                      = std::make_shared<ReduceOperator>( "m" );
        ReduceOperator::function_type m = MeanOperator();

        // Reduce node : will compute the mean
        using MeanCalculator = Functionals::ReduceNode<VectorType>;
        auto meanCalculator  = std::make_shared<MeanCalculator>( "mean" );

        // Sink for the mean
        auto nodeR = std::make_shared<Sinks::ScalarSink>( "r" );

        // Transform operator, will double the vectors' values
        auto nodeT = std::make_shared<Functionals::TransformNode<VectorType>>( "twice" );

        // Will compute the mean on the doubled vector
        auto doubleMeanCalculator = std::make_shared<MeanCalculator>( "double mean" );

        // Sink for the double mean
        auto nodeRD = std::make_shared<Sinks::ScalarSink>( "rd" );

        // Source for a comparison functor , eg f(x, y) -> 2*x == y
        auto nodePred = std::make_shared<Sources::ScalarBinaryPredicateSource>( "predicate" );
        Sources::ScalarBinaryPredicateSource::function_type predicate =
            []( const Scalar& a, const Scalar& b ) -> bool { return 2_ra * a == b; };
        nodePred->set_data( predicate );

        // Boolean sink for the validation result
        auto sinkB = std::make_shared<Sinks::BooleanSink>( "test" );

        // Node for coparing the results of the computation graph
        auto validator =
            std::make_shared<Functionals::BinaryOpNode<Scalar, Scalar, bool>>( "validator" );

        REQUIRE( g->add_node( nodeS ) );
        REQUIRE( g->add_node( nodeD ) );
        REQUIRE( g->add_node( nodeN ) );
        REQUIRE( g->add_node( nodeM ) );
        REQUIRE( g->add_node( nodeR ) );
        REQUIRE( g->add_node( meanCalculator ) );
        REQUIRE( g->add_node( doubleMeanCalculator ) );
        REQUIRE( g->add_node( nodeT ) );
        REQUIRE( g->add_node( nodeRD ) );

        REQUIRE( g->add_link( nodeS, "to", meanCalculator, "data" ) );
        REQUIRE( g->add_link( nodeM, "to", meanCalculator, "op" ) );
        REQUIRE( g->add_link( nodeN, "to", meanCalculator, "init" ) );
        REQUIRE( g->add_link( meanCalculator, "result", nodeR, "from" ) );
        REQUIRE( g->add_link( nodeS, "to", nodeT, "data" ) );
        REQUIRE( g->add_link( nodeD, "to", nodeT, "op" ) );
        REQUIRE( g->add_link( nodeT, "result", doubleMeanCalculator, "data" ) );
        REQUIRE( g->add_link( doubleMeanCalculator, "result", nodeRD, "from" ) );
        REQUIRE( g->add_link( nodeM, "to", doubleMeanCalculator, "op" ) );

        REQUIRE( g->add_node( nodePred ) );
        REQUIRE( g->add_node( sinkB ) );
        REQUIRE( g->add_node( validator ) );
        REQUIRE( g->add_link( meanCalculator, "result", validator, "a" ) );
        REQUIRE( g->add_link( doubleMeanCalculator, "result", validator, "b" ) );
        REQUIRE( g->add_link( nodePred, "to", validator, "op" ) );
        REQUIRE( g->add_link( validator, "result", sinkB, "from" ) );

        auto input   = g->input_node_port( "s", "from" );
        auto output  = g->output_node_port( "r", "data" );
        auto outputD = g->output_node_port( "rd", "data" );
        auto outputB = g->output_node_port( "test", "data" );
        auto inputR  = g->input_node_port( "m", "from" );
        REQUIRE( inputR );

        // Inspect the graph interface : inputs and outputs port

        REQUIRE( g->compile() );

        // Set input/ouput data
        VectorType test;

        test.reserve( 10 );
        std::mt19937 gen( 0 );
        std::uniform_real_distribution<> dis( 0.0, 1.0 );
        // Fill the vector with random numbers between 0 and 1
        for ( size_t n = 0; n < test.capacity(); ++n ) {
            test.push_back( dis( gen ) );
        }
        input->set_default_value( test );

        // No need to do this as mean operator source has a copy of a functor
        ReduceOperator::function_type m1 = MeanOperator();
        inputR->set_default_value( m1 );

        g->execute();

        auto& result  = output->data<Scalar>();
        auto& resultD = outputD->data<Scalar>();
        auto& resultB = outputB->data<bool>();

        std::cout << "Computed mean ( ref ): " << result << "\n";
        std::cout << "Computed mean ( tra ): " << resultD << "\n";
        std::cout << std::boolalpha;
        std::cout << "Ratio  ( expected 2 ): " << resultD / result << " -- validator --> "
                  << resultB << "\n";

        std::cout << '\n';

        REQUIRE( resultD / result == 2_ra );
        REQUIRE( resultB );
        // uncomment this if you want to edit the generated graph with GraphEditor
        //        g->saveToJson( "Transform-reduce.json" );
        g->destroy();
        delete g;
        //! [Create a complex transform/reduce graph]
    }
}
