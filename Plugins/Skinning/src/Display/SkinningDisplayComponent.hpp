#ifndef SKINPLUGIN_DISPLAY_COMPONENT_HPP
#define SKINPLUGIN_DISPLAY_COMPONENT_HPP

#include <SkinningPlugin.hpp>
#include <Engine/Component/Component.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Geometry/Adjacency/Adjacency.hpp>

#include <Core/Animation/Skinning/SkinningData.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Animation/Handle/HandleWeight.hpp>

#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

namespace SkinningPlugin {

class SKIN_PLUGIN_API SkinningDisplayComponent : public Ra::Engine::Component {
public:
    /// CONSTRUCTOR
    SkinningDisplayComponent( const std::string& name, const std::string& content ) : Ra::Engine::Component( name ), m_contentsName( content ) { }

    /// DESTRUCTOR
    ~SkinningDisplayComponent() { }

    /// INTERFACE
    void initialize() {
        display();
    }

    void display() {
        Ra::Core::TriangleMesh mesh;
        bool hasMesh = Ra::Engine::ComponentMessenger::getInstance()->get(getEntity(), m_contentsName, mesh);

        Ra::Core::Animation::WeightMatrix weights;
        bool hasWeights = Ra::Engine::ComponentMessenger::getInstance()->get(getEntity(), m_contentsName, weights);

        if( hasMesh && hasWeights ) {
            const uint size = mesh.m_vertices.size();

            const uint   fiveColor = 5;
            const Scalar magenta   = 5.0f/ 6.0f;
            Ra::Core::Vector4Array palette( fiveColor );
            for( uint i = 0; i < fiveColor; ++i ) {
                Scalar hue = ( Scalar( i ) / Scalar( fiveColor - 1 ) ) * magenta;
                palette[i] = Ra::Core::Colors::fromHSV( hue, 1.0, 0.5 );
            }

            std::vector< uint > partition( size );
            #pragma omp parallel for
            for( uint i = 0; i < size; ++i ) {
                uint ID;
                Ra::Core::VectorN row = weights.row( i );
                row.maxCoeff( &ID );
                partition[i] = ID;
            }

            Ra::Core::Geometry::AdjacencyMatrix Adj = Ra::Core::Geometry::uniformAdjacency( mesh.m_vertices, mesh.m_triangles );
            Ra::Core::Geometry::AdjacencyMatrix Seg( weights.cols(), weights.cols() );

            for( int k = 0; k < Adj.outerSize(); ++k ) {
                for( Ra::Core::Geometry::AdjacencyMatrix::InnerIterator it( Adj, k ); it; ++it ) {
                    const uint i = it.row();
                    const uint j = it.col();
                    if( partition[i] != partition[j] ) {
                        Seg.coeffRef( partition[i], partition[j] ) = 1.0;
                    }
                }
            }

            std::vector< uint > assignedColor( weights.cols(), uint( -1 ) );
            for( int k = 0; k < Seg.outerSize(); ++k ) {
                std::set< uint > option;
                for( uint i = 0; i < fiveColor; ++i ) {
                    option.insert( i );
                }

                for( Ra::Core::Geometry::AdjacencyMatrix::InnerIterator it( Seg, k ); it; ++it ) {
                    const uint j = it.row();
                    if( assignedColor[j] != uint( -1 ) ) {
                        option.erase( assignedColor[j] );
                    }
                }

                uint random = std::rand() % std::max<uint>(option.size(),1);
                auto it = option.begin();
                for( uint i = 0; i < random; ++i ) {
                    ++it;
                }
                assignedColor[k] = *it;
            }

            Ra::Core::Vector4Array color( size, Ra::Core::Vector4::Zero() );
            #pragma omp parallel for
            for( uint i = 0; i < size; ++i ) {
                color[i] = palette[ assignedColor[ partition[i] ] ];
            }

            Ra::Engine::RenderTechnique* technique = new Ra::Engine::RenderTechnique;
            technique->material = new Ra::Engine::Material( std::string( "Partition" ) + m_name );
            technique->material->setKd( Ra::Core::Vector4::Zero() );
            technique->material->setKs( Ra::Core::Vector4::Zero() );
            technique->material->setNs( 100 );
            technique->shaderConfig = Ra::Engine::ShaderConfigurationFactory::getConfiguration("Plain");


            std::string name = m_name + "_Partition";

            Ra::Engine::RenderObject* renderObject = new Ra::Engine::RenderObject( name, this, Ra::Engine::RenderObjectType::Fancy );
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

#endif //SKINPLUGIN_DISPLAY_COMPONENT_HPP
