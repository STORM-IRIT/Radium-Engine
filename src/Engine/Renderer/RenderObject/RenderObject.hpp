#ifndef RADIUMENGINE_RENDEROBJECT_HPP
#define RADIUMENGINE_RENDEROBJECT_HPP

#include <string>
#include <mutex>
#include <memory>

#include <Core/CoreMacros.hpp>
#include <Core/Index/IndexedObject.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>

namespace Ra
{
    namespace Engine
    {
        class Light;
        struct RenderTechnique;
        class Component;
        class Mesh;
        class RenderQueue;
    }
}

namespace Ra
{
    namespace Engine
    {

        // FIXME(Charly): Does this need a bit of cleanup ?
        class RA_API RenderObject : public Core::IndexedObject
        {
        public:
            enum class RenderObjectType
            {
                RO_OPAQUE,
                RO_TRANSPARENT,
                RO_DEBUG,
                RO_UI
            };

        public:
            // FIXME(Charly): Set component in the constructor ?
            explicit RenderObject( const std::string& name );
            ~RenderObject();

            void setRenderObjectType( const RenderObjectType& type );
            const RenderObjectType& getRenderObjectType() const;

            void setComponent( Component* component );

            const std::string& getName() const;

            void updateGL();
            void feedRenderQueue( RenderQueue& queue,
                                  const Core::Matrix4& view,
                                  const Core::Matrix4& proj );

            RenderObject* clone();

            void setVisible( bool visible );
            bool isVisible() const;

            bool isDirty() const;

            void setRenderTechnique( RenderTechnique* technique );
            RenderTechnique* getRenderTechnique() const;

            void setMesh( Mesh* mesh );
            Mesh* getMesh() const;

            void setLocalTransform( const Core::Transform& transform );
            void setLocalTransform( const Core::Matrix4& transform );
            const Core::Transform& getLocalTransform() const;
            const Core::Matrix4& getLocalTransformAsMatrix() const;

        private:
            std::string m_name;
            RenderObjectType m_type;

            Component* m_component;

            RenderTechnique* m_renderTechnique;
            Mesh* m_mesh;

            Core::Transform m_localTransform;

            bool m_visible;
            bool m_isDirty;

            mutable std::mutex m_updateMutex;
        };

    } // namespace Engine
} // namespace Ra

#include <Engine/Renderer/RenderObject/RenderObject.inl>

#endif // RADIUMENGINE_RENDEROBJECT_HPP
