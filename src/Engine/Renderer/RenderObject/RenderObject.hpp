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

            RenderObject( const std::string& name, const Component* comp,
                          const RenderObjectType& type );
            ~RenderObject();

            // FIXME(Charly): Remove this
            void updateGL();


            //
            // Getters and setters.
            //
            const std::string& getName() const;
            const Component* getComponent() const;

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
            const Mesh* getMesh() const;
            Mesh* getMesh();

            Core::Transform getTransform() const;
            Core::Matrix4 getTransformAsMatrix() const;

            void setLocalTransform( const Core::Transform& transform );
            void setLocalTransform( const Core::Matrix4& transform );
            const Core::Transform& getLocalTransform() const;
            const Core::Matrix4& getLocalTransformAsMatrix() const;


        private:
            Core::Transform m_localTransform;

            const Component* m_component;
            std::string m_name;

            RenderObjectType m_type;
            RenderTechnique* m_renderTechnique;
            std::shared_ptr<Mesh> m_mesh;

            // FIXME(Charly): Remove this
            RenderParameters m_renderParameters;

            bool m_visible;
            bool m_xray;
            bool m_dirty;

            mutable std::mutex m_updateMutex;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDEROBJECT_HPP
