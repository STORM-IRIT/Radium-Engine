#ifndef RADIUMENGINE_RENDEROBJECT_HPP
#define RADIUMENGINE_RENDEROBJECT_HPP

#include <Engine/RaEngine.hpp>

#include <memory>
#include <mutex>
#include <string>

#include <Core/Index/IndexedObject.hpp>
#include <Core/Math/LinearAlgebra.hpp>

#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

namespace Ra {
namespace Engine {
class Component;
class Mesh;
class Material;
class RenderParameters;
struct RenderData;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

/// The RenderObject class stores all needed data to render an object (mesh,
/// material, transformation...).
// FIXME(Charly): Does this need a bit of cleanup ?
class RA_ENGINE_API RenderObject final : public Core::IndexedObject {
  public:
    RA_CORE_ALIGNED_NEW

    /// A -1 (or any other negative value) lifetime is considered infinite,
    /// 0 is an "invalid value" (would mean the render object has to die immediatly),
    /// hence it's considered as infinite,
    /// any other positive value will be taken into account.
    RenderObject( const std::string& name, Component* comp, const RenderObjectType& type,
                  int lifetime = -1 );

    ~RenderObject();

    // clang-format off
    /// Sort of factory method to easily create a render object.
    /// Use case example :
    /// \code
    ///     std::string name = "MyRO";
    ///     Component* comp;    // Retrieve the component the way you want.
    ///                         // Since this method will often be used in a component,
    ///                         // just use this pointer.
    ///     RenderObjectType type = RenderObjectType::Fancy; // For example
    ///     // Retrieve an already created configuration, or create one (see ShaderConfiguration docs).
    ///     ShaderConfiguration config = ShaderConfigurationFactory::getConfiguration("MyConfig");
    ///     Material* mat = new Material;
    ///     // Then configure your material...
    ///     // createRenderObject can finally be called.
    ///     RenderObject* ro = createRenderObject(name, component, type, config, material);
    /// \endcode
    // clang-format on
    // TODO : update the above documentation to match the new profile and use case ...
    /*
     static RenderObject* createRenderObject( const std::string& name, Component* comp,
     const RenderObjectType& type, const std::shared_ptr<Mesh>& mesh,
     const ShaderConfiguration& shaderConfig =
     ShaderConfigurationFactory::getConfiguration("BlinnPhong"), const std::shared_ptr<Material>&
     material = nullptr );
     */
    static RenderObject* createRenderObject(
        const std::string& name, Component* comp, const RenderObjectType& type,
        const std::shared_ptr<Mesh>& mesh,
        const RenderTechnique& techniqueConfig = RenderTechnique::createDefaultRenderTechnique(),
        const std::shared_ptr<Material>& material = nullptr );

    /// Update the OpenGL buffers from the Object data.
    // FIXME(Charly): Remove this
    void updateGL();

    //
    // Getters and setters.
    //

    /// Return the name of the RenderObject.
    const std::string& getName() const;

    /// Return the Component the RenderObject belongs to.
    const Component* getComponent() const;

    /// Return the Component the RenderObject belongs to.
    Component* getComponent();

    /// Return the type of the rendered object.
    const RenderObjectType& getType() const;

    /// Set the type of the rendered object.
    void setType( const RenderObjectType& t );

    /// Toggle on/off rendering the object.
    void setVisible( bool visible );

    /// Toggle on/off rendering the object.
    void toggleVisible();

    /// Return true if the object is visible.
    bool isVisible() const;

    /// Toggle on/off picking on the object.
    void setPickable( bool pickable );

    /// Toggle on/off picking on the object.
    void togglePickable();

    /// Return true if picking is active on the object.
    bool isPickable() const;

    /// Toggle on/off xray rendering of the object (always visible, even if behind).
    void setXRay( bool xray );

    /// Toggle on/off xray rendering of the object (always visible, even if behind).
    void toggleXRay();

    /// Return true if xray rendering is active for the object..
    bool isXRay() const;

    /// Toggle on/off transparency rendering for the object.
    /// \note Effective only for objects with non-1 alpha channel.
    void setTransparent( bool transparent );

    /// Toggle on/off transparency rendering for the object.
    /// \note Effective only for objects with non-1 alpha channel.
    void toggleTransparent();

    /// Return true if transparency rendering is active for the object.
    bool isTransparent() const;

    /// Return true if the object data need to be uploaded to the GPU.
    bool isDirty() const;

    /// Set the RenderTechnique for the object.
    void setRenderTechnique( const std::shared_ptr<RenderTechnique>& technique );

    /// Return the RenderTechnique for the object.
    std::shared_ptr<const RenderTechnique> getRenderTechnique() const;

    /// Return the RenderTechnique for the object.
    std::shared_ptr<RenderTechnique> getRenderTechnique();

    /// Set the object's display Mesh.
    void setMesh( const std::shared_ptr<Mesh>& mesh );

    /// Return the object's display Mesh.
    std::shared_ptr<const Mesh> getMesh() const;

    /// Return the object's display Mesh.
    const std::shared_ptr<Mesh>& getMesh();

    /// Return the object's transformation in world space.
    Core::Transform getTransform() const;

    /// Return the object's transformation in world space.
    Core::Matrix4 getTransformAsMatrix() const;

    /// Return the object's AABB.
    /// \note Takes the object's transform into account.
    Core::Aabb getAabb() const;

    /// Return the object's display mesh AABB.
    Core::Aabb getMeshAabb() const;

    /// Set the object's tranformation in object space (relative to the Entity).
    void setLocalTransform( const Core::Transform& transform );

    /// Set the object's tranformation in object space (relative to the Entity).
    void setLocalTransform( const Core::Matrix4& transform );

    /// Return the object's tranformation in object space (relative to the Entity).
    const Core::Transform& getLocalTransform() const;

    /// Return the object's tranformation in object space (relative to the Entity).
    const Core::Matrix4& getLocalTransformAsMatrix() const;

    /// Basically just decreases lifetime counter.
    /// If it goes to zero, then calls hasExpired().
    /// Does nothing if lifetime is set to -1.
    void hasBeenRenderedOnce();

    /// Notify the manager that the RenderObject needs to be deleted.
    void hasExpired();

    /// Render the object for the given Light parameters, RenderData and Shader.
    // FIXME (Mathias) Find why this was added for shader and material refactor ...
    virtual void render( const RenderParameters& lightParams, const RenderData& rdata,
                         const ShaderProgram* shader );

    /// Render the object for the given Light parameters, RenderData and render pass.
    virtual void render( const RenderParameters& lightParams, const RenderData& rdata,
                         RenderTechnique::PassName passname = RenderTechnique::LIGHTING_OPAQUE );

  private:
    /// The object's tranformation in object space (relative to the Entity).
    Core::Transform m_localTransform;

    /// The Component the RenderObject belongs to.
    Component* m_component;

    /// The name of the RenderObject.
    std::string m_name;

    /// The type of the rendered object.
    RenderObjectType m_type;

    /// The RenderTechnique for the object.
    std::shared_ptr<RenderTechnique> m_renderTechnique;

    /// The display mesh for the object.
    std::shared_ptr<Mesh> m_mesh;

    /// Guard the upload of the object's data on the GPU against thread concurrency.
    mutable std::mutex m_updateMutex;

    /// The lifetim of the RenderObject.
    int m_lifetime;

    /// Whether rendering is active for the object.
    bool m_visible;
    /// Whether picking is active for the object.
    bool m_pickable;
    /// Whether xray rendering is active for the object.
    bool m_xray;
    /// Whether transparent rendering is active for the object.
    bool m_transparent;
    /// Whether the object's data need to be uploaded to the GPU.
    bool m_dirty;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDEROBJECT_HPP
