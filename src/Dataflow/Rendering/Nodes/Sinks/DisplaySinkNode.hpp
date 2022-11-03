#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/Node.hpp>

#include <Dataflow/Rendering/Nodes/RenderingNode.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

using namespace Ra::Dataflow::Core;

class RA_DATAFLOW_API DisplaySinkNode
    : public Node,
      public Ra::Core::Utils::Observable<const std::vector<TextureType*>&>
{
  public:
    static constexpr int MaxImages = 9;

    explicit DisplaySinkNode( const std::string& name );
    ~DisplaySinkNode() override;

    bool execute() override;
    void init() override;

    const std::vector<PortBase*>& buildInterfaces( Node* parent ) override;

    static const std::string getTypename() { return "Display Sink"; }

    const std::vector<TextureType*>& getTextures();

  protected:
    void toJsonInternal( nlohmann::json& ) const override {}
    bool fromJsonInternal( const nlohmann::json& ) override { return true; }

  private:
    std::vector<TextureType*> m_textures;

    PortIn<TextureType>* m_beautyTex { new PortIn<TextureType>( "Beauty", this ) };

    // the observer method
    void
    observeConnection( const std::string& name, const PortIn<TextureType>& port, bool connected );

    bool m_firstRun { true };
};

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
