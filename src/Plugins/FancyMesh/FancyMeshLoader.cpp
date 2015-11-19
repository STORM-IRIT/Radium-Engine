#include <Plugins/FancyMesh/FancyMeshLoader.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Core/Log/Log.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/String/StringUtils.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Light/SpotLight.hpp>
#include <Engine/Renderer/Light/PointLight.hpp>
#include <iostream>
namespace FancyMeshPlugin
{
    namespace
    {
        const Ra::Engine::ShaderConfiguration defaultShaderConf(
            "BlinnPhongWireframe", "../Shaders",
            Ra::Engine::ShaderConfiguration::DEFAULT_SHADER_PROGRAM_W_GEOM );

        std::string filepath;
        static DataVector dataVector;

        void assimpToCore( const aiVector3D& inVector, Ra::Core::Vector3& outVector );
        void assimpToCore( const aiColor4D& inColor, Ra::Core::Color& outColor );
        void assimpToCore( const aiMatrix4x4& inMatrix, Ra::Core::Matrix4& outMatrix );

        Ra::Core::Vector3 assimpToCore( const aiVector3D&  vector );
        Ra::Core::Color   assimpToCore( const aiColor4D&   color );
        Ra::Core::Matrix4 assimpToCore( const aiMatrix4x4& matrix );

        void runThroughNodes( const aiNode* node, const aiScene* scene,
                              const Ra::Core::Matrix4& transform );

        void loadMesh(const aiMesh* mesh, FancyMeshData& data , const Ra::Core::Transform &tranform);

        void loadRenderTechnique( const aiMaterial* mat, FancyComponentData& data );
        void loadDefaultRenderTechnique( FancyComponentData& data );
    }

    DataVector FancyMeshLoader::loadFile( const std::string& name )
    {
        dataVector.clear();

        Assimp::Importer importer;
//        const aiScene* scene = importer.ReadFile( name, aiProcess_JoinIdenticalVertices);
        const aiScene* scene = importer.ReadFile( name,
                                                  aiProcess_Triangulate |
                                                  aiProcess_JoinIdenticalVertices |
                                                  aiProcess_GenSmoothNormals |
                                                  aiProcess_SortByPType |
                                                  aiProcess_FixInfacingNormals |
                                                  aiProcess_CalcTangentSpace |
                                                  aiProcess_GenUVCoords );
        if ( !scene )
        {
            LOG( logERROR ) << "Error while loading file \"" << name << "\" : " << importer.GetErrorString() << ".";
            return dataVector;
        }

        //    LOG(DEBUG) << "About to load file " << name;
        //    LOG(DEBUG) << "Found " << scene->mNumMeshes << " meshes and " << scene->mNumMaterials << " materials.";

        filepath = Ra::Core::StringUtils::getDirName( name );

        runThroughNodes( scene->mRootNode, scene, Ra::Core::Matrix4::Identity() );

        //LOG(DEBUG) << "File " << name << " loaded successfully (" << dataVector.size() << " items to load).";

        return dataVector;
    }

    namespace
    {

        void runThroughNodes( const aiNode* node, const aiScene* scene,
                              const Ra::Core::Matrix4& transform )
        {
            if ( node->mNumChildren == 0 && node->mNumMeshes == 0 )
            {
                return;
            }

            Ra::Core::Matrix4 matrix = transform * assimpToCore( node->mTransformation );

            if ( node->mNumMeshes > 0 )
            {
                FancyComponentData data;
                data.transform = matrix;
                data.name = node->mName.C_Str();

                // Consider only the first material for a given component
                if ( scene->HasMaterials() )
                {
                    aiMaterial* material = scene->mMaterials[scene->mMeshes[node->mMeshes[0]]->mMaterialIndex];
                    loadRenderTechnique( material, data );
                }
                else
                {
                    loadDefaultRenderTechnique( data );
                }

                for ( uint i = 0; i < node->mNumMeshes; ++i )
                {
                    FancyMeshData meshData;

                    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                    Ra::Core::Transform transform(data.transform);
                    loadMesh( mesh, meshData, transform);

                    data.mesh = meshData;
                }

                dataVector.push_back( data );
            }

            for ( uint i = 0; i < node->mNumChildren; ++i )
            {
                runThroughNodes( node->mChildren[i], scene, matrix );
            }
        }

        void loadMesh( const aiMesh* mesh, FancyMeshData& data, const Ra::Core::Transform& transform )
        {
            Ra::Core::Vector4Array tangents;
            Ra::Core::Vector4Array bitangents;
            Ra::Core::Vector4Array texcoords;
            Ra::Core::Vector4Array colors;
            Ra::Core::Vector4Array weights;
            std::vector<uint>  indices;

            Ra::Core::TriangleMesh triangleMesh; // Used to recompute normals

            for ( uint i = 0; i < mesh->mNumVertices; ++i )
            {
                //positions.push_back( assimpToCore( mesh->mVertices[i] ) );
                //normals.push_back( assimpToCore( mesh->mNormals[i] ) );
                triangleMesh.m_vertices.push_back( transform * assimpToCore( mesh->mVertices[i] ) );

                if ( mesh->HasTangentsAndBitangents() )
                {
                    Ra::Core::Vector4 tangent( 0, 0, 0, 0 );
                    Ra::Core::Vector4 bitangent( 0, 0, 0, 0 );

                    tangent.head<3>() = assimpToCore( mesh->mTangents[i] );
                    bitangent.head<3>() = assimpToCore( mesh->mBitangents[i] );

                    tangents.push_back( tangent );
                    bitangents.push_back( bitangent );
                }

                // FIXME(Charly): What do texture coords indices mean ?
                if ( mesh->HasTextureCoords( 0 ) )
                {
                    Ra::Core::Vector4 texcoord( 0, 0, 0, 0 );
                    texcoord.head<3>() = assimpToCore( mesh->mTextureCoords[0][i] );
                    texcoords.push_back( texcoord );
                }

                if ( mesh->HasVertexColors( 0 ) )
                {
                    colors.push_back( assimpToCore( mesh->mColors[0][i] ) );
                }
            }

            for ( uint i = 0; i < mesh->mNumFaces; ++i )
            {
                aiFace f = mesh->mFaces[i];
                triangleMesh.m_triangles.push_back( Ra::Core::Triangle(f.mIndices[0], f.mIndices[1], f.mIndices[2]) );
            }

            Ra::Core::MeshUtils::removeDuplicates(triangleMesh, data.vertexMap);

            for (int i = 0; i < triangleMesh.m_triangles.size(); i++)
            {
                indices.push_back(triangleMesh.m_triangles[i](0));
                indices.push_back(triangleMesh.m_triangles[i](1));
                indices.push_back(triangleMesh.m_triangles[i](2));
            }

            Ra::Core::Vector3Array vec3Normals;

            Ra::Core::MeshUtils::getAutoNormals( triangleMesh, vec3Normals );

            Ra::Core::Vector4Array vertices;
            Ra::Core::Vector4Array normals;
            const uint vertCount = triangleMesh.m_vertices.size();
            vertices.reserve( vertCount );
            normals.reserve( vertCount );

            for ( uint i = 0; i < vertCount; ++i )
            {
                Ra::Core::Vector4 vertex( 0, 0, 0, 1 );
                vertex.head<3>() = triangleMesh.m_vertices[i];
                Ra::Core::Vector4 normal( 0, 0, 0, 0 );
                normal.head<3>() = vec3Normals[i];

                vertices.push_back( vertex );
                normals.push_back( normal );
            }

            data.positions = vertices;
            data.normals = normals;
            data.tangents = tangents;
            data.bitangents = bitangents;
            data.texcoords = texcoords;
            data.indices = indices;
        }

        void loadRenderTechnique( const aiMaterial* mat, FancyComponentData& data )
        {
            std::string materialName = data.name.append( "_Material" );
            if ( mat == nullptr )
            {
                loadDefaultRenderTechnique( data );
                return;
            }

            // TODO(Charly): Handle different shader programs
            // TODO(Charly): Handle transparency
            Ra::Engine::Material* material = new Ra::Engine::Material( materialName );

            aiColor4D color;
            if ( AI_SUCCESS == mat->Get( AI_MATKEY_COLOR_DIFFUSE, color ) )
            {
                Ra::Core::Color c = assimpToCore( color );
                material->setKd( c );
                if ( c.w() < 1.0 )
                {
                    material->setMaterialType( Ra::Engine::Material::MaterialType::MAT_TRANSPARENT );
                }
            }
            else
            {
                material->setKd( Ra::Core::Color( 0, 0, 0, 1 ) );
            }

            if ( AI_SUCCESS == mat->Get( AI_MATKEY_COLOR_SPECULAR, color ) )
            {
                material->setKs( assimpToCore( color ) );
            }
            else
            {
                material->setKs( Ra::Core::Color( 0, 0, 0, 1 ) );
            }

            Scalar shininess;
            if ( AI_SUCCESS == mat->Get( AI_MATKEY_SHININESS, shininess ) )
            {
                material->setNs( shininess );
            }
            else
            {
                material->setKs( Ra::Core::Color( 0, 0, 0, 1 ) );
                material->setNs( 1.0 );
            }

            aiString name;
            if ( AI_SUCCESS == mat->Get( AI_MATKEY_TEXTURE( aiTextureType_DIFFUSE, 0 ), name ) )
            {
                material->addTexture( Ra::Engine::Material::TextureType::TEX_DIFFUSE,
                                      filepath + "/" + std::string( name.C_Str() ) );
            }

            if ( AI_SUCCESS == mat->Get( AI_MATKEY_TEXTURE( aiTextureType_SPECULAR, 0 ), name ) )
            {
                material->addTexture( Ra::Engine::Material::TextureType::TEX_SPECULAR,
                                      filepath + "/" + std::string( name.C_Str() ) );
            }

            if ( AI_SUCCESS == mat->Get( AI_MATKEY_TEXTURE( aiTextureType_NORMALS, 0 ), name ) )
            {
                material->addTexture( Ra::Engine::Material::TextureType::TEX_NORMAL,
                                      filepath + "/" + std::string( name.C_Str() ) );
            }

            if ( AI_SUCCESS == mat->Get( AI_MATKEY_TEXTURE( aiTextureType_SHININESS, 0 ), name ) )
            {
                material->addTexture( Ra::Engine::Material::TextureType::TEX_SHININESS,
                                      filepath + "/" + std::string( name.C_Str() ) );
            }

            if ( AI_SUCCESS == mat->Get( AI_MATKEY_TEXTURE( aiTextureType_OPACITY, 0 ), name ) )
            {
                material->addTexture( Ra::Engine::Material::TextureType::TEX_ALPHA,
                                      filepath + "/" + std::string( name.C_Str() ) );
                material->setMaterialType( Ra::Engine::Material::MaterialType::MAT_TRANSPARENT );
            }

            Ra::Engine::RenderTechnique* renderTechnique = new Ra::Engine::RenderTechnique;
            renderTechnique->shaderConfig = defaultShaderConf;
            renderTechnique->material = material;

            data.renderTechnique = renderTechnique;
        }

        void loadDefaultRenderTechnique( FancyComponentData& data )
        {
            std::string materialName = data.name.append( "_Material" );

            Ra::Engine::Material* material = new Ra::Engine::Material( materialName );

            Ra::Engine::RenderTechnique* renderTechnique = new Ra::Engine::RenderTechnique;
            renderTechnique->shaderConfig = defaultShaderConf;
            renderTechnique->material = material;

            data.renderTechnique = renderTechnique;
        }

        void assimpToCore( const aiVector3D& inVector, Ra::Core::Vector3& outVector )
        {
            for ( uint i = 0; i < 3; ++i )
            {
                outVector[i] = inVector[i];
            }
        }

        void assimpToCore( const aiColor4D& inColor, Ra::Core::Color& outColor )
        {
            for ( uint i = 0; i < 4; ++i )
            {
                outColor[i] = inColor[i];
            }
        }

        void assimpToCore( const aiMatrix4x4& inMatrix, Ra::Core::Matrix4& outMatrix )
        {
            for ( uint i = 0; i < 4; ++i )
            {
                for ( uint j = 0; j < 4; ++j )
                {
                    outMatrix( i, j ) = inMatrix[i][j];
                }
            }
        }

        Ra::Core::Vector3 assimpToCore( const aiVector3D& vector )
        {
            Ra::Core::Vector3 v;
            assimpToCore( vector, v );
            return v;
        }

        Ra::Core::Color assimpToCore( const aiColor4D& color )
        {
            Ra::Core::Color c;
            assimpToCore( color, c );
            return c;
        }

        Ra::Core::Matrix4 assimpToCore( const aiMatrix4x4& matrix )
        {
            Ra::Core::Matrix4 m;
            assimpToCore( matrix, m );
            return m;
        }

    } // namespace


} // namespace Ra
