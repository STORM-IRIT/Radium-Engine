#include <Dataflow/RaDataflow.hpp>
#include <catch2/catch_test_macros.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Functionals/Types.hpp>
#include <Dataflow/Core/Node.hpp>
#include <Dataflow/Core/Sinks/SinkNode.hpp>
#include <Dataflow/Core/Sinks/Types.hpp>
#include <Dataflow/Core/Sources/Types.hpp>

#include <memory>
#include <string>

using std::string;

using namespace Ra::Dataflow::Core;

class DummyNode : public Node
{
  protected:
    DummyNode( const std::string& name, const std::string& type ) : Node( name, type ) {}

  public:
    DummyNode( const DummyNode& )            = delete;
    DummyNode( DummyNode&& )                 = delete;
    DummyNode& operator=( const DummyNode& ) = delete;
    DummyNode& operator=( DummyNode&& )      = delete;
    explicit DummyNode( const std::string& name ) : DummyNode( name, DummyNode::node_typename() ) {}
    static const std::string& node_typename() {
        static std::string name = string { "DummyNode" };
        return name;
    }
    bool execute() override { return is_initialized(); }

  protected:
    RA_NODE_PORT_IN( Scalar, port0 );
    RA_NODE_PORT_IN_WITH_DEFAULT( Scalar, port1, 2_ra );
    RA_NODE_PORT_OUT( Scalar, port0 );
    RA_NODE_PORT_OUT_WITH_DATA( Scalar, port1 );
    RA_NODE_PARAMETER( int, param0, 21 );
};

TEST_CASE( "Dataflow/Core/DummyNode", "[unittests][Dataflow][Core]" ) {

    DummyNode dummy { "dummy" };
    SECTION( "Before initialization, node is not init (and exec fail if it checks init" ) {
        REQUIRE( !dummy.is_initialized() );
        REQUIRE( !dummy.execute() );

        dummy.init();
        REQUIRE( dummy.is_initialized() );
        REQUIRE( dummy.execute() );
    }

    SECTION( "Two nodes are equals if same instance and type name" ) {
        DummyNode dummy_bis { "dummy" };
        REQUIRE( dummy == dummy_bis );
    }

    SECTION( "input ports are accessible by index or name or direct method" ) {
        for ( int index = 0; index <= 1; ++index ) {
            auto p_by_index = dummy.input_by_index( index );
            REQUIRE( p_by_index );
            auto port_name = std::string( "port" ) + std::to_string( index );
            REQUIRE( p_by_index->name() == port_name );
            auto p_by_name = dummy.input_by_name( port_name );
            REQUIRE( p_by_name.first == index );
            REQUIRE( p_by_name.second );
            REQUIRE( p_by_name.second->name() == port_name );
            REQUIRE( p_by_index == p_by_name.second );
            REQUIRE( p_by_index == dummy.port_by_index( "in", index ) );
        }
        REQUIRE( dummy.port_in_port0().get() == dummy.input_by_index( 0 ) );
        REQUIRE( dummy.port_in_port1().get() == dummy.input_by_index( 1 ) );
    }
    SECTION( "output ports are accesiible by index or name" ) {
        for ( int index = 0; index <= 1; ++index ) {
            auto p_by_index = dummy.output_by_index( index );
            REQUIRE( p_by_index );
            auto port_name = std::string( "port" ) + std::to_string( index );
            REQUIRE( p_by_index->name() == port_name );
            auto p_by_name = dummy.output_by_name( port_name );
            REQUIRE( p_by_name.first == index );
            REQUIRE( p_by_name.second );
            REQUIRE( p_by_name.second->name() == port_name );
            REQUIRE( p_by_index == p_by_name.second );
            REQUIRE( p_by_index == dummy.port_by_index( "out", index ) );
        }
        REQUIRE( dummy.port_out_port0().get() == dummy.output_by_index( 0 ) );
        REQUIRE( dummy.port_out_port1().get() == dummy.output_by_index( 1 ) );
    }
    SECTION( "port collection allows access to full port list" ) {
        REQUIRE( dummy.inputs().size() == 2 );
        REQUIRE( dummy.outputs().size() == 2 );
        for ( int index = 0; index <= 1; ++index ) {
            REQUIRE( dummy.inputs()[index].get() == dummy.input_by_index( index ) );
            REQUIRE( dummy.inputs()[index].get() == dummy.input_by_index<Scalar>( index ) );
            REQUIRE( dummy.outputs()[index].get() == dummy.output_by_index( index ) );
            REQUIRE( dummy.outputs()[index].get() == dummy.output_by_index<Scalar>( index ) );
        }
    }
    SECTION( "node name can be tweaked" ) {
        REQUIRE( dummy.model_name() == "DummyNode" );
        REQUIRE( dummy.node_typename() == "DummyNode" );
        REQUIRE( dummy.instance_name() == "dummy" );
        REQUIRE( dummy.display_name() == "dummy" );
        dummy.set_display_name( "dummy new name" );
        REQUIRE( dummy.display_name() == "dummy new name" );
        REQUIRE( dummy.model_name() == "DummyNode" );
        REQUIRE( dummy.node_typename() == "DummyNode" );
        REQUIRE( dummy.instance_name() == "dummy" );
        dummy.set_instance_name( "dummy new name" );
        REQUIRE( dummy.display_name() == "dummy new name" );
        REQUIRE( dummy.model_name() == "DummyNode" );
        REQUIRE( dummy.node_typename() == "DummyNode" );
        REQUIRE( dummy.instance_name() == "dummy new name" );
    }
    SECTION( "node metadata is empty at first, and can be edited" ) {
        REQUIRE( dummy.metadata().empty() );
        dummy.add_metadata( { { "comment", "test" }, { "foo", 2 } } );
        REQUIRE( !dummy.metadata().empty() );
        REQUIRE( dummy.metadata()["comment"] == "test" );
        REQUIRE( dummy.metadata()["foo"] == 2 );
        dummy.add_metadata( { { "comment", "new" }, { "bar", 3 } } );
        REQUIRE( dummy.metadata()["comment"] == "new" );
        REQUIRE( dummy.metadata()["foo"] == 2 );
        REQUIRE( dummy.metadata()["bar"] == 3 );
        // set to null removes from the metadata
        dummy.add_metadata( { { "comment", "null"_json } } );
        REQUIRE( !dummy.metadata().contains( "comment" ) );
        REQUIRE( dummy.metadata()["foo"] == 2 );
        REQUIRE( dummy.metadata()["bar"] == 3 );
        // "null"_json == nullptr
        dummy.add_metadata( { { "bar", nullptr } } );
        REQUIRE( !dummy.metadata().contains( "comment" ) );
        REQUIRE( dummy.metadata()["foo"] == 2 );
        REQUIRE( !dummy.metadata().contains( "bar" ) );
    }
    SECTION( "Node's default value is set and editable" ) {
        auto& def = dummy.port_in_port1()->default_value();
        REQUIRE( def == 2_ra );
        dummy.port_in_port1()->set_default_value( 3_ra );
        REQUIRE( def == 3_ra );
        def = 4_ra;
        REQUIRE( dummy.port_in_port1()->default_value() == 4_ra );
    }
    SECTION( "Node's input variables are input ports default values" ) {
        auto variables = dummy.input_variables();
        REQUIRE( variables.size() == 1 );
        auto port1 = variables.getVariable<std::reference_wrapper<Scalar>>( "port1" );
        REQUIRE( port1.get() == 2_ra );
        port1.get() = 3_ra;
        REQUIRE( dummy.port_in_port1()->default_value() == 3_ra );
        dummy.port_in_port1()->set_default_value( 4_ra );
        REQUIRE( port1.get() == 4_ra );
    }
    SECTION( "Node's parameters is accessible and editable with handle" ) {
        auto& parameters = dummy.parameters();
        REQUIRE( parameters.size() == 1 );
        //        auto& param0 = parameters.getVariable<int>( "param0" );
        auto handle0 = parameters.getVariableHandle<int>( "param0" );
        REQUIRE( handle0->second == 21 );
        REQUIRE( dummy.param_param0() == handle0->second );
        handle0->second = 42;
        REQUIRE( parameters.getVariableHandle<int>( "param0" )->second == handle0->second );
        REQUIRE( dummy.param_param0() == handle0->second );
    }
    SECTION( "Node's parameters is accessible and editable with ref" ) {
        auto& param0 = dummy.parameters().getVariable<int>( "param0" );
        REQUIRE( param0 == 21 );
        REQUIRE( dummy.param_param0() == param0 );
        param0 = 42;
        REQUIRE( dummy.parameters().getVariableHandle<int>( "param0" )->second == param0 );
        REQUIRE( dummy.param_param0() == param0 );
    }

    SECTION( "Missing instance fail from json" ) {
        nlohmann::json json = { {} };
        REQUIRE( !dummy.fromJson( json ) );
    }
    SECTION( "Missing model fail from json" ) {
        nlohmann::json json = { { "instance", "DummyNode" } };
        REQUIRE( !dummy.fromJson( json ) );
    }
    SECTION( "Valid json contains instance and model from json, display name set to model" ) {
        nlohmann::json json = { { "instance", "DummyNode" }, { "model", "DummyNode" } };
        REQUIRE( dummy.fromJson( json ) );
        REQUIRE( dummy.display_name() == "DummyNode" );
    }
    SECTION( "Valid json sets display name" ) {
        nlohmann::json json = {
            { "instance", "DummyNode" },
            { "model", { { "name", "DummyNode" }, { "display_name", "foo" } } } };
        REQUIRE( dummy.fromJson( json ) );
        REQUIRE( dummy.display_name() == "foo" );
    }
    SECTION( "Valid json sets metadata" ) {
        nlohmann::json json = { { "instance", "DummyNode" },
                                { "model", { { "name", "DummyNode" } } },
                                { "foo", "bar" } };
        REQUIRE( dummy.fromJson( json ) );
        REQUIRE( dummy.metadata()["foo"] == "bar" );
    }
}

TEST_CASE( "Dataflow/Core/LinkMandatory", "[unittests][Dataflow][Core]" ) {
    SECTION( "Port without default value needs to be linked" ) {
        using TestNode = Functionals::FunctionNode<int>;
        auto n         = std::make_shared<TestNode>( "test" );
        REQUIRE( n->port_in_data()->is_link_mandatory() );
        REQUIRE( !n->port_in_op()->is_link_mandatory() );
        n->port_in_data()->set_default_value( 5 );
        REQUIRE( !n->port_in_data()->is_link_mandatory() );
    }
}

TEST_CASE( "Dataflow/Core/Sink", "[unittests][Dataflow][Core]" ) {
    auto node = Sinks::SinkNode<int>( "sink" );
    REQUIRE( node.is_output() );
    REQUIRE( !node.execute() );
    REQUIRE( !node.is_initialized() );

    SECTION( "Without data, sink could not execute nor has data" ) {
        REQUIRE_THROWS_AS( node.data(), std::runtime_error );
        REQUIRE_THROWS_AS( node.data_reference(), std::runtime_error );
    }

    SECTION( "Set data needs also initilized to be executed" ) {
        node.port_in_from()->set_default_value( 1 );
        REQUIRE( !node.execute() );
        REQUIRE( !node.is_initialized() );
        node.init();
        REQUIRE( node.is_initialized() );
        REQUIRE( node.execute() );
    }
}
