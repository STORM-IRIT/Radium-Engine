#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <Core/Containers/MakeShared.hpp>
#include <Core/File/GeometryData.hpp>
#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/Mesh/MeshUtils.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>

namespace Ra {
    namespace Engine {
        RenderObject::RenderObject(const std::string &name, Component *comp,
                                   const RenderObjectType &type, int lifetime)
        : IndexedObject(), m_localTransform(Core::Transform::Identity()), m_component(comp), m_name(name), m_type(type),
        m_renderTechnique(nullptr), m_mesh(nullptr), m_lifetime(lifetime), m_visible(true), m_pickable(true),
        m_xray(false), m_transparent(false), m_dirty(true), m_hasLifetime(lifetime > 0)
        {
        }
        
        RenderObject::~RenderObject()
        {
        }
        
        RenderObject *RenderObject::createRenderObject(const std::string &name, Component *comp,
                                                       const RenderObjectType &type,
                                                       const std::shared_ptr<Mesh> &mesh,
                                                       const RenderTechnique &techniqueConfig,
                                                       const std::shared_ptr<Material> &material)
        {
            RenderObject *obj = new RenderObject(name, comp, type);
            obj->setMesh(mesh);
            obj->setVisible(true);
            
            std::shared_ptr<RenderTechnique> rt(new RenderTechnique(techniqueConfig));
            
            if (material != nullptr)
            {
                rt->setMaterial(material);
            }
            
            obj->setRenderTechnique(rt);
            
            return obj;
        }
        
        void RenderObject::updateGL()
        {
            // Do not update while we are cloning
            std::lock_guard<std::mutex> lock(m_updateMutex);
            
            if (m_renderTechnique)
            {
                m_renderTechnique->updateGL();
            }
            
            if (m_mesh)
            {
                m_mesh->updateGL();
            }
            
            m_dirty = false;
        }
        
        const RenderObjectType &RenderObject::getType() const
        {
            return m_type;
        }
        
        void RenderObject::setType(const RenderObjectType &t)
        {
            // Fixme (val) : this will have no effect now
            m_type = t;
        }
        
        const std::string &RenderObject::getName() const
        {
            return m_name;
        }
        
        void RenderObject::setVisible(bool visible)
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
        
        void RenderObject::setPickable(bool pickable)
        {
            m_pickable = pickable;
        }
        
        void RenderObject::togglePickable()
        {
            m_pickable = !m_pickable;
        }
        
        bool RenderObject::isPickable() const
        {
            return m_pickable;
        }
        
        void RenderObject::setXRay(bool xray)
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
        
        void RenderObject::setTransparent(bool transparent)
        {
            m_transparent = transparent;
        }
        
        void RenderObject::toggleTransparent()
        {
            m_transparent = !m_transparent;
        }
        
        bool RenderObject::isTransparent() const
        {
            return m_transparent;
        }
        
        bool RenderObject::isDirty() const
        {
            return m_dirty;
        }
        
        const Component *RenderObject::getComponent() const
        {
            return m_component;
        }
        
        Component *RenderObject::getComponent()
        {
            return m_component;
        }
        
        void RenderObject::setRenderTechnique(const std::shared_ptr<RenderTechnique> &technique)
        {
            CORE_ASSERT(technique, "Passing a nullptr as render technique");
            m_renderTechnique = technique;
        }
        
        std::shared_ptr<const RenderTechnique> RenderObject::getRenderTechnique() const
        {
            return m_renderTechnique;
        }
        
        std::shared_ptr<RenderTechnique> RenderObject::getRenderTechnique()
        {
            return m_renderTechnique;
        }
        
        void RenderObject::setMesh(const std::shared_ptr<Mesh> &mesh)
        {
            m_mesh = mesh;
        }
        
        std::shared_ptr<const Mesh> RenderObject::getMesh() const
        {
            return m_mesh;
        }
        
        const std::shared_ptr<Mesh> &RenderObject::getMesh()
        {
            return m_mesh;
        }
        
        Core::Transform RenderObject::getTransform() const
        {
            return m_component->getEntity()->getTransform() * m_localTransform;
        }
        
        Core::Matrix4 RenderObject::getTransformAsMatrix() const
        {
            return getTransform().matrix();
        }
        
        Core::Aabb RenderObject::getAabb() const
        {
            Core::Aabb aabb = Core::MeshUtils::getAabb(m_mesh->getGeometry());
            Core::Aabb result;
            
            for (int i = 0; i < 8; ++i)
            {
                result.extend(getTransform() * aabb.corner((Core::Aabb::CornerType) i));
            }
            
            return result;
        }
        
        Core::Aabb RenderObject::getMeshAabb() const
        {
            return Core::MeshUtils::getAabb(m_mesh->getGeometry());
        }
        
        void RenderObject::setLocalTransform(const Core::Transform &transform)
        {
            m_localTransform = transform;
        }
        
        void RenderObject::setLocalTransform(const Core::Matrix4 &transform)
        {
            m_localTransform = Core::Transform(transform);
        }
        
        const Core::Transform &RenderObject::getLocalTransform() const
        {
            return m_localTransform;
        }
        
        const Core::Matrix4 &RenderObject::getLocalTransformAsMatrix() const
        {
            return m_localTransform.matrix();
        }
        
        void RenderObject::hasBeenRenderedOnce()
        {
            if (m_hasLifetime)
            {
                if (--m_lifetime <= 0)
                {
                    RadiumEngine::getInstance()->getRenderObjectManager()->renderObjectExpired(idx);
                }
            }
        }
        
        void RenderObject::hasExpired()
        {
            m_component->notifyRenderObjectExpired(idx);
        }
        
        void RenderObject::render(const RenderParameters& lightParams,
                                   const RenderData& rdata,
                                   const ShaderProgram* shader ) {
            if (m_visible)
            {
                if (!shader)
                {
                    return;
                }

                Core::Matrix4 M = getTransformAsMatrix();
                Core::Matrix4 N = M.inverse().transpose();

                // bind data
                shader->bind();
                shader->setUniform("transform.proj", rdata.projMatrix);
                shader->setUniform("transform.view", rdata.viewMatrix);
                shader->setUniform("transform.model", M);
                shader->setUniform("transform.worldNormal", N);
                lightParams.bind(shader);

                // FIXME (Hugo) The simplest now, but not the expected behaviour.
                //renderTechnique.getMaterial()->bind(shader);
                getRenderTechnique()->getMaterial()->bind(shader);

                // render
                getMesh()->render();
            }
        }

        void RenderObject::render(const RenderParameters &lightParams,
                                  const RenderData &rdata,
                                  RenderTechnique::PassName passname)
        {
            render(lightParams, rdata, getRenderTechnique()->getShader(passname));
        }
        
    } // namespace Engine
} // namespace Ra

