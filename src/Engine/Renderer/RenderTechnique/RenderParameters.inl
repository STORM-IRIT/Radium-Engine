#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

namespace Ra {
namespace Engine {

template <typename T>
inline void RenderParameters::UniformBindableSet<T>::bind( const ShaderProgram* shader ) const {
    for ( const auto& value : *this )
    {
        value.second.bind( shader );
    }
}

template <typename T>
inline void RenderParameters::TParameter<T>::bind( const ShaderProgram* shader ) const {
    shader->setUniform( m_name, m_value );
}

inline void RenderParameters::TextureParameter::bind( const ShaderProgram* shader ) const {
    if ( m_texUnit == -1 ) { shader->setUniformTexture( m_name, m_texture ); }
    else
    { shader->setUniform( m_name, m_texture, m_texUnit ); }
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

} // namespace Engine
} // namespace Ra
