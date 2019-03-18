#ifndef RADIUMENGINE_RENDEROBJECT_HPP
#define RADIUMENGINE_RENDEROBJECT_HPP

#include <Engine/RaEngine.hpp>

#include <memory>
#include <mutex>
#include <string>

#include <Core/Math/Types.hpp>
#include <Core/Utils/IndexedObject.hpp>

#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

namespace Ra {
namespace Engine {
class Component;
class Mesh;
class Material;
class RenderParameters;
struct ViewingParameters;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

/**
 * The RenderObject class stores all needed data to render an object (Mesh,
 * Material, transformation...).
 */
// Radium V2 : need a bit of cleanup !
class RA_ENGINE_API RenderObject final : public Core::Utils::IndexedObject {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /**
     * Create a RenderObject with the given name, type and which belongs to the
     * given Component.
     * A -1 lifetime (or any other negative value) is considered infinite,
     * 0 is an "invalid value" (would mean the render object has to die immediatly),
     * any other positive value will be taken into account.
     */
    RenderObject( const std::string& name, Component* comp, const RenderObjectType& type,
                  int lifetime = -1 );

    ~RenderObject() override;

    // clang-format off
    /**
     * Sort of factory method to easily create a render object.
     * Use case example :
     * \code
     *     std::string name = "MyRO";
     *     RenderObjectType type = RenderObjectType::Geometry;
     *     // Retrieve the component the way you want.
     *     // Since this method will often be used in a component, just use `this` pointer.
     *     Component* comp;
     *     // Retrieve an already created configuration, or create one (see ShaderConfiguration docs).
     *     ShaderConfiguration config = ShaderConfigurationFactory::getConfiguration("MyConfig");
     *     // Then configure your material
     *     Material* mat = new Material;
     *     ...
     *     // createRenderObject can finally be called.
     *     RenderObject* ro = createRenderObject(name, component, type, config, material);
     * \endcode
     * @param name the name for the RenderObject.
     * @param comp the Component the RenderObject will belong to.
     * @param type the type of RenderObject.
     * @param mesh the RenderObject's mesh to render.
     * @param techniqueConfig the RenderTechnique to use when rendering the mesh.
     * @param material the Material to use when rendering the mesh.
     * @return the created RenderObject.
     */
    // clang-format on
    // TODO: Update the above documentation to match the new profile and use case ...
    static RenderObject* createRenderObject(
        const std::string& name, Component* comp, const RenderObjectType& type,
        const std::shared_ptr<Mesh>& mesh,
        const RenderTechnique& techniqueConfig = RenderTechnique::createDefaultRenderTechnique(),
        const std::shared_ptr<Material>& material = nullptr );

    /// \name Rendering
    /// \{

    /**
     * Render the object with the rendering environment defined by the given
     * RenderParameters, ViewingParameters and the shader.
     * \param lightParams lighting parameters for this rendering.
     * \param viewParams  viewing parameters for this rendering.
     * \param shader shader to use for this rendering.
     */
    virtual void render( const RenderParameters& lightParams, const ViewingParameters& viewParams,
                         const ShaderProgram* shader );

    /**
     * Render the object with the rendering environment defined by the given
     * RenderParameters, ViewingParameters and RenderTechnique's PassName.
     * \param lightParams lighting parameters for this rendering.
     * \param viewParams viewing parameters for this rendering.
     * \param passname RenderTechnique pass name.
     */
    virtual void render( const RenderParameters& lightParams, const ViewingParameters& viewParams,
                         RenderTechnique::PassName passname = RenderTechnique::LIGHTING_OPAQUE );

    /**
     * Updates all the OpenGL state of the object.
     */
    void updateGL();

    /**
     * Return true if the object data need to be uploaded to the GPU, false otherwise.
     */
    bool isDirty() const;

    /**
     * Basically just decreases lifetime counter.
     * If it goes to zero, then notifies the RenderObjetManager that it needs to be deleted.
     * Does nothing if lifetime is set to -1 (or any other negative value).
     */
    void hasBeenRenderedOnce();

    /**
     * Notify the Component the RenderObject has expired.
     */
    void hasExpired();
    /// \}

    /**
     * Return the name of the RenderObject.
     */
    const std::string& getName() const;

    /**
     * Return the Component the RenderObject belongs to.
     */
    const Component* getComponent() const;

    /**
     * Return the Component the RenderObject belongs to.
     */
    Component* getComponent();

    /**
     * Return the type of the rendered object.
     */
    const RenderObjectType& getType() const;

    /**
     * Set the type of the rendered object.
     */
    void setType( const RenderObjectType& t );

    /**
     * Set the RenderTechnique for the object.
     */
    void setRenderTechnique( const std::shared_ptr<RenderTechnique>& technique );

    /**
     * Return the RenderTechnique for the object.
     */
    std::shared_ptr<const RenderTechnique> getRenderTechnique() const;

    /**
     * Return the RenderTechnique for the object.
     */
    std::shared_ptr<RenderTechnique> getRenderTechnique();

    /**
     * Set the object's display Mesh.
     */
    void setMesh( const std::shared_ptr<Mesh>& mesh );

    /**
     * Return the object's display Mesh.
     */
    std::shared_ptr<const Mesh> getMesh() const;

    /**
     * Return the object's display Mesh.
     */
    const std::shared_ptr<Mesh>& getMesh();

    /**
     * Return the RenderObject's AABB.
     * \note Takes the RenderObject's transform into account.
     */
    Core::Aabb getAabb() const;

    /// \name Transform
    /// \{

    /**
     * Set the object's tranformation in object space (relative to the Entity).
     */
    void setLocalTransform( const Core::Transform& transform );

    /**
     * Set the object's tranformation in object space (relative to the Entity).
     */
    void setLocalTransform( const Core::Matrix4& transform );

    /**
     * Return the object's tranformation in object space (relative to the Entity).
     */
    const Core::Transform& getLocalTransform() const;

    /**
     * Return the object's tranformation in object space (relative to the Entity).
     */
    const Core::Matrix4& getLocalTransformAsMatrix() const;

    /**
     * Return the object's transformation in world space.
     */
    Core::Transform getTransform() const;

    /**
     * Return the object's transformation in world space.
     */
    Core::Matrix4 getTransformAsMatrix() const;
    /// \}

    /// \name Rendering options
    /// \{

    /**
     * Toggle on/off rendering the object.
     */
    void setVisible( bool visible );

    /**
     * Toggle on/off rendering the object.
     */
    void toggleVisible();

    /**
     * Return true if the object is visible.
     */
    bool isVisible() const;

    /**
     * Toggle on/off picking on the object.
     */
    void setPickable( bool pickable );

    /**
     * Toggle on/off picking on the object.
     */
    void togglePickable();

    /**
     * Return true if picking is active on the object.
     */
    bool isPickable() const;

    /**
     * Toggle on/off xray rendering of the object (always visible, even if behind).
     */
    void setXRay( bool xray );

    /**
     * Toggle on/off xray rendering of the object (always visible, even if behind).
     */
    void toggleXRay();

    /**
     * Return true if xray rendering is active for the object.
     */
    bool isXRay() const;

    /**
     * Toggle on/off transparency rendering for the object.
     * \note Effective only for objects with non-1 alpha channel.
     */
    void setTransparent( bool transparent );

    /**
     * Toggle on/off transparency rendering for the object.
     * \note Effective only for objects with non-1 alpha channel.
     */
    void toggleTransparent();

    /**
     * Return true if transparency rendering is active for the object.
     */
    bool isTransparent() const;
    /// \}

  private:
    /// The object's tranformation in object space (relative to the Entity).
    Core::Transform m_localTransform{Core::Transform::Identity()};

    /// The Component the RenderObject belongs to.
    Component* m_component{nullptr};

    /// The name of the RenderObject.
    std::string m_name{};

    /// The type of the rendered object.
    RenderObjectType m_type{RenderObjectType::Geometry};

    /// The RenderTechnique for the object.
    std::shared_ptr<RenderTechnique> m_renderTechnique{nullptr};

    /// The display mesh for the object.
    std::shared_ptr<Mesh> m_mesh;

    /// Guard the upload of the object's data on the GPU against thread concurrency.
    mutable std::mutex m_updateMutex;

    /// The lifetime of the RenderObject.
    int m_lifetime{-1};

    /// Whether the RenderObject has a finite lifetime.
    bool m_hasLifetime{false};

    bool m_visible{true};      ///< Whether rendering is active for the object.
    bool m_pickable{true};     ///< Whether picking is active for the object.
    bool m_xray{false};        ///< Whether xray rendering is active for the object.
    bool m_transparent{false}; ///< Whether transparent rendering is active for the object.
    bool m_dirty{true};        ///< Whether the object's data need to be uploaded to the GPU.
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDEROBJECT_HPP
