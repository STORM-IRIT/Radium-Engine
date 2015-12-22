#include <Plugins/Animation/Drawing/SkeletonBoneDrawable.hpp>

#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>

#include <Core/Animation/Handle/SkeletonUtils.hpp>

namespace AnimationPlugin
{

    SkeletonBoneRenderObject::SkeletonBoneRenderObject(const std::string& name, AnimationComponent* comp, uint id, Ra::Engine::RenderObjectManager* roMgr)
            : RenderObject(name, comp), m_skel( comp->getSkeleton() ), m_id( id ), m_roMgr( roMgr )
    {
        // TODO ( Val) common material / shader config...

        Ra::Engine::RenderObject* renderObject = new Ra::Engine::RenderObject( name, comp );
        Ra::Engine::ShaderConfiguration shader("BlinnPhong", "../Shaders");

        m_material.reset(new Ra::Engine::Material("Bone Material"));
        m_material->setKd(Ra::Core::Color(0.4, 0.4, 0.4, 0.5));
        m_material->setKs(Ra::Core::Color(0.0, 0.0, 0.0, 1.0));
        m_material->setMaterialType(Ra::Engine::Material::MaterialType::MAT_OPAQUE);

        m_renderParams.reset(new Ra::Engine::RenderTechnique());
        {
            m_renderParams->shaderConfig = shader;
            m_renderParams->material = m_material.get();
        }
        renderObject->setRenderTechnique(m_renderParams.get());
        renderObject->setType(Ra::Engine::RenderObject::Type::RO_XRAY);


        std::shared_ptr<Ra::Engine::Mesh> displayMesh( new Ra::Engine::Mesh( name ) );

        displayMesh->loadGeometry( makeBoneShape() );
        renderObject->setMesh( displayMesh );

        updateLocalTransform(renderObject);

        idx = m_roMgr->addRenderObject(renderObject);
    }

    void SkeletonBoneRenderObject::update()
    {
        std::shared_ptr<Ra::Engine::RenderObject> ro = m_roMgr->update( idx, false );
        updateLocalTransform( ro.get() );
        m_roMgr->doneUpdating( idx );
    }

    void SkeletonBoneRenderObject::updateLocalTransform(Ra::Engine::RenderObject* ro)
    {
        Ra::Core::Vector3 start;
        Ra::Core::Vector3 end;
        Ra::Core::Animation::SkeletonUtils::getBonePoints( m_skel, m_id, start, end );

        Ra::Core::Transform scale = Ra::Core::Transform::Identity();
        scale.scale((end - start).norm());

        Ra::Core::Quaternion rot = Ra::Core::Quaternion::FromTwoVectors(Ra::Core::Vector3::UnitZ(), end - start);

        Ra::Core::Transform boneTransform = m_skel.getTransform( m_id, Ra::Core::Animation::Handle::SpaceType::MODEL );
        Ra::Core::Matrix3 rotation = rot.toRotationMatrix();
        Ra::Core::Transform drawTransform;
        drawTransform.linear() =  rotation;
        drawTransform.translation() = boneTransform.translation();

        ro->setLocalTransform(drawTransform * scale);

        //std::cout << "update : " << m_edge << std::endl << (drawTransform * scale).matrix() << std::endl;
    }

    Ra::Core::TriangleMesh SkeletonBoneRenderObject::makeBoneShape()
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

}
