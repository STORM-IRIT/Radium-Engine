#ifndef RADIUMENGINE_RENDEROBJECT_HPP
#define RADIUMENGINE_RENDEROBJECT_HPP

#include <Engine/RaEngine.hpp>

#include <string>
#include <mutex>
#include <memory>

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
        class RA_ENGINE_API RenderObject : public Core::IndexedObject
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

            RA_CORE_ALIGNED_NEW

            RenderObject( const std::string& name, const Component* comp );
            ~RenderObject();

            //
            // Drawing related functions
            //
            void updateGL();

            void feedRenderQueue( RenderQueue& queue,
                                  const Core::Matrix4& view,
                                  const Core::Matrix4& proj );

            RenderObject* clone();

            //
            // Getters and setters.
            //
            inline const std::string& getName() const;
            inline const Component* getComponent() const;

            inline void setRenderObjectType( const RenderObjectType& type );
            inline const RenderObjectType& getRenderObjectType() const;

            inline void setVisible( bool visible );
            inline bool isVisible() const;

            inline bool isDirty() const;

            inline void setRenderTechnique( RenderTechnique* technique );
            inline RenderTechnique* getRenderTechnique() const;

            inline void setMesh( Mesh* mesh );
            inline Mesh* getMesh() const;

            inline void setLocalTransform( const Core::Transform& transform );
            inline void setLocalTransform( const Core::Matrix4& transform );
            inline const Core::Transform& getLocalTransform() const;
            inline const Core::Matrix4& getLocalTransformAsMatrix() const;

        private:
            Core::Transform m_localTransform;

            const Component* m_component;
            std::string m_name;

            RenderObjectType m_type;
            RenderTechnique* m_renderTechnique;
            Mesh* m_mesh;


            bool m_visible;
            bool m_isDirty;

            mutable std::mutex m_updateMutex;
        };

    } // namespace Engine
} // namespace Ra

#include <Engine/Renderer/RenderObject/RenderObject.inl>

#endif // RADIUMENGINE_RENDEROBJECT_HPP
