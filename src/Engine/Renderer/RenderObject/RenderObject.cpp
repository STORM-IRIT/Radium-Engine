#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

namespace Ra
{
    namespace Engine
    {
        RenderObject::RenderObject( const std::string& name, Component* comp,
                                    const RenderObjectType& type, int lifetime )
            : IndexedObject()
            , m_localTransform( Core::Transform::Identity() )
            , m_component( comp )
            , m_name( name )
            , m_type( type )
            , m_renderTechnique( nullptr )
            , m_mesh( nullptr )
            , m_lifetime( lifetime )
            , m_visible( true )
            , m_xray( false )
            , m_dirty( true )
            , m_hasLifetime( lifetime > 0 )
        {
        }

        RenderObject::~RenderObject()
        {
        }

        RenderObject* RenderObject::createRenderObject(const std::string& name, Component* comp, const RenderObjectType& type, const std::shared_ptr<Mesh> &mesh, const ShaderConfiguration &shaderConfig, Material *material)
        {
            RenderObject* obj = new RenderObject(name, comp, type);
            obj->setMesh(mesh);
            obj->setVisible(true);

            RenderTechnique* rt = new RenderTechnique;

            if (shaderConfig.isComplete())
            {
                rt->shaderConfig = shaderConfig;
            }
            else
            {
                rt->shaderConfig = ShaderProgramManager::getInstance()->getDefaultShaderProgram()->getBasicConfiguration();
            }

            if (material != nullptr)
            {
                rt->material = material;
            }
            else
            {
                // Lightgrey non specular material by default
                rt->material = new Material(name + "_Mat");
                rt->material->setKd(Core::Color::Constant(0.9));
                rt->material->setKs(Core::Color::Zero());
            }

            obj->setRenderTechnique(rt);

            return obj;
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

        Component* RenderObject::getComponent()
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

        std::shared_ptr<const Mesh> RenderObject::getMesh() const
        {
            return m_mesh;
        }

        const std::shared_ptr<Mesh>& RenderObject::getMesh()
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

            Core::Vector3 min(aabb.min());
            Core::Vector3 max(aabb.max());

            Eigen::Matrix<Scalar, 8, 4> vertices;
            vertices << min(0), min(1), min(2), 1,  // Left  Bottom Near
                        min(0), min(1), max(2), 1,  // Left  Bottom Far
                        max(0), min(1), min(2), 1,  // Right Bottom Near
                        max(0), min(1), max(2), 1,  // Right Bottom Far
                        min(0), max(1), min(2), 1,  // Left  Top    Near
                        min(0), max(1), max(2), 1,  // Left  Top    Far
                        max(0), max(1), min(2), 1,  // Right Top    Near
                        max(0), max(1), max(2), 1;  // Right Top    Far

            Core::Matrix4 trans = getTransformAsMatrix();

            vertices *= trans;

            aabb = Core::Aabb(Core::Vector3(vertices.col(0).minCoeff(),
                                            vertices.col(1).minCoeff(),
                                            vertices.col(2).minCoeff()));
            aabb.extend(Core::Vector3(vertices.col(0).maxCoeff(),
                                      vertices.col(1).maxCoeff(),
                                      vertices.col(2).maxCoeff()));


            return aabb;
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

        void RenderObject::hasBeenRenderedOnce()
        {
            if ( m_hasLifetime )
            {
                if ( --m_lifetime <= 0 )
                {
                    RadiumEngine::getInstance()->getRenderObjectManager()->renderObjectExpired( idx );
                }
            }
        }

        void RenderObject::hasExpired()
        {
            m_component->notifyRenderObjectExpired( idx );
        }

    } // namespace Engine
} // namespace Ra

