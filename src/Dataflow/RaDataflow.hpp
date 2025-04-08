#pragma once
#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined RA_DATAFLOW_CORE_EXPORTS
#    define RA_DATAFLOW_CORE_API DLL_EXPORT
#elif defined RA_DATAFLOW_STATIC
#    define RA_DATAFLOW_CORE_API
#else
#    define RA_DATAFLOW_CORE_API DLL_IMPORT
#endif

#if defined RA_DATAFLOW_GUI_EXPORTS
#    define RA_DATAFLOW_GUI_API DLL_EXPORT
#elif defined RA_DATAFLOW_STATIC
#    define RA_DATAFLOW_GUI_API
#else
#    define RA_DATAFLOW_GUI_API DLL_IMPORT
#endif

#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/Log.hpp>
#include <Core/Utils/TypesUtils.hpp>

#include <nlohmann/json.hpp>

/**
 * \brief Create the node system default factory.
 *
 * The default factory of the node system contains instantiation of the nodes below for the
 * following type
 *   - Scalar, float, double, int, unsigned int
 *   - Color, VectorDf, VectorDd (where D in {2, 3, 4}
 *
 * List of instanced nodes for any TYPE above
 *   - SingleDataSourceNode<TYPE> and SingleDataSourceNode<Ra::Core::VectorArray<TYPE>>
 *   - FunctionSourceNode<TYPE, const TYPE&>, FunctionSourceNode<TYPE, const TYPE&, const TYPE&>
 *   - FunctionSourceNode<bool, const TYPE&>, FunctionSourceNode<bool, const TYPE&, const TYPE&>
 *   - SinkNode<TYPE>, SinkNode<Ra::Core::VectorArray<TYPE>
 *   - FilterNode<Ra::Core::VectorArray<TYPE>>
 *   - TransformNode<Ra::Core::VectorArray<TYPE>>, ReduceNode<Ra::Core::VectorArray<TYPE>>
 *   - BinaryOpNode<TYPE>, BinaryOpNode<Ra::Core::VectorArray<TYPE>>, BinaryOpNode<TYPE, TYPE, bool>
 *
 * All these node might be serialized/unserialized without any additional nor custom factory.
 *
 * If needed, the definition of all these type aliases can be included using one of the headers
 *  - #include <Dataflow/Core/Nodes/Functionals/CoreDataFunctionals.hpp>
 *  - #include <Dataflow/Core/Nodes/Sinks/CoreDataSinks.hpp>
 *  - #include <Dataflow/Core/Nodes/Sources/CoreDataSources.hpp>
 */
// PREFIX/SUFFIX TYPE FACTORY NAMESPACE
#define NODE_TYPES( FACTORY, NAMESPACE )                       \
    DATAFLOW_MACRO( Float, float, FACTORY, NAMESPACE );        \
    DATAFLOW_MACRO( Double, double, FACTORY, NAMESPACE );      \
    DATAFLOW_MACRO( Int, int, FACTORY, NAMESPACE );            \
    DATAFLOW_MACRO( UInt, unsigned int, FACTORY, NAMESPACE );  \
    DATAFLOW_MACRO( Color, Utils::Color, FACTORY, NAMESPACE ); \
    DATAFLOW_MACRO( Vector2f, Vector2f, FACTORY, NAMESPACE );  \
    DATAFLOW_MACRO( Vector3f, Vector3f, FACTORY, NAMESPACE );  \
    DATAFLOW_MACRO( Vector4f, Vector4f, FACTORY, NAMESPACE );  \
    DATAFLOW_MACRO( Vector2d, Vector2d, FACTORY, NAMESPACE );  \
    DATAFLOW_MACRO( Vector3d, Vector3d, FACTORY, NAMESPACE );  \
    DATAFLOW_MACRO( Vector4d, Vector4d, FACTORY, NAMESPACE );

// add json serialization
namespace nlohmann {

// usual template <typename Derived> struct adl_serializer<Eigen::MatrixBase<Derived>>
// do not find correct template substitution, see eg https://github.com/nlohmann/json/issues/3267

template <typename Scalar, int Rows, int Cols>
struct adl_serializer<Eigen::Matrix<Scalar, Rows, Cols>> {
    static void to_json( nlohmann::json& j, const Eigen::Matrix<Scalar, Rows, Cols>& matrix ) {
        for ( int row = 0; row < matrix.rows(); ++row ) {
            nlohmann::json column = nlohmann::json::array();
            for ( int col = 0; col < matrix.cols(); ++col ) {
                column.push_back( matrix( row, col ) );
            }
            j.push_back( column );
        }
    }

    static void from_json( const nlohmann::json& j, Eigen::Matrix<Scalar, Rows, Cols>& matrix ) {
        for ( std::size_t row = 0; row < j.size(); ++row ) {
            const auto& jrow = j.at( row );
            for ( std::size_t col = 0; col < jrow.size(); ++col ) {
                const auto& value = jrow.at( col );
                value.get_to( matrix( row, col ) );
            }
        }
    }
};

template <typename T>
struct adl_serializer<Ra::Core::Utils::ColorBase<T>> {
    using Color = Ra::Core::Utils::ColorBase<T>;
    static void to_json( json& j, const Color& value ) {
        j.push_back( value( 0 ) );
        j.push_back( value( 1 ) );
        j.push_back( value( 2 ) );
        j.push_back( value( 3 ) );
    }
    static void from_json( const json& j, Color& value ) {
        j.at( 0 ).get_to( value( 0 ) );
        j.at( 1 ).get_to( value( 1 ) );
        j.at( 2 ).get_to( value( 2 ) );
        j.at( 3 ).get_to( value( 3 ) );
    }
};

} // namespace nlohmann

/*******************************************************************************
 * Node creation
 */

/**
 * @brief Use the following macro to define a new parameter in a node.
 *
 * This macro defines const and non-const accessors to the parameter called
 * `param_##name()`.
 */
#define RA_NODE_PARAMETER( T, name, ... )    \
  public:                                    \
    T& param_##name() noexcept {             \
        return m_param_##name->second;       \
    }                                        \
    const T& param_##name() const noexcept { \
        return m_param_##name->second;       \
    }                                        \
                                             \
  private:                                   \
    ParamHandle<T> m_param_##name { addParameter<T>( std::string( name ), __VA_ARGS__ ) };

#define RA_NODE_PORT_IN_ACCESSOR( T, name ) \
  public:                                   \
    PortInPtr<T> port_in_##name() {         \
        return m_port_in_##name;            \
    }

/**                                                                      \
 * @brief Use the following macro to define a new port_in in a node.     \
 *                                                                       \
 * This macro defines an accessor to the port called `port_in_##name()`. \
 */
#define RA_NODE_PORT_IN( T, name )              \
    RA_NODE_PORT_IN_ACCESSOR( T, name )         \
                                                \
  private:                                      \
    PortInPtr<T> m_port_in_##name {             \
        addInputPort<T>( std::string( #name ) ) \
    }

/**                                                                      \
 * @brief Use the following macro to define a new port_in in a node.     \
 *                                                                       \
 * This macro defines an accessor to the port called `port_in_##name()`. \
 */
#define RA_NODE_PORT_IN_WITH_DEFAULT( T, name, default_value ) \
    RA_NODE_PORT_IN_ACCESSOR( T, name )                        \
                                                               \
  private:                                                     \
    PortInPtr<T> m_port_in_##name {                            \
        addInputPort<T>( std::string( #name ), default_value ) \
    }

#define RA_NODE_PORT_OUT_ACCESSOR( T, name ) \
  public:                                    \
    PortOutPtr<T> port_out_##name() {        \
        return m_port_out_##name;            \
    }

/**
 * @brief Use the following macro to define a new port_out in a node.
 *
 * This macro defines an accessor to the port called `port_out_##name()`.
 */
#define RA_NODE_PORT_OUT( T, name )              \
    RA_NODE_PORT_OUT_ACCESSOR( T, name )         \
  private:                                       \
    PortOutPtr<T> m_port_out_##name {            \
        addOutputPort<T>( std::string( #name ) ) \
    }

/**
 * @brief Use the following macro to define a new port_out in a node.
 *
 * This macro defines an accessor to the port called `port_out_##name()`.
 */
#define RA_NODE_PORT_OUT_WITH_DATA( T, name )               \
    RA_NODE_PORT_OUT_ACCESSOR( T, name )                    \
  private:                                                  \
    T m_##name;                                             \
    PortOutPtr<T> m_port_out_##name {                       \
        addOutputPort<T>( &m_##name, std::string( #name ) ) \
    }
