#ifndef SKINPLUGIN_DISPLAY_COMPONENT_HPP
#define SKINPLUGIN_DISPLAY_COMPONENT_HPP

#include <Engine/Component/Component.hpp>
#include <SkinningPlugin.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/Adjacency/Adjacency.hpp>
#include <Core/Math/ColorPresets.hpp>

#include <Core/Animation/Handle/HandleWeight.hpp>
#include <Core/Animation/Skinning/SkinningData.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

using Ra::Core::TriangleMesh;
using Ra::Core::Animation::WeightMatrix;
using Ra::Engine::ComponentMessenger;

namespace SkinningPlugin {

class SKIN_PLUGIN_API SkinningDisplayComponent : public Ra::Engine::Component {
  public:
    /// CONSTRUCTOR
    SkinningDisplayComponent( const std::string& name, const std::string& content,
                              Ra::Engine::Entity* entity ) :
        Ra::Engine::Component( name, entity ),
        m_contentsName( content ) {}

    /// DESTRUCTOR
    ~SkinningDisplayComponent() {}

    /// INTERFACE
    void initialize() { display(); }

    void display() {
        auto compMsg = ComponentMessenger::getInstance();
        bool hasMesh = compMsg->canGet<TriangleMesh>( getEntity(), m_contentsName );
        bool hasWeights = compMsg->canGet<WeightMatrix>( getEntity(), m_contentsName );

        if ( hasMesh && hasWeights )
        {
            const TriangleMesh& mesh = compMsg->get<TriangleMesh>( getEntity(), m_contentsName );
            const WeightMatrix& weights = compMsg->get<WeightMatrix>( getEntity(), m_contentsName );

            const uint size = mesh.vertices().size();

            const uint fiveColor = 5;
            const Scalar magenta = 5.0f / 6.0f;
            Ra::Core::Vector4Array palette( fiveColor );
            for ( uint i = 0; i < fiveColor; ++i )
            {
                Scalar hue = ( Scalar( i ) / Scalar( fiveColor - 1 ) ) * magenta;
                palette[i] = Ra::Core::Colors::fromHSV( hue, 1.0, 0.5 );
            }

            std::vector<uint> partition( size );
#pragma omp parallel for
            for ( int i = 0; i < int( size ); ++i )
            {
                uint ID;
                Ra::Core::VectorN row = weights.row( i );
                row.maxCoeff( &ID );
                partition[i] = ID;
            }

            Ra::Core::Geometry::AdjacencyMatrix Adj =
                Ra::Core::Geometry::uniformAdjacency( mesh.vertices(), mesh.m_triangles );
            Ra::Core::Geometry::AdjacencyMatrix Seg( weights.cols(), weights.cols() );

            for ( int k = 0; k < Adj.outerSize(); ++k )
            {
                for ( Ra::Core::Geometry::AdjacencyMatrix::InnerIterator it( Adj, k ); it; ++it )
                {
                    const uint i = it.row();
                    const uint j = it.col();
                    if ( partition[i] != partition[j] )
                    {
                        Seg.coeffRef( partition[i], partition[j] ) = 1.0;
                    }
                }
            }

            std::vector<uint> assignedColor( weights.cols(), uint( -1 ) );
            for ( int k = 0; k < Seg.outerSize(); ++k )
            {
                std::set<uint> option;
                for ( uint i = 0; i < fiveColor; ++i )
                {
                    option.insert( i );
                }

                for ( Ra::Core::Geometry::AdjacencyMatrix::InnerIterator it( Seg, k ); it; ++it )
                {
                    const uint j = it.row();
                    if ( assignedColor[j] != uint( -1 ) && option.size() > 1 )
                    {
                        option.erase( assignedColor[j] );
                    }
                }

                uint random = std::rand() % std::max<uint>( option.size(), 1 );
                auto it = option.begin();
                for ( uint i = 0; i < random && it != option.end(); ++i )
                {
                    ++it;
                }
                assignedColor[k] = *it;
            }

            Ra::Core::Vector4Array color( size, Ra::Core::Vector4::Zero() );
#pragma omp parallel for
            for ( int i = 0; i < int( size ); ++i )
            {
                color[i] = palette[assignedColor[partition[i]]];
            }

            std::shared_ptr<Ra::Engine::RenderTechnique> technique(
                new Ra::Engine::RenderTechnique );

            Ra::Engine::BlinnPhongMaterial* nm =
                new Ra::Engine::BlinnPhongMaterial( std::string( "Partition" ) + m_name );
            nm->m_kd = Ra::Core::Vector4::Zero();
            nm->m_ks = Ra::Core::Vector4::Zero();
            nm->m_ns = 100;
            technique->resetMaterial( nm );

            technique->setConfiguration(
                Ra::Engine::ShaderConfigurationFactory::getConfiguration( "Plain" ) );

            std::string name = m_name + "_Partition";

            Ra::Engine::RenderObject* renderObject =
                new Ra::Engine::RenderObject( name, this, Ra::Engine::RenderObjectType::Fancy );
            renderObject->setVisible( true );
            renderObject->setRenderTechnique( technique );
            std::shared_ptr<Ra::Engine::Mesh> displayMesh( new Ra::Engine::Mesh( name ) );
            displayMesh->loadGeometry( mesh );
            displayMesh->addData( Ra::Engine::Mesh::Vec4Data::VERTEX_COLOR, color );
            renderObject->setMesh( displayMesh );

            renderObject->setVisible( false );

            addRenderObject( renderObject );
        }
    }

  protected:
    /// VARIABLE
    std::string m_contentsName;
};

} // namespace SkinningPlugin

#endif // SKINPLUGIN_DISPLAY_COMPONENT_HPP
