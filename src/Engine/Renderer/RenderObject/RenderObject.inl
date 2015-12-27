#include <Engine/Renderer/RenderObject/RenderObject.hpp>

namespace Ra
{
    namespace Engine
    {
        inline const RenderObjectType& RenderObject::getType() const
        {
            return m_type;
        }

        inline const std::string& RenderObject::getName() const
        {
            return m_name;
        }

        inline void RenderObject::setVisible( bool visible )
        {
            m_visible = visible;
        }

        inline bool RenderObject::isVisible() const
        {
            return m_visible;
        }

        inline bool RenderObject::isDirty() const
        {
            return m_isDirty;
        }

        inline const Component* RenderObject::getComponent() const
        {
            return m_component;
        }

        inline void RenderObject::setRenderTechnique( RenderTechnique* technique )
        {
            CORE_ASSERT( technique, "Passing a nullptr as render technique" );
            m_renderTechnique = technique;
        }

        inline const RenderTechnique* RenderObject::getRenderTechnique() const
        {
            return m_renderTechnique;
        }

        inline RenderTechnique* RenderObject::getRenderTechnique()
        {
            return m_renderTechnique;
        }

        inline void RenderObject::setMesh( const std::shared_ptr<Mesh>& mesh )
        {
            m_mesh = mesh;
        }

        inline const Mesh* RenderObject::getMesh() const
        {
            return m_mesh.get();
        }

        inline Mesh* RenderObject::getMesh()
        {
            return m_mesh.get();
        }

        inline void RenderObject::setLocalTransform( const Core::Transform& transform )
        {
            m_localTransform = transform;
        }

        inline void RenderObject::setLocalTransform( const Core::Matrix4& transform )
        {
            m_localTransform = Core::Transform( transform );
        }

        inline const Core::Transform& RenderObject::getLocalTransform() const
        {
            return m_localTransform;
        }

        inline const Core::Matrix4& RenderObject::getLocalTransformAsMatrix() const
        {
            return m_localTransform.matrix();
        }

        inline void RenderObject::addRenderParameters( const RenderParameters &parameters )
        {
            m_renderParameters.concatParameters( parameters );
        }
    }
} // namespace Ra
