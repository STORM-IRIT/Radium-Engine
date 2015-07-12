#ifndef RADIUMENGINE_SHADERCONFIGURATION_HPP
#define RADIUMENGINE_SHADERCONFIGURATION_HPP

#include <list>
#include <string>
#include <set>

#include <CoreMacros.hpp>

namespace Ra
{

struct ShaderType
{
	std::string getShaderTypeString(uint type);
};

class ShaderConfiguration
{
public:
	enum ShaderType
	{
		VERTEX_SHADER = 0x1,
		FRAGMENT_SHADER = 0x2,
		GEOMETRY_SHADER = 0x4,
		TESSELATIONC_SHADER = 0x8,
		TESSELATIONE_SHADER = 0x10,
		TESSELATION_SET = TESSELATIONC_SHADER | TESSELATIONE_SHADER, // 0x18

		DEFAULT_SHADER_PROGRAM = VERTEX_SHADER | FRAGMENT_SHADER, // 0x3
		DEFAULT_SHADER_PROGRAM_W_GEOM = DEFAULT_SHADER_PROGRAM | GEOMETRY_SHADER, // 0x7
		COMPLETE_SHADER_PROGRAM = DEFAULT_SHADER_PROGRAM_W_GEOM | TESSELATION_SET // 0x1F
	};

public:
	ShaderConfiguration() = default;
	explicit ShaderConfiguration(const std::string& name,
								 const std::string& path,
								 const ShaderType& type = DEFAULT_SHADER_PROGRAM);

	void addProperty(const std::string& prop);
	void addProperties(const std::list<std::string>& props);
	void removeProperty(const std::string& prop);

	bool operator<(const ShaderConfiguration& other) const;

	void setName(const std::string& name);
	void setPath(const std::string& path);
	void setType(const ShaderType&  type);

	std::string getName() const;
	std::string getPath() const;
	std::string getFullName() const;
	ShaderType getType() const;
	std::set<std::string> getProperties() const;

private:
	std::string m_name;
	std::string m_shaderPath;
	ShaderType m_type;
	std::set<std::string> m_properties;
};

} // namespace Ra

#endif // RADIUMENGINE_SHADERCONFIGURATION_HPP