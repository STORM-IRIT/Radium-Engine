#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

namespace Ra
{
    namespace Engine
    {
        RenderObject::RenderObject( const std::string& name, const Component* comp,
                                    const RenderObjectType& type )
            : IndexedObject()
            , m_localTransform( Core::Transform::Identity() )
            , m_component( comp )
            , m_name( name )
            , m_type( type )
            , m_renderTechnique( nullptr )
            , m_mesh( nullptr )
            , m_visible( true )
            , m_isDirty( true )
        {
        }

        RenderObject::~RenderObject()
        {
        }

        void RenderObject::updateGL()
        {
            // Do not update while we are cloning
            std::lock_guard<std::mutex> lock( m_updateMutex );

            if ( m_renderTechnique )
            {
                m_renderTechnique->updateGL();
            }

            if ( m_mesh )
            {
                m_mesh->updateGL();
            }

            m_isDirty = false;
        }
    } // namespace Engine
} // namespace Ra

