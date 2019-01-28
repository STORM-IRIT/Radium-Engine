#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <Engine/RadiumEngine.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Renderer/Displayable/DisplayableObject.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>

// STRANGE : only needed to access to the entity and its transform for components --> refactor
// component to give this directly ?
#include <Engine/Entity/Entity.hpp>

// Only needed to access the ViewingParameters struct
#include <Engine/Renderer/Camera/Camera.hpp>

namespace Ra {
namespace Engine {
RenderObject::RenderObject( const std::string& name, Component* comp, const RenderObjectType& type,
                            int lifetime ) :
    IndexedObject(),
    m_component{comp},
    m_name{name},
    m_type{type},
    m_mesh{nullptr},
    m_lifetime{lifetime},
    m_hasLifetime{lifetime > 0} {}

RenderObject::~RenderObject() = default;

RenderObject* RenderObject::createRenderObject( const std::string& name, Component* comp,
                                                const RenderObjectType& type,
                                                const std::shared_ptr<Displayable>& mesh,
                                                const RenderTechnique& techniqueConfig,
                                                const std::shared_ptr<Material>& material ) {
    auto obj = new RenderObject( name, comp, type );
    obj->setMesh( mesh );
    obj->setVisible( true );

    auto rt = std::make_shared<RenderTechnique>( techniqueConfig );

    if ( material != nullptr )
    {
        rt->setMaterial( material );
    }

    obj->setRenderTechnique( rt );

    return obj;
}

void RenderObject::updateGL() {
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

const RenderObjectType& RenderObject::getType() const {
    return m_type;
}

void RenderObject::setType( const RenderObjectType& t ) {
    m_type = t;
}

const std::string& RenderObject::getName() const {
    return m_name;
}

void RenderObject::setVisible( bool visible ) {
    m_visible = visible;
}

void RenderObject::toggleVisible() {
    m_visible = !m_visible;
}

bool RenderObject::isVisible() const {
    return m_visible;
}

void RenderObject::setPickable( bool pickable ) {
    m_pickable = pickable;
}

void RenderObject::togglePickable() {
    m_pickable = !m_pickable;
}

bool RenderObject::isPickable() const {
    return m_pickable;
}

void RenderObject::setXRay( bool xray ) {
    m_xray = xray;
}

void RenderObject::toggleXRay() {
    m_xray = !m_xray;
}

bool RenderObject::isXRay() const {
    return m_xray;
}

void RenderObject::setTransparent( bool transparent ) {
    m_transparent = transparent;
}

void RenderObject::toggleTransparent() {
    m_transparent = !m_transparent;
}

bool RenderObject::isTransparent() const {
    return m_transparent;
}

bool RenderObject::isDirty() const {
    return m_dirty;
}

const Component* RenderObject::getComponent() const {
    return m_component;
}

Component* RenderObject::getComponent() {
    return m_component;
}

void RenderObject::setRenderTechnique( const std::shared_ptr<RenderTechnique>& technique ) {
    CORE_ASSERT( technique, "Passing a nullptr as render technique" );
    m_renderTechnique = technique;
}

std::shared_ptr<const RenderTechnique> RenderObject::getRenderTechnique() const {
    return m_renderTechnique;
}

std::shared_ptr<RenderTechnique> RenderObject::getRenderTechnique() {
    return m_renderTechnique;
}

void RenderObject::setMesh( const std::shared_ptr<Displayable>& mesh ) {
    m_mesh = mesh;
}

std::shared_ptr<const Displayable> RenderObject::getMesh() const {
    return m_mesh;
}

const std::shared_ptr<Displayable>& RenderObject::getMesh() {
    return m_mesh;
}

Core::Transform RenderObject::getTransform() const {
    return m_component->getEntity()->getTransform() * m_localTransform;
}

Core::Matrix4 RenderObject::getTransformAsMatrix() const {
    return getTransform().matrix();
}

Core::Aabb RenderObject::getAabb() const {
    Core::Aabb aabb = m_mesh->getGeometry().computeAabb();
    Core::Aabb result;

    for ( int i = 0; i < 8; ++i )
    {
        result.extend( getTransform() * aabb.corner( Core::Aabb::CornerType( i ) ) );
    }

    return result;
}

void RenderObject::setLocalTransform( const Core::Transform& transform ) {
    m_localTransform = transform;
}

void RenderObject::setLocalTransform( const Core::Matrix4& transform ) {
    m_localTransform = Core::Transform( transform );
}

const Core::Transform& RenderObject::getLocalTransform() const {
    return m_localTransform;
}

const Core::Matrix4& RenderObject::getLocalTransformAsMatrix() const {
    return m_localTransform.matrix();
}

void RenderObject::hasBeenRenderedOnce() {
    if ( m_hasLifetime )
    {
        if ( --m_lifetime <= 0 )
        {
            RadiumEngine::getInstance()->getRenderObjectManager()->renderObjectExpired( m_idx );
        }
    }
}

void RenderObject::hasExpired() {
    m_component->notifyRenderObjectExpired( m_idx );
}

void RenderObject::render( const RenderParameters& lightParams, const ViewingParameters& viewParams,
                           const ShaderProgram* shader ) {
    if ( m_visible )
    {
        if ( !shader )
        {
            return;
        }

        // Radium V2 : avoid this temporary
        Core::Matrix4 modelMatrix = getTransformAsMatrix();
        Core::Matrix4 normalMatrix = modelMatrix.inverse().transpose();
        // bind data
        shader->bind();
        shader->setUniform( "transform.proj", viewParams.projMatrix );
        shader->setUniform( "transform.view", viewParams.viewMatrix );
        shader->setUniform( "transform.model", modelMatrix );
        shader->setUniform( "transform.worldNormal", normalMatrix );
        lightParams.bind( shader );

        auto material = m_renderTechnique->getMaterial();
        if ( material != nullptr )
            material->bind( shader );

        // render
        getMesh()->render();
    }
}

void RenderObject::render( const RenderParameters& lightParams, const ViewingParameters& viewParams,
                           RenderTechnique::PassName passname ) {
    render( lightParams, viewParams, getRenderTechnique()->getShader( passname ) );
}

} // namespace Engine
} // namespace Ra
