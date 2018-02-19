#include <Drawing/SkeletonBoneDrawable.hpp>

#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

namespace AnimationPlugin {

SkeletonBoneRenderObject::SkeletonBoneRenderObject( const std::string& name,
                                                    AnimationComponent* comp, uint id,
                                                    Ra::Engine::RenderObjectManager* roMgr ) :
    m_roIdx( Ra::Core::Index::Invalid() ),
    m_id( id ),
    m_skel( comp->getSkeleton() ),
    m_roMgr( roMgr ) {
    // TODO ( Val) common material / shader config...

    // FIXME(Charly): Debug or fancy ?
    Ra::Engine::RenderObject* renderObject =
        new Ra::Engine::RenderObject( name, comp, Ra::Engine::RenderObjectType::Fancy );
    renderObject->setXRay( false );

    Ra::Engine::ShaderConfiguration shader =
        Ra::Engine::ShaderConfigurationFactory::getConfiguration( "BlinnPhong" );
    auto bpMaterial = new Ra::Engine::BlinnPhongMaterial( "Bone Material" );
    m_material.reset( bpMaterial );
    bpMaterial->m_kd = Ra::Core::Color( 0.4f, 0.4f, 0.4f, 0.5f );
    bpMaterial->m_ks = Ra::Core::Color( 0.0f, 0.0f, 0.0f, 1.0f );
    m_material->setMaterialAspect(Ra::Engine::Material::MaterialAspect::MAT_OPAQUE);

    m_renderParams.reset( new Ra::Engine::RenderTechnique() );
    {
        m_renderParams->setConfiguration( shader );
        m_renderParams->setMaterial( m_material );
    }
    renderObject->setRenderTechnique( m_renderParams );

    std::shared_ptr<Ra::Engine::Mesh> displayMesh( new Ra::Engine::Mesh( name ) );

    displayMesh->loadGeometry( makeBoneShape() );
    renderObject->setMesh( displayMesh );

    m_roIdx = m_roMgr->addRenderObject( renderObject );
    updateLocalTransform();
}

void SkeletonBoneRenderObject::update() {
    updateLocalTransform();
}

void SkeletonBoneRenderObject::updateLocalTransform() {
    Ra::Core::Vector3 start;
    Ra::Core::Vector3 end;
    m_skel.getBonePoints( m_id, start, end );

    Ra::Core::Transform scale = Ra::Core::Transform::Identity();
    scale.scale( ( end - start ).norm() );

    Ra::Core::Quaternion rot =
        Ra::Core::Quaternion::FromTwoVectors( Ra::Core::Vector3::UnitZ(), end - start );

    Ra::Core::Transform boneTransform =
        m_skel.getTransform( m_id, Ra::Core::Animation::Handle::SpaceType::MODEL );
    Ra::Core::Matrix3 rotation = rot.toRotationMatrix();
    Ra::Core::Transform drawTransform;
    drawTransform.linear() = rotation;
    drawTransform.translation() = boneTransform.translation();

    m_roMgr->getRenderObject( m_roIdx )->setLocalTransform( drawTransform * scale );
}

Ra::Core::TriangleMesh SkeletonBoneRenderObject::makeBoneShape() {
    // Bone along Z axis.

    Ra::Core::TriangleMesh mesh;

    const Scalar l = 0.1f;
    const Scalar w = 0.1f;

    mesh.vertices() = {Ra::Core::Vector3( 0, 0, 0 ),  Ra::Core::Vector3( 0, 0, 1 ),
                       Ra::Core::Vector3( 0, w, l ),  Ra::Core::Vector3( w, 0, l ),
                       Ra::Core::Vector3( 0, -w, l ), Ra::Core::Vector3( -w, 0, l )};

    mesh.m_triangles = {Ra::Core::Triangle( 0, 2, 3 ), Ra::Core::Triangle( 0, 5, 2 ),
                        Ra::Core::Triangle( 0, 3, 4 ), Ra::Core::Triangle( 0, 4, 5 ),
                        Ra::Core::Triangle( 1, 3, 2 ), Ra::Core::Triangle( 1, 2, 5 ),
                        Ra::Core::Triangle( 1, 4, 3 ), Ra::Core::Triangle( 1, 5, 4 )};
    Ra::Core::MeshUtils::getAutoNormals( mesh, mesh.normals() );
    return mesh;
}

void SkeletonBoneRenderObject::setXray( bool on ) {
    std::shared_ptr<Ra::Engine::RenderObject> ro = m_roMgr->getRenderObject( m_roIdx );
    ro->setXRay( on );
}
} // namespace AnimationPlugin
