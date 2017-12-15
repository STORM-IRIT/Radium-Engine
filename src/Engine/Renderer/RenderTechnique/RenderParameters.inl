#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

namespace Ra
{
    namespace Engine
    {

        template <typename T>
        inline void RenderParameters::UniformBindableVector<T>::bind(const ShaderProgram* shader ) const
        {
            for ( auto& value : *this )
            {
                value.second.bind( shader );
            }
        }

        template <typename T>
        inline void RenderParameters::TParameter<T>::bind(const ShaderProgram* shader ) const
        {
            shader->setUniform( m_name, m_value );
        }

        inline void RenderParameters::TextureParameter::bind(const ShaderProgram* shader ) const
        {
            shader->setUniform( m_name, m_texture, m_texUnit );
        }

    } // namespace Engine
} // namespace Ra
