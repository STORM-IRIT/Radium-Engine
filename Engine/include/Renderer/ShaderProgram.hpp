#ifndef RADIUMENGINE_SHADERPROGRAM_HPP
#define RADIUMENGINE_SHADERPROGRAM_HPP

#include <set>
#include <string>
#include <array>

#include <CoreMacros.hpp>
#include <math/Vector.hpp>
#include <math/Matrix.hpp>

#include <Renderer/ShaderConfiguration.hpp>

namespace Ra
{

class ShaderObject
{
public:
	ShaderObject();
	~ShaderObject();

	void loadAndCompile(uint type,
						const std::string& filename,
						const std::set<std::string>& properties);

	void reloadAndCompile(const std::set<std::string>& properties);

	uint getId() const;

private:
	bool parseFile(const std::string& filename, std::string& content);
	std::string load();
	void compile(const std::string& shader, const std::set<std::string>& properties);
	void check();

private:
	uint m_id;
	std::string m_filename;
	uint m_type;
	std::set<std::string> m_properties;
};

class ShaderProgram
{
	enum ShaderType
	{
		VERTEX_SHADER = 0,
		TESS_CONTROL_SHADER,
		TESS_EVALUATION_SHADER,
		GEOMETRY_SHADER,
		FRAGMENT_SHADER,
		COMPUTE_SHADER,
		SHADER_TYPE_COUNT
	};
public:
	ShaderProgram();
	explicit ShaderProgram(const ShaderConfiguration& shaderConfig);
	virtual ~ShaderProgram();

	void load(const ShaderConfiguration& shaderConfig);
	void reload();

	ShaderConfiguration getBasicConfiguration() const;
	//void addProperty(const std::string& property);
	//void delProperty(const std::string& property);
	//void removeAllProperties() { m_properties.clear(); }

	void bind();
	//void bind(const RenderParameters& params);
	void unbind();

	uint getId() const;

	// Uniform setters
	void setUniform(const char* name, int value) const;
	void setUniform(const char* name, uint value) const;
	void setUniform(const char* name, Scalar value) const;

	void setUniform(const char* name, const Vector2&  value) const;
	void setUniform(const char* name, const Vector3& value) const;
	void setUniform(const char* name, const Vector4& value) const;

	void setUniform(const char* name, const Matrix2& value) const;
	void setUniform(const char* name, const Matrix3& value) const;
	void setUniform(const char* name, const Matrix4& value) const;

	// TODO (Charly) : Add Texture support
	//void setUniform(const char* name, Texture* tex, int texUnit) const;

private:
	//  bool exists(const std::string& filename);
	void loadVertShader(const std::string& name,
						const std::set<std::string>& props);
	void loadFragShader(const std::string& name,
						const std::set<std::string>& props);
	void loadTessShader(const std::string& name,
						const std::set<std::string>& props,
						const ShaderConfiguration::ShaderType& type);
	void loadGeomShader(const std::string& name,
						const std::set<std::string>& props,
						const ShaderConfiguration::ShaderType& type);
	void link();

private:
	ShaderConfiguration m_configuration;
	uint m_shaderId;
	std::array<ShaderObject*, SHADER_TYPE_COUNT> m_shaderObjects;

	bool m_binded;
};

} // namespace Ra

#endif // RADIUMENGINE_SHADERPROGRAM_HPP