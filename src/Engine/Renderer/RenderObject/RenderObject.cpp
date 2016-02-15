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
            , m_xray( false )
            , m_dirty( true )
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

            m_dirty = false;
        }

        const RenderObjectType& RenderObject::getType() const
        {
            return m_type;
        }

        void RenderObject::setType(const RenderObjectType &t)
        {
            // Fixme (val) : this will have no effect now
            m_type=t;
        }

        const std::string& RenderObject::getName() const
        {
            return m_name;
        }

        void RenderObject::setVisible( bool visible )
        {
            m_visible = visible;
        }

        void RenderObject::toggleVisible()
        {
            m_visible = !m_visible;
        }

        bool RenderObject::isVisible() const
        {
            return m_visible;
        }

        void RenderObject::setXRay( bool xray )
        {
            m_xray = xray;
        }

        void RenderObject::toggleXRay()
        {
            m_xray = !m_xray;
        }

        bool RenderObject::isXRay() const
        {
            return m_xray;
        }

        bool RenderObject::isDirty() const
        {
            return m_dirty;
        }

        const Component* RenderObject::getComponent() const
        {
            return m_component;
        }

        void RenderObject::setRenderTechnique( RenderTechnique* technique )
        {
            CORE_ASSERT( technique, "Passing a nullptr as render technique" );
            m_renderTechnique = technique;
        }

        const RenderTechnique* RenderObject::getRenderTechnique() const
        {
            return m_renderTechnique;
        }

        RenderTechnique* RenderObject::getRenderTechnique()
        {
            return m_renderTechnique;
        }

        void RenderObject::setMesh( const std::shared_ptr<Mesh>& mesh )
        {
            m_mesh = mesh;
        }

        const Mesh* RenderObject::getMesh() const
        {
            return m_mesh.get();
        }

        Mesh* RenderObject::getMesh()
        {
            return m_mesh.get();
        }

        Core::Transform RenderObject::getTransform() const
        {
            return m_component->getEntity()->getTransform() * m_localTransform;
        }

        Core::Matrix4 RenderObject::getTransformAsMatrix() const
        {
            return getTransform().matrix();
        }

        void RenderObject::setLocalTransform( const Core::Transform& transform )
        {
            m_localTransform = transform;
        }

        void RenderObject::setLocalTransform( const Core::Matrix4& transform )
        {
            m_localTransform = Core::Transform( transform );
        }

        const Core::Transform& RenderObject::getLocalTransform() const
        {
            return m_localTransform;
        }

        const Core::Matrix4& RenderObject::getLocalTransformAsMatrix() const
        {
            return m_localTransform.matrix();
        }
    } // namespace Engine
} // namespace Ra

