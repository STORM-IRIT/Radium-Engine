#pragma once
#include <Engine/Data/RenderParameters.hpp>
#include <Engine/Data/ShaderProgram.hpp>

namespace Ra {
namespace Engine {
namespace Data {

template <typename T>
inline void
RenderParameters::UniformBindableSet<T>::bind( const Data::ShaderProgram* shader ) const {
    for ( const auto& value : *this ) {
        value.second.bind( shader );
    }
}

template <typename T>
inline void RenderParameters::TParameter<T>::bind( const Data::ShaderProgram* shader ) const {
    shader->setUniform( m_name.c_str(), m_value );
}

template <>
inline void RenderParameters::TParameter<Ra::Core::Utils::Color>::bind(
    const Data::ShaderProgram* shader ) const {
    shader->setUniform( m_name.c_str(), Ra::Core::Utils::Color::VectorType( m_value ) );
}

inline void RenderParameters::TextureParameter::bind( const Data::ShaderProgram* shader ) const {
    if ( m_texUnit == -1 ) { shader->setUniformTexture( m_name.c_str(), m_texture ); }
    else { shader->setUniform( m_name.c_str(), m_texture, m_texUnit ); }
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::BoolParameter>&
RenderParameters::getParameterSet() const {
    return m_boolParamsVector;
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::IntParameter>&
RenderParameters::getParameterSet() const {
    return m_intParamsVector;
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::UIntParameter>&
RenderParameters::getParameterSet() const {
    return m_uintParamsVector;
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::ScalarParameter>&
RenderParameters::getParameterSet() const {
    return m_scalarParamsVector;
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::IntsParameter>&
RenderParameters::getParameterSet() const {
    return m_intsParamsVector;
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::UIntsParameter>&
RenderParameters::getParameterSet() const {
    return m_uintsParamsVector;
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::ScalarsParameter>&
RenderParameters::getParameterSet() const {
    return m_scalarsParamsVector;
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::Vec2Parameter>&
RenderParameters::getParameterSet() const {
    return m_vec2ParamsVector;
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::Vec3Parameter>&
RenderParameters::getParameterSet() const {
    return m_vec3ParamsVector;
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::Vec4Parameter>&
RenderParameters::getParameterSet() const {
    return m_vec4ParamsVector;
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::ColorParameter>&
RenderParameters::getParameterSet() const {
    return m_colorParamsVector;
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::Mat2Parameter>&
RenderParameters::getParameterSet() const {
    return m_mat2ParamsVector;
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::Mat3Parameter>&
RenderParameters::getParameterSet() const {
    return m_mat3ParamsVector;
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::Mat4Parameter>&
RenderParameters::getParameterSet() const {
    return m_mat4ParamsVector;
}

template <>
inline const RenderParameters::UniformBindableSet<RenderParameters::TextureParameter>&
RenderParameters::getParameterSet() const {
    return m_texParamsVector;
}

template <typename T>
inline bool RenderParameters::containsParameter( const std::string& name ) const {
    auto& params = getParameterSet<T>();
    return params.find( name ) != params.end();
}

template <typename T>
inline const T& RenderParameters::getParameter( const std::string& name ) const {
    auto& params = getParameterSet<T>();
    return params.at( name );
}

template <typename Enum>
inline RenderParameters::EnumConverter<Enum>::EnumConverter(
    std::initializer_list<std::pair<typename std::underlying_type_t<Enum>, std::string>> pairs ) :
    AbstractEnumConverter(), m_valueToString { pairs } {}

template <typename Enum>
inline void
RenderParameters::EnumConverter<Enum>::setEnumValue( RenderParameters& p,
                                                     const std::string& name,
                                                     const std::string& enumerator ) const {
    p.addParameter( name, m_valueToString.key( enumerator ) );
}

template <typename Enum>
inline std::string RenderParameters::EnumConverter<Enum>::getEnumerator( int v ) const {
    return m_valueToString( std::underlying_type_t<Enum>( v ) );
}

template <typename Enum>
inline int RenderParameters::EnumConverter<Enum>::getEnumerator( const std::string& v ) const {
    return m_valueToString.key( v );
}

template <typename Enum>
std::vector<std::string> RenderParameters::EnumConverter<Enum>::getEnumerators() const {
    std::vector<std::string> keys;
    keys.reserve( m_valueToString.size() );
    for ( const auto& p : m_valueToString ) {
        keys.push_back( p.second );
    }
    return keys;
}

} // namespace Data
} // namespace Engine
} // namespace Ra
