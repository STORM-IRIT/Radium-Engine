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
