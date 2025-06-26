#include "Dataflow/Core/Sources/SingleDataSourceNode.hpp"
#include <Dataflow/RaDataflow.hpp>
#include <catch2/catch_test_macros.hpp>

#include <Dataflow/Core/PortFactory.hpp>
#include <string>
using namespace Ra::Dataflow::Core;
using namespace Ra::Core;
TEST_CASE( "Dataflow/Core/PortFactory", "[unittests][Dataflow][PortFactory]" ) {

    /// singleton is init in core lib init
    REQUIRE( PortFactory::getInstance() != nullptr );
    auto factory = PortFactory::getInstance();

    SECTION( "After init default port types are available" ) {
        REQUIRE( factory->output_setter( std::type_index( typeid( Scalar ) ) ) );
        REQUIRE( factory->output_setter( std::type_index( typeid( int ) ) ) );
        REQUIRE( factory->output_setter( std::type_index( typeid( unsigned int ) ) ) );
        REQUIRE( factory->output_setter( std::type_index( typeid( Utils::Color ) ) ) );
        REQUIRE( factory->output_setter( std::type_index( typeid( Vector2 ) ) ) );
        REQUIRE( factory->output_setter( std::type_index( typeid( Vector3 ) ) ) );
        REQUIRE( factory->output_setter( std::type_index( typeid( Vector4 ) ) ) );
        REQUIRE( factory->input_getter( std::type_index( typeid( Scalar ) ) ) );
        REQUIRE( factory->input_getter( std::type_index( typeid( int ) ) ) );
        REQUIRE( factory->input_getter( std::type_index( typeid( unsigned int ) ) ) );
        REQUIRE( factory->input_getter( std::type_index( typeid( Utils::Color ) ) ) );
        REQUIRE( factory->input_getter( std::type_index( typeid( Vector2 ) ) ) );
        REQUIRE( factory->input_getter( std::type_index( typeid( Vector3 ) ) ) );
        REQUIRE( factory->input_getter( std::type_index( typeid( Vector4 ) ) ) );
    }

    SECTION( "Other types aren't available" ) {
        REQUIRE_THROWS( factory->output_setter( std::type_index( typeid( std::string ) ) ) );
        REQUIRE_THROWS( factory->output_setter( std::type_index( typeid( std::vector<int> ) ) ) );
        REQUIRE_THROWS( factory->input_getter( std::type_index( typeid( std::string ) ) ) );
        REQUIRE_THROWS( factory->input_getter( std::type_index( typeid( std::vector<int> ) ) ) );
    }

    auto node = Sources::SingleDataSourceNode<int>( "node" );

    SECTION( "Can add port with default types" ) {
        REQUIRE( factory->make_input_port( &node, "port", typeid( Scalar ) ) );
        REQUIRE( factory->make_output_port( &node, "port", typeid( Scalar ) ) );
    }
    SECTION( "Can't add port with other types" ) {
        REQUIRE( !factory->make_input_port( &node, "port0", typeid( std::string ) ) );
        REQUIRE( !factory->make_output_port( &node, "port0", typeid( std::string ) ) );
        REQUIRE( !factory->make_output_port( &node, "port1", typeid( std::vector<int> ) ) );
        REQUIRE( !factory->make_input_port( &node, "port1", typeid( std::vector<int> ) ) );
    }
    SECTION( "Add new port type and can add port with other types" ) {
        add_port_type<std::string>();
        add_port_type<std::vector<int>>();
        REQUIRE( factory->make_input_port( &node, "port0", typeid( std::string ) ) );
        REQUIRE( factory->make_output_port( &node, "port0", typeid( std::string ) ) );
        REQUIRE( factory->make_output_port( &node, "port1", typeid( std::vector<int> ) ) );
        REQUIRE( factory->make_input_port( &node, "port1", typeid( std::vector<int> ) ) );
    }
}
