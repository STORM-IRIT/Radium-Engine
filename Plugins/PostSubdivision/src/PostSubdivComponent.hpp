#ifndef POSTSUBDIV_PLUGIN_POSTSUBDIV_COMPONENT_HPP_
#define POSTSUBDIV_PLUGIN_POSTSUBDIV_COMPONENT_HPP_

#include <PostSubdivPluginMacros.hpp>

#include <Core/Algorithm/Subdivision/CatmullClarkSubdivider.hpp>
#include <Core/Algorithm/Subdivision/LoopSubdivider.hpp>
#include <Core/File/HandleData.hpp>
#include <Core/Index/Index.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

namespace Ra {
namespace Engine {
class RenderObject;
}
} // namespace Ra

namespace PostSubdivPlugin {

using Ra::Core::CatmullClarkSubdivider;
using Ra::Core::LoopSubdivider;
using Ra::Core::TopologicalMesh;
using Ra::Core::TriangleMesh;

/// The PostSubdivComponent class is responsible for applying post-process
/// subdivision on a mesh.
class POST_SUBDIV_PLUGIN_API PostSubdivComponent : public Ra::Engine::Component {
  public:
    /// The Subdivision Method.
    enum SubdivMethod {
        LOOP = 0,    ///< Loop subdivision
        CATMULLCLARK ///< CatmullClark subdivision
    };

    PostSubdivComponent( const std::string& name, SubdivMethod method, uint iter,
                         Ra::Engine::Entity* entity ) :
        Component( name, entity ),
        m_subdivMethod( method ),
        m_iter( iter ) {}

    virtual ~PostSubdivComponent();

    virtual void initialize() override;

    /// Set the id to get data from the ComponentMessenger.
    void setContentsName( const std::string& name ) { m_contentsName = name; }

    /// Apply the Subdivision on the mesh.
    void subdiv();

    /// Sets the subdivision method to use.
    void setSubdivMethod( SubdivMethod method ) { m_subdivMethod = method; }

    /// \returns the current subdivision method.
    inline SubdivMethod getSubdivMethod() const { return m_subdivMethod; }

    /// Sets the number of subdivision operations.
    void setSubdivIter( uint iter ) { m_iter = iter; }

    /// \returns the number of subdivision operations.
    uint getSubdivIter() const { return m_iter; }

  private:
    /// The Entity name for Component communication.
    std::string m_contentsName;

    /// The Subdivision Method.
    SubdivMethod m_subdivMethod;

    /// The number of subdivision iterations to perform.
    uint m_iter;

    /// Display RO.
    Ra::Engine::RenderObject* m_ro;

    /// Getter for the Geometry RO index.
    Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Index>::Getter m_roIdxGetter;

    /// The list of per-subdiv-iteration triplets of subdivider-topoMesh-TriangleMesh.
    std::map<uint, std::tuple<LoopSubdivider*, TopologicalMesh, TriangleMesh>> m_loopData;
    std::map<uint, std::tuple<CatmullClarkSubdivider*, TopologicalMesh, TriangleMesh>>
        m_catmullClarkData;
};

} // namespace PostSubdivPlugin

#endif //  POSTSUBDIV_PLUGIN_POSTSUBDIV_COMPONENT_HPP_
