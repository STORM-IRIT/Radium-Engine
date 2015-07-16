#ifndef RADIUMENGINE_SHADERPROGRAMANAGER_HPP
#define RADIUMENGINE_SHADERPROGRAMANAGER_HPP

#include <string>
#include <map>

#include <Core/Utils/Singleton.hpp>

// TODO (Charly) :  Since ShaderProgramManager has the responsability for the shaders,
//                  use shared_ptrs here

namespace Ra
{

class ShaderProgram;
class ShaderConfiguration;

class ShaderProgramManager : public Core::Singleton<ShaderProgramManager>
{
    friend class Core::Singleton<ShaderProgramManager>;

	enum class ShaderProgramStatus
	{
		NOT_COMPILED = 0,
		COMPILED = 1
	};

public:
	int getShaderId(const std::string& shader) const;

	ShaderProgram* addShaderProgram(const std::string& name);
	ShaderProgram* addShaderProgram(const ShaderConfiguration& config);

	ShaderProgram* getShaderProgram(const ShaderConfiguration& config);

	ShaderProgram* getDefaultShaderProgram() const;

	void reloadAllShaderPrograms();
	void reloadNotCompiledShaderPrograms();

private:
	ShaderProgramManager(const std::string& shaderPath,
						 const std::string& defaultShaderProgram);
	~ShaderProgramManager();

	void insertShader(const ShaderConfiguration& config, ShaderProgram* shader, const ShaderProgramStatus& status);
	ShaderConfiguration getDefaultShaderConfiguration(const std::string& shaderName);
	std::string getFullShaderName(const std::string& shaderName);

private:
	std::string m_shaderPath;

	std::map<ShaderConfiguration, ShaderProgram*> m_shaderPrograms;
	std::map<ShaderConfiguration, ShaderProgramStatus> m_shaderProgramStatus;
	
	ShaderProgram* m_defaultShaderProgram;
	
	int m_defaultShaderId;
};

} // namespace Ra

#endif // RADIUMENGINE_SHADERMANAGER_HPP
