#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>

namespace Ra
{
    namespace Engine
    {

        ShaderConfiguration::ShaderConfiguration(const std::string& name)
            : m_name(name)
        {
        }

        ShaderConfiguration::ShaderConfiguration(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader)
            : m_name(name)
        {
            m_shaders[ShaderType_VERTEX] = vertexShader;
            m_shaders[ShaderType_FRAGMENT] = fragmentShader;
        }

        void ShaderConfiguration::addShader(ShaderType type, const std::string& name)
        {
            m_shaders[type] = name;
        }

        void ShaderConfiguration::addProperty( const std::string& prop )
        {
            m_properties.insert( prop );
        }

        void ShaderConfiguration::addProperties( const std::list<std::string>& props )
        {
            for ( const auto& prop : props )
            {
                m_properties.insert( prop );
            }
        }

        void ShaderConfiguration::removeProperty( const std::string& prop )
        {
            m_properties.erase( prop );
        }

        bool ShaderConfiguration::isComplete() const
        {
             return ((m_shaders[ShaderType_VERTEX] != "") && (m_shaders[ShaderType_FRAGMENT] != "")) || m_shaders[ShaderType_COMPUTE] != "";
        }

        bool ShaderConfiguration::operator< (const ShaderConfiguration& o) const
        {
            bool res;

            for (size_t i = 0; i < ShaderType_COUNT; ++i)
            {
                if (m_shaders[i] != o.m_shaders[i])
                {
                    return m_shaders[i] < o.m_shaders[i];
                }
            }

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


            return res;
        }

        std::set<std::string> ShaderConfiguration::getProperties() const
        {
            return m_properties;
        }


    }
} // namespace Ra
