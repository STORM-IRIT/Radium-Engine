#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <Core/Containers/MakeShared.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Geometry/Normal/Normal.hpp>

#include <Engine/Assets/GeometryData.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
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
            , m_transparent( false )
            , m_dirty( true )
            , m_hasLifetime( lifetime > 0 )
        {
        }

        RenderObject::~RenderObject()
        {
        }

        RenderObject* RenderObject::createRenderObject(const std::string& name, Component* comp,
                                                       const RenderObjectType& type, const std::shared_ptr<Mesh> &mesh,
                                                       const ShaderConfiguration &shaderConfig, Material *material)
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
                rt->material->m_kd = Core::Color::Constant(0.9f);
                rt->material->m_ks = Core::Color::Zero();
            }

            obj->setRenderTechnique(rt);

            return obj;
        }

        RenderObject* RenderObject::createFancyFromAsset(const std::string& name, Component* comp, const Asset::GeometryData* asset, bool allow_transparency)
        {
            auto displayMesh = Core::make_shared<Ra::Engine::Mesh>(name);

            Core::TriangleMesh mesh;
            Core::Transform T = asset->getFrame();
            Core::Transform N;
            N.matrix() = (T.matrix()).inverse().transpose();
            
            for (size_t i = 0; i < asset->getVerticesSize(); ++i)
            {
                mesh.m_vertices.push_back(T * asset->getVertices()[i]);
                mesh.m_normals.push_back((N * asset->getNormals()[i]).normalized());
            }

            for (const auto& face : asset->getFaces())
            {
                mesh.m_triangles.push_back(face.head<3>());
            }

            //Core::Geometry::uniformNormal(mesh.m_vertices, mesh.m_triangles, mesh.m_normals);
            displayMesh->loadGeometry(mesh);

            Core::Vector3Array tangents;
            Core::Vector3Array bitangents;
            Core::Vector3Array texcoords;

            Core::Vector4Array colors;

            for (const auto& v : asset->getTangents())   tangents.push_back(v);
            for (const auto& v : asset->getBiTangents()) bitangents.push_back(v);
            for (const auto& v : asset->getTexCoords())  texcoords.push_back(v);
            for (const auto& v : asset->getColors())     colors.push_back(v);

            displayMesh->addData(Mesh::VERTEX_TANGENT, tangents);
            displayMesh->addData(Mesh::VERTEX_BITANGENT, bitangents);
            displayMesh->addData(Mesh::VERTEX_TEXCOORD, texcoords);
            displayMesh->addData(Mesh::VERTEX_COLOR, colors);

            Material* mat = new Material(name);

            auto m = asset->getMaterial();

            if (m.hasDiffuse())
            {
                mat->m_kd = m.m_diffuse;
            }

            if (m.hasSpecular())
            {
                mat->m_ks = m.m_specular;
            }

            if (m.hasShininess())
            {
                mat->m_ns = m.m_shininess;
            }

            if (m.hasOpacity())
            {
                mat->m_alpha = m.m_opacity;
            }
            
#ifdef LOAD_TEXTURES
            if (m.hasDiffuseTexture())
            {
                mat->addTexture(Material::TextureType::TEX_DIFFUSE, m.m_texDiffuse);
            }

            if (m.hasSpecularTexture())
            {
                mat->addTexture(Material::TextureType::TEX_SPECULAR, m.m_texSpecular);
            }

            if (m.hasShininessTexture())
            {
                mat->addTexture(Material::TextureType::TEX_SHININESS, m.m_texShininess);
            }

            if (m.hasOpacityTexture())
            {
                mat->addTexture(Material::TextureType::TEX_ALPHA, m.m_texOpacity);
            }

            if (m.hasNormalTexture())
            {
                mat->addTexture(Material::TextureType::TEX_NORMAL, m.m_texNormal);
            }
#endif
            
            auto shaderConfig = ShaderConfigurationFactory::getConfiguration("BlinnPhong");
            auto result = createRenderObject(name, comp, RenderObjectType::Fancy, displayMesh, shaderConfig, mat);
            
            if (allow_transparency && mat->m_alpha < 1.0)
            {
                result->setTransparent(true);
            }
            
            return result;
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

        void RenderObject::setTransparent( bool transparent )
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

        void RenderObject::render( const RenderParameters& lightParams, const RenderData& rdata, const ShaderProgram* altShader )
        {
            const ShaderProgram* shader;

            if (m_visible)
            {
                shader = (altShader == nullptr) ? getRenderTechnique()->shader : altShader;

                if (!shader)
                {
                    return;
                }

                Core::Matrix4 M = getTransformAsMatrix();
                Core::Matrix4 N = M.inverse().transpose();

                // bind data
                shader->bind();
                shader->setUniform( "transform.proj", rdata.projMatrix );
                shader->setUniform( "transform.view", rdata.viewMatrix );
                shader->setUniform( "transform.model", M );
                shader->setUniform( "transform.worldNormal", N );
                lightParams.bind( shader );

                getRenderTechnique()->material->bind( shader );

                // render
                getMesh()->render();
            }
        }

    } // namespace Engine
} // namespace Ra

