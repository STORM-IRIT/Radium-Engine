#ifndef RADIUMENGINE_RENDEROBJECT_HPP
#define RADIUMENGINE_RENDEROBJECT_HPP

#include <Engine/RaEngine.hpp>

#include <memory>
#include <mutex>
#include <string>

#include <Core/Types.hpp>
#include <Core/Utils/IndexedObject.hpp>

#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

namespace Ra {
namespace Engine {
class Component;
class Displayable;
class Material;
class RenderParameters;
struct ViewingParameters;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

/**
 * Class to manage renderable objects.
 * This class associate all the informations needed to draw an object in OpenGL.
 */
class RA_ENGINE_API RenderObject final : public Core::Utils::IndexedObject
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /**
     * Construct a renderObject
     *
     * @param name the name of the renderObject
     * @param comp The component that holds the RenderObject
     * @param type The type (ui, debug, geometry) of the render object
     * @param lifetime A -1 (or any other negative value) lifetime is considered infinite,
     *        0 is an "invalid value" (would mean the render object has to die immediatly),
     *        hence it's considered as infinite,
     *        any other positive value will be taken into account.
     */
    RenderObject( const std::string& name,
                  Component* comp,
                  const RenderObjectType& type,
                  int lifetime = -1 );

    ~RenderObject() override;

    /**
     * Sort of factory method to easily create a render object.
     * Use case example :
     *     std::string name = "MyRO";
     *     Component* comp;    // Retrieve the component the way you want.
     *                         // Since this method will often be used in a component,
     *                         // just use this pointer.
     *     RenderObjectType type = RenderObjectType::Geometry; // For example
     *     // Retrieve an already created configuration, or create one (see ShaderConfiguration
     *     docs). ShaderConfiguration config =
     *     ShaderConfigurationFactory::getConfiguration("MyConfig");
     *     Material* mat = new Material;
     *     // Then configure your material...
     *     // Configure your render technique
     *     RenderTechnique rt;
     *
     *     // createRenderObject can finally be called.
     *     RenderObject* ro = createRenderObject(name, component, type, config, material);
     * @param name
     * @param comp
     * @param type
     * @param mesh
     * @param techniqueConfig
     * @param material
     * @return
     */
    static RenderObject* createRenderObject(
        const std::string& name,
        Component* comp,
        const RenderObjectType& type,
        const std::shared_ptr<Displayable>& mesh,
        const RenderTechnique& techniqueConfig = RenderTechnique::createDefaultRenderTechnique() );

    /**
     * Updates all the openGL state of the object. Called at the beginning of each frame
     */
    void updateGL();

    //
    // Getters and setters.
    //
    const std::string& getName() const;
    const Component* getComponent() const;
    Component* getComponent();

    const RenderObjectType& getType() const;
    void setType( const RenderObjectType& t );

    void setVisible( bool visible );
    void toggleVisible();
    bool isVisible() const;

    void setPickable( bool pickable );
    void togglePickable();
    bool isPickable() const;

    void setXRay( bool xray );
    void toggleXRay();
    bool isXRay() const;

    void setTransparent( bool transparent );
    void toggleTransparent();
    bool isTransparent() const;

    bool isDirty() const;

    void setRenderTechnique( const std::shared_ptr<RenderTechnique>& technique );
    std::shared_ptr<const RenderTechnique> getRenderTechnique() const;
    std::shared_ptr<RenderTechnique> getRenderTechnique();

    void setMaterial( const std::shared_ptr<Material>& material );
    std::shared_ptr<const Material> getMaterial() const;
    std::shared_ptr<Material> getMaterial();

    void setMesh( const std::shared_ptr<Displayable>& mesh );
    std::shared_ptr<const Displayable> getMesh() const;
    const std::shared_ptr<Displayable>& getMesh();

    Core::Transform getTransform() const;
    Core::Matrix4 getTransformAsMatrix() const;

    Core::Aabb computeAabb() const;

    void setLocalTransform( const Core::Transform& transform );
    void setLocalTransform( const Core::Matrix4& transform );
    const Core::Transform& getLocalTransform() const;
    const Core::Matrix4& getLocalTransformAsMatrix() const;

    /// Basically just decreases lifetime counter.
    /// If it goes to zero, then render object notifies the manager that it needs to be deleted.
    /// Does nothing if lifetime is set to -1
    void hasBeenRenderedOnce();
    void hasExpired();

    /**
     * Render the object with the given rendering environment defined by the lighting parameters,
     * the viewing parameters and  the shader
     * @param lightParams lighting parameters for this rendering
     * @param viewParams  viewing parameters for this rendering
     * @param shader shader to use for this rendering
     */
    void render( const RenderParameters& lightParams,
                 const ViewingParameters& viewParams,
                 const ShaderProgram* shader,
                 const RenderParameters& shaderParams );

    /**
     * Render the object for the given rendering environment defined by the lighting parameters, the
     * viewing parameters and the RenderTechnique pass name
     * @param lightParams lighting parameters for this rendering
     * @param viewParams viewing parameters for this rendering
     * @param passname RenderTechnique pass name
     */
    void render( const RenderParameters& lightParams,
                 const ViewingParameters& viewParams,
                 Core::Utils::Index passId = DefaultRenderingPasses::LIGHTING_OPAQUE );

  private:
    Core::Transform m_localTransform{Core::Transform::Identity()};

    Component* m_component{nullptr};
    std::string m_name{};

    RenderObjectType m_type{RenderObjectType::Geometry};
    std::shared_ptr<RenderTechnique> m_renderTechnique{nullptr};
    std::shared_ptr<Displayable> m_mesh{nullptr};
    std::shared_ptr<Material> m_material{nullptr};

    mutable std::mutex m_updateMutex;

    int m_lifetime{-1};
    bool m_visible{true};
    bool m_pickable{true};
    bool m_xray{false};
    bool m_transparent{false};
    bool m_dirty{true};
    bool m_hasLifetime{false};
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDEROBJECT_HPP
