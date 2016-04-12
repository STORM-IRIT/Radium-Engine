#ifndef RADIUMENGINE_RENDEROBJECT_HPP
#define RADIUMENGINE_RENDEROBJECT_HPP

#include <Engine/RaEngine.hpp>

#include <string>
#include <mutex>
#include <memory>

#include <Core/Index/IndexedObject.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>

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
        class RA_ENGINE_API RenderObject : public Core::IndexedObject
        {
        public:
            RA_CORE_ALIGNED_NEW

            /// A -1 (or any other negative value) lifetime is considered infinite,
            /// 0 is an "invalid value" (would mean the render object has to die immediatly),
            /// hence it's considered as infinite,
            /// any other positive value will be taken into account.
            RenderObject( const std::string& name, Component* comp,
                          const RenderObjectType& type, int lifetime = -1 );
            ~RenderObject();

            static RenderObject* createRenderObject(const std::string& name, Component* comp, const RenderObjectType& type, const std::shared_ptr<Mesh>& mesh, const ShaderConfiguration& shaderConfig = ShaderConfiguration(), Material* material = nullptr);

            // FIXME(Charly): Remove this
            void updateGL();

            //
            // Getters and setters.
            //
            const std::string& getName() const;
            const Component* getComponent() const;
                  Component* getComponent();

            const RenderObjectType& getType() const;
            void setType( const RenderObjectType& t);

            void setVisible( bool visible );
            void toggleVisible();
            bool isVisible() const;

            void setXRay( bool xray );
            void toggleXRay();
            bool isXRay() const;

            bool isDirty() const;

            void setRenderTechnique( RenderTechnique* technique );
            const RenderTechnique* getRenderTechnique() const;
            RenderTechnique* getRenderTechnique();

            void setMesh( const std::shared_ptr<Mesh>& mesh );
            std::shared_ptr<const Mesh> getMesh() const;
            const std::shared_ptr<Mesh>& getMesh();

            Core::Transform getTransform() const;
            Core::Matrix4 getTransformAsMatrix() const;

            Core::Aabb getAabb() const;

            void setLocalTransform( const Core::Transform& transform );
            void setLocalTransform( const Core::Matrix4& transform );
            const Core::Transform& getLocalTransform() const;
            const Core::Matrix4& getLocalTransformAsMatrix() const;

            /// Basically just decreases lifetime counter.
            /// If it goes to zero, then render object notifies the manager that it needs to be deleted.
            /// Does nothing if lifetime is set to -1
            void hasBeenRenderedOnce();
            void hasExpired();

        private:
            Core::Transform m_localTransform;

            Component* m_component;
            std::string m_name;

            RenderObjectType m_type;
            RenderTechnique* m_renderTechnique;
            std::shared_ptr<Mesh> m_mesh;

            // FIXME(Charly): Remove this
            RenderParameters m_renderParameters;

            mutable std::mutex m_updateMutex;

            int m_lifetime;

            bool m_visible;
            bool m_xray;
            bool m_dirty;
            bool m_hasLifetime;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDEROBJECT_HPP
