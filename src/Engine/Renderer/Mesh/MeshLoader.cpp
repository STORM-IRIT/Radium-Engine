#include <Engine/Renderer/Mesh/MeshLoader.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Core/Math/Vector.hpp>
#include <Core/Math/Matrix.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/String/StringUtils.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/RenderSystem.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>
#include <Engine/Renderer/Shader/ShaderProgramManager.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Drawable/DrawableComponent.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Light/SpotLight.hpp>
#include <Engine/Renderer/Light/PointLight.hpp>

namespace Ra
{

namespace
{
const Engine::ShaderConfiguration blinnPhongShaderConfiguration("BlinnPhong", "../Shaders");

void assimpToCore(const aiVector3D& inVector, Core::Vector3& outVector);
void assimpToCore(const aiColor4D& inColor, Core::Color& outColor);
void assimpToCore(const aiMatrix4x4& inMatrix, Core::Matrix4& outMatrix);

Core::Vector3 assimpToCore(const aiVector3D&  vector);
Core::Color   assimpToCore(const aiColor4D&   color);
Core::Matrix4 assimpToCore(const aiMatrix4x4& matrix);

void runThroughNodes(const aiNode* node, const aiScene* scene,
                     const Core::Matrix4& transform, Engine::RadiumEngine* engine);

void loadMesh(const aiMesh* mesh, Engine::DrawableComponent* component, const std::string& name);

void loadMaterial(const aiMaterial* mat, Engine::DrawableComponent* component, const std::string& name);
void loadDefaultMaterial(Engine::DrawableComponent* component, const std::string& name);

void loadLights(const aiScene* scene, Engine::RadiumEngine* engine);
}

bool Engine::MeshLoader::loadFile(const std::string& name, RadiumEngine* engine)
{
    fprintf(stderr, "Loading file \"%s\"...\n", name.c_str());

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(name,
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_GenSmoothNormals |
                                             aiProcess_SortByPType |
                                             aiProcess_FixInfacingNormals |
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_GenUVCoords);
    if (!scene)
    {
        fprintf(stderr, "Error while loading file \"%s\" :\n\t%s.\n",
                 name.c_str(), importer.GetErrorString());
        return false;
    }

    runThroughNodes(scene->mRootNode, scene, Core::Matrix4::Identity(), engine);

    if (scene->HasLights())
    {
        loadLights(scene, engine);
    }

    return true;
}

namespace
{

void runThroughNodes(const aiNode* node, const aiScene* scene,
                     const Core::Matrix4& transform, Engine::RadiumEngine* engine)
{
    static int currentMesh = 0;

    if (node->mNumChildren == 0 && node->mNumMeshes == 0)
    {
        return;
    }

    Core::Matrix4 matrix = transform * assimpToCore(node->mTransformation);

    if (node->mNumMeshes > 0)
    {
        Engine::Entity* entity = engine->createEntity();
        Engine::DrawableComponent* component = new Engine::DrawableComponent;
        engine->addComponent(component, entity, "RenderSystem");

        entity->setTransform(matrix);

        for (uint i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            std::string name;
            Core::StringUtils::stringPrintf(name, "Mesh_%s_%u", node->mName.C_Str(), currentMesh++);

            loadMesh(mesh, component, name);

            if (scene->HasMaterials())
            {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                name.clear();
                Core::StringUtils::stringPrintf(name, "Material_%u", mesh->mMaterialIndex);

                loadMaterial(material, component, name);
            }
            else
            {
                name = "Material_default";
                loadDefaultMaterial(component, name);
            }
        }
    }

    for (uint i = 0; i < node->mNumChildren; ++i)
    {
        runThroughNodes(node->mChildren[i], scene, matrix, engine);
    }
}

void loadMesh(const aiMesh* mesh, Engine::DrawableComponent* component, const std::string& name)
{
    Core::TriangleMesh data;
    Engine::Mesh* emesh = new Engine::Mesh(name);

    Core::VectorArray<Core::Vector3> tangents;
    Core::VectorArray<Core::Vector3> bitangents;

    for (uint i = 0; i < mesh->mNumVertices; ++i)
    {
        data.m_vertices.push_back(assimpToCore(mesh->mVertices[i]));
        data.m_normals.push_back(assimpToCore(mesh->mNormals[i]));

        if (mesh->HasTangentsAndBitangents())
        {
            tangents.push_back(assimpToCore(mesh->mTangents[i]));
            bitangents.push_back(assimpToCore(mesh->mBitangents[i]));
        }
    }

    for (uint i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace f = mesh->mFaces[i];

        data.m_triangles.push_back(
                    Core::Triangle(f.mIndices[0], f.mIndices[1], f.mIndices[2]));
    }

    emesh->loadGeometry(data, tangents, bitangents);
    component->addDrawable(emesh);
}

void loadMaterial(const aiMaterial* mat, Engine::DrawableComponent* component, const std::string& name)
{
    if (mat == nullptr)
    {
        loadDefaultMaterial(component, name);
        return;
    }

    // TODO(Charly): Handle different shader programs
    // TODO(Charly): Handle transparency
    Engine::ShaderProgram* shader = Engine::ShaderProgramManager::getInstancePtr()->getShaderProgram(blinnPhongShaderConfiguration);

    Engine::Material* material = new Engine::Material(name);
    material->setShaderProgram(shader);

    aiColor4D color;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, color))
    {
        Core::Color c = assimpToCore(color);
        material->setKd(c);
        fprintf(stderr, "%s kd : %.3f %.3f %.3f\n", name.c_str(), c.x(), c.y(), c.z());
    }

    if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, color))
    {
        Core::Color c = assimpToCore(color);
        material->setKs(c);
        fprintf(stderr, "%s ks : %.3f %.3f %.3f\n", name.c_str(), c[0], c[1], c[2]);
    }

    component->setMaterial(material);
}

void loadDefaultMaterial(Engine::DrawableComponent* component, const std::string& name)
{
    Engine::ShaderProgram* shader = Engine::ShaderProgramManager::getInstancePtr()->getShaderProgram(blinnPhongShaderConfiguration);
    Engine::Material* material = new Engine::Material(name);
    material->setShaderProgram(shader);
    component->setMaterial(material);
}

void loadLights(const aiScene* scene, Engine::RadiumEngine* engine)
{
    Engine::RenderSystem* renderer = static_cast<Engine::RenderSystem*>(
                engine->getSystem("RenderSystem"));
    for (uint i = 0; i < scene->mNumLights; ++i)
    {
        aiLight* ailight = scene->mLights[i];

        aiString name = ailight->mName;
        aiNode* node = scene->mRootNode->FindNode(name);

        Core::Matrix4 transform(Core::Matrix4::Identity());

        if (node != nullptr)
        {
            transform = assimpToCore(scene->mRootNode->mTransformation) *
                    assimpToCore(node->mTransformation);
        }

        Core::Color color(ailight->mColorDiffuse.r,
                          ailight->mColorDiffuse.g,
                          ailight->mColorDiffuse.b, 1.0);

        switch (ailight->mType)
        {
            case aiLightSource_DIRECTIONAL:
            {
                Core::Vector4 dir(ailight->mDirection[0],
                                  ailight->mDirection[1],
                                  ailight->mDirection[2], 0.0);
                dir = transform.transpose().inverse() * dir;

                Core::Vector3 finalDir(dir.x(), dir.y(), dir.z());
                finalDir = -finalDir;

                Engine::DirectionalLight* light = new Engine::DirectionalLight;
                light->setColor(color);
                light->setDirection(finalDir);

                renderer->addLight(light);

            } break;

            case aiLightSource_POINT:
            {
                Core::Vector4 pos(ailight->mPosition[0],
                                  ailight->mPosition[1],
                                  ailight->mPosition[2], 1.0);
                pos = transform * pos;
                pos /= pos.w();

                Engine::PointLight* light = new Engine::PointLight;
                light->setColor(color);
                light->setPosition(Core::Vector3(pos.x(), pos.y(), pos.z()));
                light->setAttenuation(ailight->mAttenuationConstant,
                                      ailight->mAttenuationLinear,
                                      ailight->mAttenuationQuadratic);

                renderer->addLight(light);

            } break;

            case aiLightSource_SPOT:
            {
                Core::Vector4 pos(ailight->mPosition[0],
                                  ailight->mPosition[1],
                                  ailight->mPosition[2], 1.0);
                pos = transform * pos;
                pos /= pos.w();

                Core::Vector4 dir(ailight->mDirection[0],
                                  ailight->mDirection[1],
                                  ailight->mDirection[2], 0.0);
                dir = transform.transpose().inverse() * dir;

                Core::Vector3 finalDir(dir.x(), dir.y(), dir.z());
                finalDir = -finalDir;

                Engine::SpotLight* light = new Engine::SpotLight;
                light->setColor(color);
                light->setPosition(Core::Vector3(pos.x(), pos.y(), pos.z()));
                light->setDirection(finalDir);

                light->setAttenuation(ailight->mAttenuationConstant,
                                      ailight->mAttenuationLinear,
                                      ailight->mAttenuationQuadratic);

                light->setInnerAngleInRadians(ailight->mAngleInnerCone);
                light->setOuterAngleInRadians(ailight->mAngleOuterCone);

                renderer->addLight(light);

            } break;

            case aiLightSource_UNDEFINED:
            default:
            {
                fprintf(stderr, "Light %s has undefined type.\n", name.C_Str());
            } break;
        }
    }
}

void assimpToCore(const aiVector3D& inVector, Core::Vector3& outVector)
{
    for (uint i = 0; i < 3; ++i)
    {
        outVector[i] = inVector[i];
    }
}

void assimpToCore(const aiColor4D& inColor, Core::Color& outColor)
{
    for (uint i = 0; i < 4; ++i)
    {
        outColor[i] = inColor[i];
    }
}

void assimpToCore(const aiMatrix4x4& inMatrix, Core::Matrix4& outMatrix)
{
    for (uint i = 0; i < 4; ++i)
    {
        for (uint j = 0; j < 4; ++j)
        {
            outMatrix(i, j) = inMatrix[i][j];
        }
    }
}

Core::Vector3 assimpToCore(const aiVector3D& vector)
{
    Core::Vector3 v;
    assimpToCore(vector, v);
    return v;
}

Core::Color assimpToCore(const aiColor4D& color)
{
    Core::Color c;
    assimpToCore(color, c);
    return c;
}

Core::Matrix4 assimpToCore(const aiMatrix4x4& matrix)
{
    Core::Matrix4 m;
    assimpToCore(matrix, m);
    return m;
}

} // namespace


} // namespace Ra
