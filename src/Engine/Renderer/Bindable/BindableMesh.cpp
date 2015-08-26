#include <Engine/Renderer/Bindable/BindableMesh.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

namespace Ra
{

    Engine::BindableMesh::BindableMesh( Mesh* mesh, uint id )
        : m_mesh( mesh )
        , m_id( id )
    {
        Scalar r = Scalar( ( id & 0x000000FF ) >> 0 ) / 255.0;
        Scalar g = Scalar( ( id & 0x0000FF00 ) >> 8 ) / 255.0;
        Scalar b = Scalar( ( id & 0x00FF0000 ) >> 16 ) / 255.0;

        m_idAsColor = Core::Vector4( r, g, b, 1.0 );
    }

    Engine::BindableMesh::~BindableMesh()
    {
    }

    void Engine::BindableMesh::bind( ShaderProgram* shader ) const
    {
        m_renderParameters.bind( shader );
        shader->setUniform( "objectId", m_idAsColor );
    }

    void Engine::BindableMesh::render() const
    {
        m_mesh->render();
    }

    bool Engine::BindableMesh::operator< ( const BindableMesh& other ) const
    {
        return m_id < other.m_id;
    }

} // namespace Ra
