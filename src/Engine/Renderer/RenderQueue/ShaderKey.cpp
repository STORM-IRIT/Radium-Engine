#include <Engine/Renderer/RenderQueue/ShaderKey.hpp>

#include <Core/CoreMacros.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Core/Log/Log.hpp>

namespace Ra
{
    namespace Engine
    {
        ShaderKey::ShaderKey( ShaderProgram *shader )
                : m_shader( shader )
        {
        }

        ShaderKey::~ShaderKey()
        {
        }

        void ShaderKey::bind() const
        {
            m_shader->bind();
        }

        void ShaderKey::bind( const RenderParameters &params ) const
        {
            m_shader->bind();
            params.bind( m_shader );
        }

        bool ShaderKey::operator<( const ShaderKey &other ) const
        {
            return m_shader->getId() < other.m_shader->getId();
        }

    } // namespace Engine

} // namespace Ra
