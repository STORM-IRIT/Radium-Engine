#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>

#include <sstream>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>

namespace Ra
{

    Engine::ShaderConfiguration::ShaderConfiguration( const std::string& name,
                                                      const std::string& path,
                                                      const ShaderType& type )
        : m_name( name )
        , m_shaderPath( path )
        , m_type( type )
    {
    }

    void Engine::ShaderConfiguration::addProperty( const std::string& prop )
    {
        m_properties.insert( prop );
    }

    void Engine::ShaderConfiguration::addProperties( const std::list<std::string>& props )
    {
        for ( const auto& prop : props )
        {
            m_properties.insert( prop );
        }
    }

    void Engine::ShaderConfiguration::removeProperty( const std::string& prop )
    {
        m_properties.erase( prop );
    }

    bool Engine::ShaderConfiguration::operator< ( const Engine::ShaderConfiguration& o ) const
    {
        bool res;

        if ( m_name == o.m_name )
        {
            if ( m_properties.size() == o.m_properties.size() )
            {
                if ( m_properties.size() == 0 )
                {
                    res = false;
                }
                else
                {
                    auto lit = m_properties.begin();
                    auto rit = o.m_properties.begin();

                    for ( ; ( lit != m_properties.end() ) && ( *lit == *rit ); ++lit, ++rit );

                    if ( lit == m_properties.end() )
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

    void Engine::ShaderConfiguration::setName( const std::string& name )
    {
        m_name = name;
    }

    void Engine::ShaderConfiguration::setPath( const std::string& path )
    {
        m_shaderPath = path;
    }

    void Engine::ShaderConfiguration::setType( const Engine::ShaderConfiguration::ShaderType& type )
    {
        m_type = type;
    }

    std::string Engine::ShaderConfiguration::getName() const
    {
        return m_name;
    }

    std::string Engine::ShaderConfiguration::getPath() const
    {
        return m_shaderPath;
    }

    std::string Engine::ShaderConfiguration::getFullName() const
    {
        std::stringstream ss;
        ss << m_shaderPath << '/' << m_name;
        return ss.str();
    }

    Engine::ShaderConfiguration::ShaderType Engine::ShaderConfiguration::getType() const
    {
        return m_type;
    }

    std::set<std::string> Engine::ShaderConfiguration::getProperties() const
    {
        return m_properties;
    }


    std::string Engine::ShaderType::getShaderTypeString( unsigned int type )
    {
        std::stringstream ss;
        switch ( type )
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
