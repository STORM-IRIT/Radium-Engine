#include <Engine/Renderer/Shader/ShaderProgramManager.hpp>

#include <cstdio>

#include <Engine/Renderer/Shader/ShaderConfiguration.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>

// TODO (Charly) :  Remove Exceptions uses

namespace Ra
{

Engine::ShaderProgramManager::ShaderProgramManager(const std::string& shaderPath,
                                                   const std::string& defaultShaderProgram)
	: m_shaderPath(shaderPath)
{
	ShaderConfiguration config = getDefaultShaderConfiguration(defaultShaderProgram);
	m_defaultShaderProgram = new ShaderProgram(config);
}

Engine::ShaderProgramManager::~ShaderProgramManager()
{
	m_shaderPrograms.clear();
	m_shaderProgramStatus.clear();
}

Engine::ShaderProgram* Engine::ShaderProgramManager::addShaderProgram(const std::string& name)
{
	ShaderConfiguration config = getDefaultShaderConfiguration(name);
	return addShaderProgram(config);
}

Engine::ShaderProgram* Engine::ShaderProgramManager::addShaderProgram(const ShaderConfiguration& config)
{
	ShaderProgram* ret;

	// Check if not already inserted
	if (m_shaderPrograms.find(config) != m_shaderPrograms.end())
	{
		ret = m_shaderPrograms[config];
	}
	else
	{
		try
		{
			// Try to load the shader
			ShaderProgram* shader = new ShaderProgram(config);
			insertShader(config, shader, ShaderProgramStatus::COMPILED);
			ret = shader;
		}
		catch (std::exception& e)
		{
			// Shader encountered an error :
			//  -> Print error message,
			//  -> Set it as not compiled
			//  -> Replace it by default shader
                        fprintf(stderr, "Error occurred while loading shader program %s :\n%s\nDefault shader program used instead.\n",
                                        config.getName().c_str(), e.what());

			insertShader(config, m_defaultShaderProgram, ShaderProgramStatus::NOT_COMPILED);
			ret = m_defaultShaderProgram;
		}
	}

	return ret;
}

#if 0
int ShaderProgramManager::getShaderId(const std::string& name) const
{
	Z_ASSERT(m_shaderPrograms.find(name) != m_shaderPrograms.end(), "ShaderProgram not in the map.");
	return m_shaderPrograms.at(name)->getId();
}
#endif

Engine::ShaderProgram* Engine::ShaderProgramManager::getShaderProgram(const ShaderConfiguration& config)
{
	//Z_ASSERT(m_shaderPrograms.find(name) != m_shaderPrograms.end(), "ShaderProgram not in the map.");
	ShaderProgram* ret;

	if (m_shaderPrograms.find(config) != m_shaderPrograms.end())
	{
		// Already in the map
		ret = m_shaderPrograms[config];
	}
	else
	{
		ret = addShaderProgram(config);
	}

	return ret;
}

void Engine::ShaderProgramManager::reloadAllShaderPrograms()
{
	// For each shader in the map
	for (auto shader : m_shaderPrograms)
	{
		if (m_shaderProgramStatus.at(shader.first) == ShaderProgramStatus::COMPILED)
		{
			// Shader program has already been compiled successfully, try to reload it
			try
			{
				shader.second->reload();
			}
			catch (std::exception& e)
			{
				// Shader encountered an error :
				//  -> Print error message,
				//  -> Set it as not compiled
				//  -> Replace it by default shader
				fprintf(stderr, "Error occurred while loading shader program %s :\n%s\nDefault shader program used instead.\n",
                                                shader.first.getName().c_str(), e.what());

				m_shaderPrograms.at(shader.first) = m_defaultShaderProgram;
				m_shaderProgramStatus.at(shader.first) = ShaderProgramStatus::NOT_COMPILED;
			}
		}
		else
		{
			// Shader program has not been compiled properly, create a new shader
			try
			{
				ShaderProgram* newShader = new ShaderProgram(shader.first);

				// Ok compiled, register it in the map
				m_shaderPrograms.at(shader.first) = newShader;
				m_shaderProgramStatus.at(shader.first) = ShaderProgramStatus::COMPILED;
			}
			catch (std::exception& e)
			{
				// Still not compiled, print exception. Keep default shader.
				fprintf(stderr, "Error occurred while loading shader program %s:\n%s\n\nDefault shader program used instead.\n",
                                                shader.first.getName().c_str(), e.what());
			}
		}
	}
}

void Engine::ShaderProgramManager::reloadNotCompiledShaderPrograms()
{
	for (auto shader : m_shaderPrograms)
	{
		// Just look not compiled shaders
		if (m_shaderProgramStatus.at(shader.first) == ShaderProgramStatus::NOT_COMPILED)
		{
			try
			{
				ShaderProgram* newShader = new ShaderProgram(shader.first);

				// Ok compiled, register it in the map
				m_shaderPrograms.at(shader.first) = newShader;
				m_shaderProgramStatus.at(shader.first) = ShaderProgramStatus::COMPILED;
			}
			catch (std::exception& e)
			{
				// Still not compiled, print exception. Keep default shader.
				fprintf(stderr, "Error occurred while loading shader program %s :\n%s\nDefault shader program used instead.\n",
                                                shader.first.getName().c_str(), e.what());
			}
		}
	}
}

Engine::ShaderConfiguration Engine::ShaderProgramManager::getDefaultShaderConfiguration(const std::string& shaderName)
{
	return ShaderConfiguration(shaderName, m_shaderPath);
}

Engine::ShaderProgram* Engine::ShaderProgramManager::getDefaultShaderProgram() const
{
	return m_defaultShaderProgram;
}

std::string Engine::ShaderProgramManager::getFullShaderName(const std::string& shaderName)
{
	std::stringstream ss;
	ss << m_shaderPath << '/' << shaderName;
	return ss.str();
}

void Engine::ShaderProgramManager::insertShader(const ShaderConfiguration& config,
                                                ShaderProgram* shader,
                                                const ShaderProgramStatus& status)
{
	m_shaderPrograms.insert(std::pair<ShaderConfiguration, ShaderProgram*>(config, shader));
	m_shaderProgramStatus.insert(std::pair<ShaderConfiguration, ShaderProgramStatus>(config, status));
}

} // namespace Ra
