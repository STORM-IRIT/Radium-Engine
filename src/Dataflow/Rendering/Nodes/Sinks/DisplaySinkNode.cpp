#include <Dataflow/Rendering/Nodes/Sinks/DisplaySinkNode.hpp>

#define MAX_DISPLAY_INPUTS 8

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

DisplaySinkNode::DisplaySinkNode( const std::string& name ) : Node( name, getTypename() ) {
    addInput( m_beautyTex );
    m_beautyTex->attachMember( this, &DisplaySinkNode::observeConnection );
    for ( size_t i = 0; i < DisplaySinkNode::MaxImages; i++ ) {
        auto portIn = new PortIn<TextureType>( "AOV_" + std::to_string( i ), this );
        addInput( portIn );
        portIn->attachMember( this, &DisplaySinkNode::observeConnection );
    }
    m_textures.resize( DisplaySinkNode::MaxImages );
}
void DisplaySinkNode::init() {}

DisplaySinkNode::~DisplaySinkNode() {
    detachAll();
}

bool DisplaySinkNode::execute() {
    // TODO verify the robustness of this (address of port data stored in a vector ....)
    if ( m_firstRun ) {
        m_firstRun = false;
        bool gotData { false };
        for ( size_t i = 0; i < DisplaySinkNode::MaxImages; i++ ) {
            if ( m_inputs[i]->isLinked() ) {
                auto input    = static_cast<PortIn<TextureType>*>( m_inputs[i].get() );
                m_textures[i] = &( input->getData() );
                gotData       = true;
            }
            else {
                m_textures[i] = nullptr;
            }
        }
        auto interfacePort = static_cast<PortOut<std::vector<TextureType*>>*>( m_interface[0] );
        if ( gotData ) { interfacePort->setData( &m_textures ); }
        else {
            interfacePort->setData( nullptr );
        }
        // not sure DisplaySink should be observable
        this->notify( m_textures );
    }
    return true;
}

const std::vector<TextureType*>& DisplaySinkNode::getTextures() {
    return m_textures;
}

void DisplaySinkNode::observeConnection(
    const std::string& /*name*/,
    // Will be used for efficient management of connection/de-connection
    const PortIn<TextureType>& /* port */,
    bool /*connected*/ ) {
    // deffer the port management to DisplaySinkNode::execute()
    m_firstRun = true;
}

const std::vector<PortBase*>& DisplaySinkNode::buildInterfaces( Node* parent ) {
    m_interface.clear();
    m_interface.shrink_to_fit();
    m_interface.reserve( 1 );
    auto interfacePort = new PortOut<std::vector<TextureType*>>( "Beauty+AOV", parent );
    m_interface.emplace_back( interfacePort );
    return m_interface;
}
} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
