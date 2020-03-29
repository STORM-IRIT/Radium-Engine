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

} // namespace Engine
} // namespace Ra
