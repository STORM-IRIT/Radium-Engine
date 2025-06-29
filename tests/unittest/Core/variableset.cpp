#include <Core/Containers/DynamicVisitor.hpp>
#include <Core/Containers/VariableSet.hpp>

#include <Core/Utils/TypesUtils.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <string>

#include "../unittestUtils.hpp"

using namespace Ra::Core;

struct PrintThemAll
    : public PrintAllHelper<Ra::Core::Utils::TypeList<int, size_t, float, double, std::string>> {};

class MyParameterVisitor : public DynamicVisitor
{
  public:
    MyParameterVisitor() : DynamicVisitor() {
        addOperator<int>( *this );
        addOperator<float>( *this );
        addOperator<std::string>( *this );
    }

    template <typename T>
    void operator()( const std::string& name, T& _in, std::any&& ) {
        std::cout << "\t(MyParameterVisitor : ( " << Utils::simplifiedDemangledType<T>() << " ) "
                  << name << " --> " << _in << " // ";
        _in /= 2;
        std::cout << _in << "\n";
        ++m_counter;
    }

    void operator()( const std::string& name, std::string& _in, std::any&& ) {
        std::cout << "\t(MyParameterVisitor : ( std::string ) " << name << " --> " << _in << "\n";
        ++m_counter;
    }

    size_t getCount() { return m_counter; }
    void resetCount() { m_counter = 0; }

  private:
    size_t m_counter { 0 };
};

// Apply a functor to mofify each integer of a VariableSet
struct modifyInts : public VariableSet::StaticVisitor<int> {
    template <typename F>
    void operator()( const std::string&, int& value, F&& f ) {
        value = f( value );
    }
};
auto print_container = []( const std::string& name, VariableSet& ps ) {
    std::cout << name << " content : ";
    ps.visit( PrintThemAll {} );
    std::cout << std::endl;
};

TEST_CASE( "Core/Container/VariableSet", "[unittests][Core][Container][VariableSet]" ) {
    REQUIRE( PrintThemAll::types::Size == 5 );

    VariableSet params;
    REQUIRE( !params.existsVariableType<int>() );
    int i { 1 };
    int initial_i = i;
    float x { 1.f };

    std::cout << "Adding parameters" << std::endl;
    params.insertVariable( "i", i );             // i added by value
    params.insertVariable( "j", std::ref( i ) ); // j is a reference on i
    params.insertVariable( "x", 2 );
    params.insertVariable( "x", x );
    params.insertVariable( "foo", std::string { "bar" } );
    std::cout << " ... done!" << std::endl;
    print_container( "Initial set", params );

    SECTION( "Construction, access and removal to and from a variable set" ) {

        REQUIRE( params.existsVariable<int>( "i" ) );
        REQUIRE( params.existsVariable<std::reference_wrapper<int>>( "j" ) );
        REQUIRE( params.existsVariable<int>( "x" ) );
        REQUIRE( params.existsVariable<float>( "x" ) );
        REQUIRE( params.existsVariable<std::string>( "foo" ) );

        auto added = params.insertVariable( "x", x );
        REQUIRE( added.second == false );
        REQUIRE( added.first->second == x );

        // Verify handle validity
        auto fooHandle = params.getVariableHandle<std::string>( "foo" );
        REQUIRE( params.isHandleValid( fooHandle ) == true );
        REQUIRE( fooHandle->first == "foo" );
        REQUIRE( fooHandle->second == "bar" );

        auto dummyHandle = params.getVariableHandle<int>( "z" );
        REQUIRE( params.isHandleValid( dummyHandle ) == false );

        REQUIRE( params.size() == 5 );
        REQUIRE( params.numberOf<int>() == 2 );
        REQUIRE( params.numberOf<std::reference_wrapper<int>>() == 1 );
        REQUIRE( params.numberOf<float>() == 1 );
        REQUIRE( params.numberOf<std::string>() == 1 );

        REQUIRE( params.getVariable<float>( "x" ) == x );
        // modify "x" through its handle
        auto xHandle = params.getVariableHandle<float>( "x" );
        REQUIRE( xHandle->second == x );
        xHandle->second = 5;
        REQUIRE( params.getVariable<float>( "x" ) != x );
        REQUIRE( params.getVariable<float>( "x" ) == 5 );

        // variable i store a value, copy of local variable i. Changing its value ...
        auto inserted = params.setVariable( "i", 2 );
        REQUIRE( inserted.second == false );
        REQUIRE( params.getVariable<int>( "i" ) == 2 );
        // does not change the local variable i
        REQUIRE( i == 1 );

        inserted = params.setVariable( "k", 3 );
        REQUIRE( inserted.second == true );

        // variable "j" is a reference to local variable i, and has the same value
        REQUIRE( params.getVariable<std::reference_wrapper<int>>( "j" ) == i );
        // Changing local variable i ....
        i = 3;
        // ... changes the content of its reference "j"
        REQUIRE( params.getVariable<std::reference_wrapper<int>>( "j" ) == i );

        params.deleteVariable<float>( "x" );
        REQUIRE( !params.existsVariable<float>( "x" ).has_value() );
        // as x (float) variable was removed, xHandle is now invalid
        REQUIRE( params.isHandleValid( xHandle ) == false );
        REQUIRE( params.existsVariable<int>( "x" ).has_value() );
        params.deleteVariable<int>( "x" );
        REQUIRE( !params.existsVariable<int>( "x" ).has_value() );
    }

    SECTION( "Visiting and modifying variable set using static visitor" ) {
        auto modifyFunction = []( int x_ ) { return 2 * x_ + 1; };

        REQUIRE( params.getVariable<int>( "i" ) == 1 );
        REQUIRE( params.getVariable<int>( "x" ) == 2 );
        REQUIRE( params.getVariable<float>( "x" ) == 1 );
        // modifies params i, and params j as a ref, so local var i is also modified
        params.visit( modifyInts {}, modifyFunction );
        REQUIRE( params.getVariable<int>( "i" ) == modifyFunction( initial_i ) );
        REQUIRE( i == modifyFunction( initial_i ) );
        REQUIRE( params.getVariable<std::reference_wrapper<int>>( "j" ) ==
                 modifyFunction( initial_i ) );
        REQUIRE( params.getVariable<int>( "x" ) == modifyFunction( 2 ) );
        REQUIRE( params.getVariable<float>( "x" ) == 1 );
    }

    SECTION( "Visiting and modifying variable set using dynamic visitor" ) {
        REQUIRE( params.getVariable<int>( "i" ) == 1 );
        REQUIRE( params.getVariable<int>( "x" ) == 2 );
        REQUIRE( params.getVariable<float>( "x" ) == 1 );

        DynamicVisitor vf;
        // Adding a visitor operator on ints
        vf.addOperator<int>( []( const std::string& name, auto& value, std::any&& ) {
            std::cout << "\tDoubling the int " << name << " (equal to " << value << ")\n";
            value *= 2;
        } );
        params.visit( vf );
        print_container( "Doubled set", params );
        REQUIRE( params.getVariable<int>( "i" ) == ( 2 * initial_i ) );
        REQUIRE( params.getVariable<std::reference_wrapper<int>>( "j" ) == ( 2 * initial_i ) );
        REQUIRE( params.getVariable<std::reference_wrapper<int>>( "j" ) == i );
        REQUIRE( params.getVariable<int>( "x" ) == ( 2 * 2 ) );
        REQUIRE( params.getVariable<float>( "x" ) == 1 );

        // Changing the visitor operator on ints
        vf.addOrReplaceOperator<int>( []( const std::string& name, auto& value, std::any&& ) {
            std::cout << "\tHalving the int " << name << " (equal to " << value << ")\n";
            value /= 2;
        } );
        params.visit( vf );
        REQUIRE( params.getVariable<int>( "i" ) == ( i ) );
        REQUIRE( params.getVariable<int>( "x" ) == ( 2 ) );
        REQUIRE( params.getVariable<float>( "x" ) == 1 );
        // removing the visitor operator on ints
        vf.removeOperator<int>();
        params.visit( vf );
        REQUIRE( params.getVariable<int>( "i" ) == ( i ) );
        REQUIRE( params.getVariable<int>( "x" ) == ( 2 ) );
        REQUIRE( params.getVariable<float>( "x" ) == 1 );
    }

    SECTION( "Visiting and modifying variable set using standard range for" ) {
        params.insertVariable( "y", std::sqrt( 3 * x ) );

        REQUIRE( params.getVariable<float>( "x" ) == x );
        REQUIRE( params.getVariable<float>( "y" ) == std::sqrt( 3 * x ) );
        auto& floatParams = params.getAllVariables<float>();
        // read-write loop on float params
        for ( auto& p : floatParams ) {
            std::cout << p.first << " = " << p.second;
            p.second = p.second * 2;
            std::cout << " ==> " << p.second << "\n";
        }
        REQUIRE( params.getVariable<float>( "x" ) == 2 * x );
        REQUIRE( params.getVariable<float>( "y" ) == 2 * std::sqrt( 3 * x ) );

        // read-only loop on int params
        for ( const auto& p : params.getAllVariables<int>() ) {
            std::cout << p.first << " = " << p.second;
            // p.second = p.second * 2; // this does not compile ^^
            std::cout << " ==> " << p.second << "\n";
        }
        print_container( "Final set", params );

        auto handle = params.getVariableHandle<int>( "i" );
        REQUIRE( params.isHandleValid( handle ) );
        params.deleteVariable( handle );
        REQUIRE( params.isHandleValid( handle ) == false );
        auto& intVariables = params.getAllVariablesFromHandle( handle );
        std::cout << "Looping on all variables with the same type of a given handle\n";
        for ( const auto& p : intVariables ) {
            std::cout << p.first << " = " << p.second << " ";
        }
        std::cout << "\n";
    }
    SECTION( "General visit using a custom visitor" ) {
        std::cout << "General visit using a custom visitor" << std::endl;

        MyParameterVisitor mp;
        REQUIRE( mp.getCount() == 0 );
        params.visit( mp );
        REQUIRE( mp.getCount() == 5 );
        REQUIRE( i == 0 );

        auto xHandle = params.getVariableHandle<float>( "x" );
        REQUIRE( params.isHandleValid( xHandle ) == true );
        auto deletedFloats = params.deleteAllVariables<float>();
        REQUIRE( deletedFloats );
        REQUIRE( !params.existsVariableType<float>() );
        deletedFloats = params.deleteAllVariables<float>();
        REQUIRE( !deletedFloats );
        // as all floats was removed, xHandle is now invalid
        REQUIRE( params.isHandleValid( xHandle ) == false );

        mp.resetCount();
        params.visit( mp );
        REQUIRE( mp.getCount() == 4 );
        print_container( "Final set", params );
    }

    print_container( "Final set", params );
}
TEST_CASE( "Core/Container/VariableSet/Merging, copying, moving",
           "[Core][Container][VariableSet]" ) {

    VariableSet params;

    REQUIRE( !params.existsVariableType<int>() );
    params.insertVariable( "a", 1 );
    params.insertVariable( "b", 2 );
    print_container( "initial params ", params );

    VariableSet params2;
    params2.insertVariable( "b", 4 );
    params2.insertVariable( "c", 5 );
    params2.insertVariable( "e", 2.7182818285 );
    print_container( "initial params2 ", params2 );

    VariableSet params3;
    params3.insertVariable( "a", 0 );
    params3.insertVariable( "c", 4 );
    params3.insertVariable( "pi", 3.151592f );
    print_container( "initial params3 ", params3 );

    print_container( "initial params ", params );

    REQUIRE( params.getVariable<int>( "b" ) == 2 );
    REQUIRE( params2.getVariable<int>( "b" ) == 4 );
    params.mergeReplaceVariables( params2 );
    REQUIRE( params.getVariable<int>( "b" ) == 4 );
    print_container( "params after merge of params2 (replace)", params );

    REQUIRE( params.getVariable<int>( "c" ) == 5 );
    REQUIRE( params3.getVariable<int>( "c" ) == 4 );
    params.mergeKeepVariables( params3 );
    REQUIRE( params.getVariable<int>( "c" ) == 5 );
    print_container( "params after merge of params3 (keep)", params );

    // copy constructor
    auto newparams = new VariableSet( params );
    REQUIRE( newparams->size() == params.size() );

    auto numInt  = params.numberOf<int>();
    auto numIntN = newparams->numberOf<int>();
    REQUIRE( numInt == numIntN );

    auto numFloat  = params.numberOf<float>();
    auto numFloatN = newparams->numberOf<float>();
    REQUIRE( numFloat == numFloatN );

    auto numDouble  = params.numberOf<double>();
    auto numDoubleN = newparams->numberOf<double>();
    REQUIRE( numDouble == numDoubleN );

    REQUIRE( ( numIntN + numFloatN + numDoubleN ) == newparams->size() );

    auto numString = newparams->numberOf<std::string>();
    REQUIRE( numString == 0 );
    auto removed = newparams->deleteAllVariables<std::string>();
    REQUIRE( removed == false );

    print_container( "Copied params into newparams", *newparams );
    delete newparams;
    auto sp = params.size();
    auto paramsMoved { std::move( params ) };
    REQUIRE( params.size() == 0 );
    REQUIRE( paramsMoved.size() == sp );
    print_container( "Moved params into paramsMoved", paramsMoved );
    print_container( "params is empty", params );

    REQUIRE( !params.existsVariableType<int>() );
}

TEST_CASE( "Core/Container/VariableSe/Iterating on stored types",
           "[unittests][Core][Container][VariableSet]" ) {
    VariableSet vs;
    vs.insertVariable( "x", 1.414_ra );
    vs.insertVariable( "y", std::sqrt( 2 ) );
    std::function<Scalar( Scalar )> multBy2 = []( Scalar x ) { return x * 2_ra; };
    vs.insertVariable( "f", multBy2 );
    auto typeVector = vs.getStoredTypes();
    std::cout << "Stored types : \n";
    for ( const auto& t : typeVector ) {
        std::cout << "\t" << Ra::Core::Utils::simplifiedDemangledType( t ) << "\n";
    }

    REQUIRE( std::find( typeVector.begin(),
                        typeVector.end(),
                        std::type_index( typeid( Scalar ) ) ) != typeVector.end() );
    REQUIRE( std::find( typeVector.begin(),
                        typeVector.end(),
                        std::type_index( typeid( std::function<Scalar( Scalar )> ) ) ) !=
             typeVector.end() );

    auto b = vs.deleteVariable<Scalar>( "x" );
    REQUIRE( b );
    b = vs.deleteVariable<float>( "y" );
    REQUIRE( !b );
    b = vs.deleteVariable<double>( "y" );
    REQUIRE( b );
}

TEST_CASE( "Core/Container/VariableSet/Clear", "[unittests][Core][Container][VariableSet]" ) {
    VariableSet params;
    REQUIRE( !params.existsVariableType<int>() );
    REQUIRE( !params.existsVariableType<std::string>() );

    params.insertVariable( "a", 1 );
    params.insertVariable( "b", 2 );
    params.insertVariable( "s1", std::string { "String 1" } );
    REQUIRE( params.existsVariableType<int>() );

    REQUIRE( params.existsVariableType<std::string>() );
    REQUIRE( params.size() == 3 );
    params.clear();
    REQUIRE( params.size() == 0 );
    REQUIRE( !params.existsVariableType<int>() );
    REQUIRE( !params.existsVariableType<std::string>() );
}
