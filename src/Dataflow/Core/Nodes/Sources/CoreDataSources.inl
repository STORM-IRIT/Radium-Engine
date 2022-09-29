#pragma once
#include <Dataflow/Core/Nodes/Sources/CoreDataSources.hpp>

#include <Core/Utils/TypesUtils.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sources {

/* Source bool */
inline BooleanValueSource::BooleanValueSource( const std::string& name ) :
    SingleDataSourceNode( name, BooleanValueSource::getTypename() ) {
    setEditable( "boolean" );
}

inline void BooleanValueSource::toJsonInternal( nlohmann::json& data ) const {
    data["boolean"] = *getData();
}

inline void BooleanValueSource::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "boolean" ) ) {
        bool v = data["boolean"];
        setData( v );
    }
}

inline const std::string BooleanValueSource::getTypename() {
    return "Source::bool";
}

/* Source Scalar */

inline const std::string ScalarValueSource::getTypename() {
    return "Source::Scalar";
}

inline ScalarValueSource::ScalarValueSource( const std::string& name ) :
    SingleDataSourceNode( name, ScalarValueSource::getTypename() ) {
    setEditable( "number" );
}

inline void ScalarValueSource::toJsonInternal( nlohmann::json& data ) const {
    data["number"] = *getData();
}

inline void ScalarValueSource::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "number" ) ) {
        Scalar v = data["number"];
        setData( v );
    }
}

/* Source Color */
inline const std::string ColorSourceNode::getTypename() {
    return "Source::Color";
}
inline ColorSourceNode::ColorSourceNode( const std::string& name ) :
    SingleDataSourceNode( name, ColorSourceNode::getTypename() ) {
    setEditable( "color" );
}

inline void ColorSourceNode::toJsonInternal( nlohmann::json& data ) const {
    auto c = Ra::Core::Utils::Color::linearRGBTosRGB( *getData() );
    std::array<Scalar, 3> color { { c.x(), c.y(), c.z() } };
    data["color"] = color;
}

inline void ColorSourceNode::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "color" ) ) {
        std::array<Scalar, 3> c = data["color"];
        auto v =
            Ra::Core::Utils::Color::sRGBToLinearRGB( Ra::Core::Utils::Color( c[0], c[1], c[2] ) );
        setData( v );
    }
}

template <typename T>
const std::string VectorArrayDataSource<T>::getTypename() {
    return std::string { "Source:VectorArray<" } + Ra::Core::Utils::demangleType<T>() + ">";
}

template <typename T>
void VectorArrayDataSource<T>::toJsonInternal( nlohmann::json& data ) const {
    data["comment"] = std::string { "Serialization of data from a " } +
                      VectorArrayDataSource<T>::getTypename() + " is not supported.";
}

template <typename T>
void VectorArrayDataSource<T>::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "data" ) ) {
        std::cerr << "Warning : deserialization of Ra::Core::VectorArray<T> not yet implemented\n";
    }
}

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra
