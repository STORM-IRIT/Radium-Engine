#include <Engine/Renderer/FancyMeshPlugin/FancyMeshLoader.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Core/Math/Vector.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/String/StringUtils.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>
#include <Engine/Renderer/Shader/ShaderProgramManager.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Light/SpotLight.hpp>
#include <Engine/Renderer/Light/PointLight.hpp>

// FIXME(Charly): Many stuff to fix here to have the compatibility with the new
//                Drawables architecture.
// FIXME(Charly): Shouldn't MeshLoader move into the FancyMeshPlugin ?

namespace Ra
{

namespace
{
const Engine::ShaderConfiguration defaultShaderConf("BlinnPhong", "../Shaders");
const Engine::ShaderConfiguration contourShaderConf("BlinnPhongContour", "../Shaders",
                                                    Engine::ShaderConfiguration::DEFAULT_SHADER_PROGRAM_W_GEOM);
const Engine::ShaderConfiguration wireframeShaderConf("BlinnPhongWireframe", "../Shaders",
                                                      Engine::ShaderConfiguration::DEFAULT_SHADER_PROGRAM_W_GEOM);

std::string filepath;
static DataVector dataVector;

void assimpToCore(const aiVector3D& inVector, Core::Vector3& outVector);
void assimpToCore(const aiColor4D& inColor, Core::Color& outColor);
void assimpToCore(const aiMatrix4x4& inMatrix, Core::Matrix4& outMatrix);

Core::Vector3 assimpToCore(const aiVector3D&  vector);
Core::Color   assimpToCore(const aiColor4D&   color);
Core::Matrix4 assimpToCore(const aiMatrix4x4& matrix);

void runThroughNodes(const aiNode* node, const aiScene* scene,
                     const Core::Matrix4& transform);

void loadMesh(const aiMesh* mesh, Engine::FancyMeshData& data);

void loadMaterial(const aiMaterial* mat, Engine::FancyComponentData& data);
void loadDefaultMaterial(Engine::FancyComponentData& data);
}

DataVector Engine::FancyMeshLoader::loadFile(const std::string & name)
{
    fprintf(stderr, "Loading file \"%s\"...\n", name.c_str());

	dataVector.clear();

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
        return dataVector;
    }

    fprintf(stderr, "About to load file %s :\n", name.c_str());
    fprintf(stderr, "\tFound %d meshes and %d materials\n", scene->mNumMeshes, scene->mNumMaterials);

    filepath = Core::StringUtils::getDirName(name);
    fprintf(stderr, "Path : %s\n", filepath.c_str());

    runThroughNodes(scene->mRootNode, scene, Core::Matrix4::Identity());

    fprintf(stderr, "Loaded successfully. Vector size : %zu\n", dataVector.size());

	return dataVector;
}

namespace
{

void runThroughNodes(const aiNode* node, const aiScene* scene,
                     const Core::Matrix4& transform)
{
    if (node->mNumChildren == 0 && node->mNumMeshes == 0)
    {
        return;
    }

    Core::Matrix4 matrix = transform * assimpToCore(node->mTransformation);

    if (node->mNumMeshes > 0)
    {
		Engine::FancyComponentData data;
		data.transform = matrix;
		data.name = node->mName.C_Str();

		// Consider only the first material for a given component
		if (scene->HasMaterials())
		{
			aiMaterial* material = scene->mMaterials[scene->mMeshes[node->mMeshes[0]]->mMaterialIndex];
			loadMaterial(material, data);
		}
		else
		{
			loadDefaultMaterial(data);
		}

		for (uint i = 0; i < node->mNumMeshes; ++i)
		{
			Engine::FancyMeshData meshData;

			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			loadMesh(mesh, meshData);

			data.meshes.push_back(meshData);
		}

		dataVector.push_back(data);
	}

	for (uint i = 0; i < node->mNumChildren; ++i)
	{
		runThroughNodes(node->mChildren[i], scene, matrix);
	}
}

void loadMesh(const aiMesh* mesh, Engine::FancyMeshData& data)
{
	Core::TriangleMesh meshData;
	Core::Vector3Array tangents;
	Core::Vector3Array bitangents;
	Core::Vector3Array texcoords;

	for (uint i = 0; i < mesh->mNumVertices; ++i)
	{
		meshData.m_vertices.push_back(assimpToCore(mesh->mVertices[i]));
		meshData.m_normals.push_back(assimpToCore(mesh->mNormals[i]));

		if (mesh->HasTangentsAndBitangents())
		{
			tangents.push_back(assimpToCore(mesh->mTangents[i]));
			bitangents.push_back(assimpToCore(mesh->mBitangents[i]));
		}

		// FIXME(Charly): What do texture coords indices mean ? 
		if (mesh->HasTextureCoords(0))
		{
			texcoords.push_back(assimpToCore(mesh->mTextureCoords[0][i]));
		}
	}

	for (uint i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace f = mesh->mFaces[i];

		meshData.m_triangles.push_back(
			Core::Triangle(f.mIndices[0], f.mIndices[1], f.mIndices[2]));
	}

	data.mesh = meshData;
	data.tangents = tangents;
	data.bitangents = bitangents;
	data.texcoords = texcoords;
}

void loadMaterial(const aiMaterial* mat, Engine::FancyComponentData& data)
{
	std::string materialName = data.name.append("_Material");
	if (mat == nullptr)
	{
		loadDefaultMaterial(data);
		return;
	}

	// TODO(Charly): Handle different shader programs
	// TODO(Charly): Handle transparency
	Engine::Material* material = new Engine::Material(materialName);

	material->setDefaultShaderProgram(defaultShaderConf);
	material->setContourShaderProgram(contourShaderConf);
	material->setWireframeShaderProgram(wireframeShaderConf);

	aiColor4D color;
	if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, color))
	{
		material->setKd(assimpToCore(color));
	}
	else
	{
		material->setKd(Core::Color(0, 0, 0, 1));
	}

	if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, color))
	{
		material->setKs(assimpToCore(color));
	}
	else
	{
		material->setKs(Core::Color(0, 0, 0, 1));
	}

	Scalar shininess;
	if (AI_SUCCESS == mat->Get(AI_MATKEY_SHININESS, shininess))
	{
		material->setNs(shininess);
	}
	else
	{
		material->setKs(Core::Color(0, 0, 0, 1));
		material->setNs(1.0);
	}

    aiString name;
    if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), name))
    {
        material->addTexture(Engine::Material::TEX_DIFFUSE, filepath + "/" + std::string(name.C_Str()));
    }

    if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), name))
    {
        material->addTexture(Engine::Material::TEX_SPECULAR, filepath + "/" + std::string(name.C_Str()));
    }

    if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), name))
    {
        material->addTexture(Engine::Material::TEX_NORMAL, filepath + "/" + std::string(name.C_Str()));
    }

	if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(aiTextureType_SHININESS, 0), name))
	{
		material->addTexture(Engine::Material::TEX_SHININESS, filepath + "/" + std::string(name.C_Str()));
	}

    if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(aiTextureType_OPACITY, 0), name))
    {
        material->addTexture(Engine::Material::TEX_ALPHA, filepath + "/" + std::string(name.C_Str()));
    }

	data.material = material;
}

void loadDefaultMaterial(Engine::FancyComponentData& data)
{
	std::string materialName = data.name.append("_Material");

    Engine::Material* material = new Engine::Material(materialName);
    material->setDefaultShaderProgram(defaultShaderConf);
    material->setContourShaderProgram(contourShaderConf);
    material->setWireframeShaderProgram(wireframeShaderConf);
    
	data.material = material;
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
