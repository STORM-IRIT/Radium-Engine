#include <Dataflow/Rendering/Nodes/Sinks/DisplaySinkNode.hpp>

#define MAX_DISPLAY_INPUTS 8

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

DisplaySinkNode::DisplaySinkNode( const std::string& name ) : Node( name, getTypename() ) {
    auto beautyTex = new PortIn<TextureType>( "Beauty", this );
    addInput( beautyTex );
    beautyTex->attachMember( this, &DisplaySinkNode::observeConnection );
    for ( size_t i = 0; i < DisplaySinkNode::MaxImages; i++ ) {
        auto portIn = new PortIn<TextureType>( "AOV_" + std::to_string( i ), this );
        addInput( portIn );
        portIn->attachMember( this, &DisplaySinkNode::observeConnection );
    }
    m_textures.resize( DisplaySinkNode::MaxImages );
}

DisplaySinkNode::~DisplaySinkNode() {
    detachAll();
}

void DisplaySinkNode::execute() {
    // TODO verify the robustness of this (address of port data stored in a vector ....)
    if ( m_firstRun ) {
        m_firstRun = false;
        for ( size_t i = 0; i <= MAX_DISPLAY_INPUTS; i++ ) {
            if ( m_inputs[i]->isLinked() ) {
                auto input     = static_cast<PortIn<TextureType>*>( m_inputs[i].get() );
                m_textures[i]  = &( input->getData() );
                auto interface = static_cast<PortOut<TextureType>*>( m_interface[i] );
                interface->setData( m_textures[i] );
            }
            else {
                m_textures[i] = nullptr;
            }
        }
        // not sure DisplaySink should be observable
        this->notify( m_textures );
    }
}

const std::vector<TextureType*>& DisplaySinkNode::getTextures() {
    return m_textures;
}

void DisplaySinkNode::observeConnection(
    const std::string& name,
    // Will be used for efficient management of connection/de-connection
    const PortIn<TextureType>& /* port */,
    bool connected ) {
    // deffer the port management to DisplaySinkNode::execute()
    m_firstRun = true;
}

const std::vector<PortBase*>& DisplaySinkNode::buildInterfaces( Node* parent ) {
    m_interface.clear();
    m_interface.shrink_to_fit();
    m_interface.reserve( 1 );
    auto interfacePort = new PortOut<std::vector<TextureType*>>( "Beauty+AOV", parent );
    m_interface.push_back( interfacePort );
    return m_interface;
}
} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
