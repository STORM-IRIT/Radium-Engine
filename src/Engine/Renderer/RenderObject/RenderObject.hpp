#ifndef RADIUMENGINE_RENDEROBJECT_HPP
#define RADIUMENGINE_RENDEROBJECT_HPP

#include <Engine/RaEngine.hpp>

#include <string>
#include <mutex>
#include <memory>

#include <Core/Index/IndexedObject.hpp>
#include <Core/Math/LinearAlgebra.hpp>

#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

namespace Ra
{
    namespace Engine
    {
        class Light;
        class Component;
        class Mesh;
        class RenderQueue;
        class Material;
        class RenderParameters;
        class ShaderProgram;
    }
}

namespace Ra
{
    namespace Engine
    {
        
        struct RenderData;
        
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
            
            /// Sort of factory method to easily create a render object.
            /// Use case example :
            ///     std::string name = "MyRO";
            ///     Component* comp;    // Retrieve the component the way you want.
            ///                         // Since this method will often be used in a component,
            ///                         // just use this pointer.
            ///     RenderObjectType type = RenderObjectType::Fancy; // For example
            ///     // Retrieve an already created configuration, or create one (see ShaderConfiguration docs).
            ///     ShaderConfiguration config = ShaderConfigurationFactory::getConfiguration("MyConfig");
            ///     Material* mat = new Material; // Then configure your material...
            ///     // createRenderObject can finally be called.
            ///     RenderObject* ro = createRenderObject(name, component, type, config, material);
            /// TODO : update the above documentation to match the new profile and use case ...
            /*
             static RenderObject* createRenderObject( const std::string& name, Component* comp,
             const RenderObjectType& type, const std::shared_ptr<Mesh>& mesh,
             const ShaderConfiguration& shaderConfig = ShaderConfigurationFactory::getConfiguration("BlinnPhong"),
             const std::shared_ptr<Material>& material = nullptr );
             */
            static RenderObject* createRenderObject( const std::string& name, Component* comp,
                                                    const RenderObjectType& type,
                                                    const std::shared_ptr<Mesh>& mesh,
                                                    const RenderTechnique& techniqueConfig = RenderTechnique::createDefaultRenderTechnique(),
                                                    const std::shared_ptr<Material>& material = nullptr );
            
            
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

            void setPickable( bool pickable );
            void togglePickable();
            bool isPickable() const;

            void setXRay( bool xray );
            void toggleXRay();
            bool isXRay() const;

            void setTransparent( bool transparent );
            void toggleTransparent();
            bool isTransparent() const;

            bool isDirty() const;

            void setRenderTechnique( const std::shared_ptr<RenderTechnique>& technique );
            std::shared_ptr<const RenderTechnique> getRenderTechnique() const;
            std::shared_ptr<RenderTechnique> getRenderTechnique();

            void setMesh( const std::shared_ptr<Mesh>& mesh );
            std::shared_ptr<const Mesh> getMesh() const;
            const std::shared_ptr<Mesh>& getMesh();

            Core::Transform getTransform() const;
            Core::Matrix4 getTransformAsMatrix() const;

            Core::Aabb getAabb() const;
            Core::Aabb getMeshAabb() const;

            void setLocalTransform( const Core::Transform& transform );
            void setLocalTransform( const Core::Matrix4& transform );
            const Core::Transform& getLocalTransform() const;
            const Core::Matrix4& getLocalTransformAsMatrix() const;

            /// Basically just decreases lifetime counter.
            /// If it goes to zero, then render object notifies the manager that it needs to be deleted.
            /// Does nothing if lifetime is set to -1
            void hasBeenRenderedOnce();
            void hasExpired();
            
            // virtual void render( const RenderParameters& lightParams, const RenderData& rdata, const ShaderProgram* altShader = nullptr );
            virtual void render( const RenderParameters& lightParams, const RenderData& rdata, RenderTechnique::PassName passname = RenderTechnique::LIGHTING_OPAQUE );

            virtual void renderWithExplicitRenderTechnique( const RenderParameters& lightParams, const RenderData& rdata, const RenderTechnique& renderTechnique, RenderTechnique::PassName passname = RenderTechnique::LIGHTING_OPAQUE );

        private:
            Core::Transform m_localTransform;

            Component* m_component;
            std::string m_name;

            RenderObjectType m_type;
            std::shared_ptr<RenderTechnique> m_renderTechnique;
            std::shared_ptr<Mesh> m_mesh;

            mutable std::mutex m_updateMutex;

            int m_lifetime;

            bool m_visible;
            bool m_pickable;
            bool m_xray;
            bool m_transparent;
            bool m_dirty;
            bool m_hasLifetime;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDEROBJECT_HPP
