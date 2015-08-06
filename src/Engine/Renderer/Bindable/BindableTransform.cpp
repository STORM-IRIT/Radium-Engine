#include <Engine/Renderer/Bindable/BindableTransform.hpp>

#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
namespace Ra
{

    Engine::BindableTransform::BindableTransform ( const Core::Matrix4& model,
                                                   const Core::Matrix4& view,
                                                   const Core::Matrix4& proj )
        : m_model ( model )
        , m_view ( view )
        , m_proj ( proj )
    {
        m_mvp = proj * view * model;
        m_modelView = view * model;
        m_worldNormal = model.inverse().transpose();
        m_viewNormal = m_modelView.inverse().transpose();
    }

    Engine::BindableTransform::~BindableTransform()
    {
    }

    void Engine::BindableTransform::bind ( ShaderProgram* shader ) const
    {
        // FIXME(Charly): Use UBO here ?
        shader->setUniform ( "transform.model", m_model );
        shader->setUniform ( "transform.view", m_view );
        shader->setUniform ( "transform.proj", m_proj );
        shader->setUniform ( "transform.modelView", m_modelView );
        shader->setUniform ( "transform.mvp", m_mvp );
        shader->setUniform ( "transform.worldNormal", m_worldNormal );
        shader->setUniform ( "transform.viewNormal", m_viewNormal );
    }

    bool Engine::BindableTransform::operator< ( const BindableTransform& other ) const
    {
        for ( uint i = 0; i < 4; ++i )
        {
            for ( uint j = 0; j < 4; ++j )
            {
                if ( m_modelView ( i, j ) < other.m_modelView ( i, j ) )
                {
                    return true;
                }

                if ( m_modelView ( i, j ) > other.m_modelView ( i, j ) )
                {
                    return false;
                }
            }
        }
        return false;
    }

}
