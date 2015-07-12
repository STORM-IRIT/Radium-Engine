#include <Engine/Renderer/ShaderConfiguration.hpp>

#include <sstream>

#include <Engine/Renderer/OpenGL.hpp>

namespace Ra
{

ShaderConfiguration::ShaderConfiguration(const std::string& name,
										 const std::string& path,
										 const ShaderType& type)
										 : m_name(name)
										 , m_shaderPath(path)
										 , m_type(type)
{
}

void ShaderConfiguration::addProperty(const std::string& prop)
{
	m_properties.insert(prop);
}

void ShaderConfiguration::addProperties(const std::list<std::string>& props)
{
	for (const auto& prop : props)
	{
		m_properties.insert(prop);
	}
}

void ShaderConfiguration::removeProperty(const std::string& prop)
{
	m_properties.erase(prop);
}

bool ShaderConfiguration::operator<(const ShaderConfiguration& o) const
{
	bool res;

	if (m_name == o.m_name)
	{
		if (m_properties.size() == o.m_properties.size())
		{
			if (m_properties.size() == 0)
			{
				res = false;
			}
			else
			{
				auto lit = m_properties.begin();
				auto rit = o.m_properties.begin();

				for (; (lit != m_properties.end()) && (*lit == *rit); ++lit, ++rit);

				if (lit == m_properties.end())
				{
					res = false;
				}
				else
				{
					res = *lit < *rit;
				}
			}
		}
		else
		{
			res = m_properties.size() < o.m_properties.size();
		}
	}
	else
	{
		res = m_name < o.m_name;
	}

	return res;
}

void ShaderConfiguration::setName(const std::string& name)
{
	m_name = name;
}

void ShaderConfiguration::setPath(const std::string& path)
{
	m_shaderPath = path;
}

void ShaderConfiguration::setType(const ShaderConfiguration::ShaderType& type)
{
	m_type = type;
}

std::string ShaderConfiguration::getName() const
{
	return m_name;
}

std::string ShaderConfiguration::getPath() const
{
	return m_shaderPath;
}

std::string ShaderConfiguration::getFullName() const
{
	std::stringstream ss;
	ss << m_shaderPath << '/' << m_name;
	return ss.str();
}

ShaderConfiguration::ShaderType ShaderConfiguration::getType() const
{
	return m_type;
}

std::set<std::string> ShaderConfiguration::getProperties() const
{
	return m_properties;
}


std::string ShaderType::getShaderTypeString(unsigned int type)
{
	std::stringstream ss;
	switch (type)
	{
		case GL_VERTEX_SHADER:
			ss << "Vertex ";
			break;
		case GL_TESS_CONTROL_SHADER:
			ss << "Tessellation Control ";
			break;
		case GL_TESS_EVALUATION_SHADER:
			ss << "Tessellation Evaluation ";
			break;
		case GL_GEOMETRY_SHADER:
			ss << "Geometry ";
			break;
		case GL_FRAGMENT_SHADER:
			ss << "Fragment ";
			break;
		default:
			ss << "Unknown ";
	}
	return ss.str();
}

} // namespace Ra
