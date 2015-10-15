#ifndef ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_
#define ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_

#include <Plugins/Animation/AnimationPlugin.hpp>

#include <Core/Mesh/MeshUtils.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Plugins/Animation/AnimationComponent.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>
#include <iostream>

namespace AnimationPlugin
{
class SkeletonBoneRenderObject : public Ra::Engine::RenderObject
{
public:
    SkeletonBoneRenderObject(const std::string& name, const AnimationComponent* comp, Ra::Core::Edge edge )
        : RenderObject(name, comp), m_skel( comp->getSkeleton()), m_edge( edge )
    {
        // TODO ( Val) common material / shader config...
        Ra::Engine::ShaderConfiguration shader("BlinnPhong", "../Shaders");

        m_material.reset(new Ra::Engine::Material("Bone Material"));
        m_material->setKd(Ra::Core::Color(0.4, 0.4, 0.4, 0.5));
        m_material->setKs(Ra::Core::Color(0.0, 0.0, 0.0, 1.0));
        m_material->setMaterialType(Ra::Engine::Material::MaterialType::MAT_TRANSPARENT);

        m_renderParams.reset(new Ra::Engine::RenderTechnique());
        {
            m_renderParams->shaderConfig = shader;
            m_renderParams->material = m_material.get();
        }
        setRenderTechnique(m_renderParams.get());
        setType(Ra::Engine::RenderObject::Type::RO_TRANSPARENT);

        Ra::Engine::Mesh* displayMesh = new Ra::Engine::Mesh( name );

        displayMesh->loadGeometry( makeBoneShape() );
        setMesh( displayMesh );
        
        updateLocalTransform();
    }

    void updateLocalTransform()
    {
        Ra::Core::Vector3 start = m_skel.getTransform( m_edge( 0 ), Ra::Core::Animation::Handle::SpaceType::MODEL ).translation();
        Ra::Core::Vector3 end = m_skel.getTransform( m_edge( 1 ), Ra::Core::Animation::Handle::SpaceType::MODEL ).translation();

        Ra::Core::Transform scale = Ra::Core::Transform::Identity();
        scale.scale((end - start).norm());

        Ra::Core::Quaternion rot = Ra::Core::Quaternion::FromTwoVectors(Ra::Core::Vector3::UnitZ(), end - start);

        Ra::Core::Transform boneTransform = m_skel.getTransform( m_edge( 0 ), Ra::Core::Animation::Handle::SpaceType::MODEL );
        Ra::Core::Matrix3 rotation = rot.toRotationMatrix();
        Ra::Core::Transform drawTransform;
        drawTransform.linear() =  rotation;
        drawTransform.translation() = boneTransform.translation();

        setLocalTransform(drawTransform * scale );
    }
    
    static Ra::Core::TriangleMesh makeBoneShape()
    {
        // Bone along Z axis.

        Ra::Core::TriangleMesh mesh;

        const Scalar l = 0.1f;
        const Scalar w = 0.1f;

        mesh.m_vertices = {
            Ra::Core::Vector3(0,0,0), Ra::Core::Vector3(0,0,1),
            Ra::Core::Vector3(0,w,l), Ra::Core::Vector3(w,0,l),
            Ra::Core::Vector3(0,-w,l), Ra::Core::Vector3(-w,0,l)
        };

        mesh.m_triangles = {
            Ra::Core::Triangle(0,2,3),Ra::Core::Triangle(0,5,2),
            Ra::Core::Triangle(0,3,4),Ra::Core::Triangle(0,4,5),
            Ra::Core::Triangle(1,3,2),Ra::Core::Triangle(1,2,5),
            Ra::Core::Triangle(1,4,3),Ra::Core::Triangle(1,5,4)
        };
        Ra::Core::MeshUtils::getAutoNormals(mesh, mesh.m_normals);
        return mesh;
    }

    inline uint getParent() const {
        return m_edge( 0 );
    }

    inline uint getChild() const {
        return m_edge( 1 );
    }

    protected:
        const Ra::Core::Animation::Skeleton& m_skel;
        Ra::Core::Edge m_edge;
        std::unique_ptr<Ra::Engine::RenderTechnique> m_renderParams;
        std::unique_ptr<Ra::Engine::Material> m_material;
};

}

#endif //ANIMPLUGIN_SKELETON_BONE_DRAWABLE_HPP_
