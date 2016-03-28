#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <regex>

#include <cstdio>
#ifdef OS_WINDOWS
#include <direct.h>
#define getCurrentDir _getcwd
#else
#include <unistd.h>
#define getCurrentDir getcwd
#endif

#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>

namespace Ra
{
    namespace Engine
    {
        // From OpenGL Shading Language 3rd Edition, p215-216
        std::string getShaderInfoLog( GLuint shader )
        {
            int infoLogLen = 0;
            int charsWritten = 0;
            GLchar* infoLog;
            std::stringstream ss;

            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLogLen );

            if ( infoLogLen > 0 )
            {
                infoLog = new GLchar[infoLogLen];
                // error check for fail to allocate memory omitted
                glGetShaderInfoLog( shader, infoLogLen, &charsWritten, infoLog );
                ss << "InfoLog : " << std::endl << infoLog << std::endl;
                delete[] infoLog;
            }

            return ss.str();
        }

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

        ShaderObject::ShaderObject()
            : m_id( 0 )
        {
        }

        ShaderObject::~ShaderObject()
        {
            if ( m_id != 0 )
            {
                GL_ASSERT( glDeleteShader( m_id ) );
            }
        }

        bool ShaderObject::loadAndCompile( uint type, const std::string& filename, const std::set<std::string>& properties )
        {
            m_filename = filename;
            m_filepath = Core::StringUtils::getDirName(m_filename) + "/";
            m_type = type;
            m_properties = properties;
            GL_ASSERT( m_id = glCreateShader( type ) );

            std::string shader = preprocessIncludes(load());

            if ( shader != "" )
            {
                compile( shader, properties );
            }

            return check();
        }

        bool ShaderObject::reloadAndCompile( const std::set<std::string>& properties )
        {
            LOG( logINFO ) << "Reloading shader " << m_filename;
            return loadAndCompile( m_type, m_filename, properties );
        }

        uint ShaderObject::getId() const
        {
            return m_id;
        }

        std::string ShaderObject::load()
        {
            std::string shader;

            bool ok = parseFile( m_filename, shader );
            if ( !ok )
            {
                std::ostringstream error;
                error << m_filename << " not found.\n";
                char currentPath[FILENAME_MAX];
                if (getCurrentDir( currentPath, sizeof( currentPath ) ) != nullptr)
                {
                    error << "Path : " << currentPath;
                }
                CORE_WARN_IF( !ok, error.str().c_str() );
                return "";
            }

            return shader;
        }

        std::string ShaderObject::preprocessIncludes(const std::string& shader, int level)
        {
            CORE_ERROR_IF(level < 32, "Shader inclusion depth limit reached.");

            std::string result = "";
            std::vector<std::string> finalStrings;
            auto shaderLines = Core::StringUtils::splitString(shader, '\n');
            finalStrings.reserve(shaderLines.size());

            static const std::regex reg("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");

            for (const auto& l : shaderLines)
            {
                std::string line = l;
                std::smatch match;
                if (std::regex_search(l, match, reg))
                {
                    std::string inc;
                    std::string file = m_filepath + match[1].str();
                    if (parseFile(file, inc))
                    {
                        line = preprocessIncludes(inc, level + 1);
                    }
                    else
                    {
                        LOG(logWARNING) << "Cannot open included file " << file << " from " << m_filename << ". Ignored.";
                        continue;
                    }
                }

                finalStrings.push_back(line);
            }

            // Build final shader string
            for (const auto& l : finalStrings)
            {
                result.append(l);
                result.append("\n");
            }

            result.append("\0");

            return result;
        }

        void ShaderObject::compile( const std::string& shader, const std::set<std::string>& properties )
        {
            const char* data[3];
            data[0] = "#version 330\n";

            std::stringstream ss;
            for ( auto property : properties )
            {
                ss << property << "\n";
            }
            std::string str = ss.str();
            data[1] = str.c_str();
            data[2] = shader.c_str();

            GL_ASSERT( glShaderSource( m_id, 3, data, nullptr ) );
            GL_ASSERT( glCompileShader( m_id ) );
        }

        bool ShaderObject::check()
        {
            GLint ok;
            std::stringstream error;
            GL_ASSERT( glGetShaderiv( m_id, GL_COMPILE_STATUS, &ok ) );

            if ( !ok )
            {
                error << m_filename << " not compiled.\n";
                error << getShaderInfoLog( m_id );
                glDeleteShader( m_id );

                // For now, crash when a shader is not compiling
                CORE_ERROR_IF( ok, error.str().c_str() );
            }
            return !( !ok );
        }

        bool ShaderObject::parseFile( const std::string& filename, std::string& content )
        {
            std::ifstream ifs( filename.c_str(), std::ios::in );
            if ( !ifs )
            {
                return false;
            }

            std::stringstream buf;
            buf << ifs.rdbuf();
            content = buf.str();

            ifs.close();
            return true;
        }

        ShaderProgram:: ShaderProgram()
            : m_shaderId( 0 )
        {
            for ( uint i = 0; i < m_shaderObjects.size(); ++i )
            {
                m_shaderObjects[i] = nullptr;
                m_shaderStatus[i]  = false;
            }
        }

        ShaderProgram::ShaderProgram( const ShaderConfiguration& config )
            : ShaderProgram()
        {
            load( config );
        }

        ShaderProgram::~ShaderProgram()
        {
            if ( m_shaderId != 0 )
            {
                for ( auto shader : m_shaderObjects )
                {
                    if ( shader && ( shader->getId() != 0 ) )
                    {
                        GL_ASSERT( glDetachShader( m_shaderId, shader->getId() ) );
                        delete shader;
                    }
                }
                glDeleteProgram( m_shaderId );
            }
        }

        uint ShaderProgram::getId() const
        {
            return m_shaderId;
        }

        bool ShaderProgram::isOk() const
        {
            bool ok = true;
            for ( uint i = 0; i < ShaderType_COUNT; ++i )
            {
                if ( m_configuration.m_shaders[i] != "" )
                {
                    ok = ( ok && m_shaderStatus[i] );
                }
            }
            return ok;
        }

        void ShaderProgram::loadShader(ShaderType type, const std::string& name, const std::set<std::string>& props)
        {
    #ifdef OS_MACOS
            if (type == ShaderType_COMPUTE)
            {
                LOG(logERROR) << "No compute shader on OsX <= El Capitan";
                return;
            }
    #endif
            ShaderObject* shader = new ShaderObject;
            bool status = shader->loadAndCompile(getTypeAsGLEnum(type), name, props);
            m_shaderObjects[type] = shader;
            m_shaderStatus[type] = status;
        }

        uint ShaderProgram::getTypeAsGLEnum(ShaderType type) const
        {
            switch(type)
            {
                case ShaderType_VERTEX: return GL_VERTEX_SHADER;
                case ShaderType_FRAGMENT: return GL_FRAGMENT_SHADER;
                case ShaderType_GEOMETRY: return GL_GEOMETRY_SHADER;
                case ShaderType_TESS_EVALUATION: return GL_TESS_EVALUATION_SHADER;
                case ShaderType_TESS_CONTROL: return GL_TESS_CONTROL_SHADER;
                case ShaderType_COMPUTE: return GL_COMPUTE_SHADER;
                default: CORE_ERROR("Wrong ShaderType");
            }
            // Should never get there
            return 0;
        }

        void ShaderProgram::load( const ShaderConfiguration& shaderConfig )
        {
            //    LOG(INFO) << "Loading shader " << shaderConfig.getName() << " <type = " << std::hex << shaderConfig.getType() << std::dec << ">";
            m_configuration = shaderConfig;

            CORE_ERROR_IF(m_configuration.isComplete(), ("Shader program " + shaderConfig.m_name + " misses vertex or fragment shader.").c_str());

            GL_ASSERT( m_shaderId = glCreateProgram() );

            for (size_t i = 0; i < ShaderType_COUNT; ++i)
            {
                if (m_configuration.m_shaders[i] != "")
                {
                    loadShader(ShaderType(i), m_configuration.m_shaders[i], m_configuration.getProperties());
                }
            }

            link();
        }

        void ShaderProgram::link()
        {
            for ( auto shader : m_shaderObjects )
            {
                if ( shader )
                {
                    GL_ASSERT( glAttachShader( m_shaderId, shader->getId() ) );
                }
            }

            GL_ASSERT( glLinkProgram( m_shaderId ) );
        }

        void ShaderProgram::bind() const
        {
            CORE_ASSERT( m_shaderId != 0, "Shader is not initialized" );
            GL_ASSERT( glUseProgram( m_shaderId ) );
        }

        void ShaderProgram::unbind() const
        {
            GL_ASSERT( glUseProgram( 0 ) );
        }

        void ShaderProgram::reload()
        {
            for ( unsigned int i = 0; i < m_shaderObjects.size(); ++i )
            {
                if ( m_shaderObjects[i] != nullptr )
                {
                    GL_ASSERT( glDetachShader( m_shaderId, m_shaderObjects[i]->getId() ) );
                    m_shaderObjects[i]->reloadAndCompile( m_configuration.getProperties() );
                }
            }

            link();
        }

        ShaderConfiguration ShaderProgram::getBasicConfiguration() const
        {
            ShaderConfiguration basicConfig;
            basicConfig.m_shaders = m_configuration.m_shaders;
            return basicConfig;
        }

        void ShaderProgram::setUniform( const char* name, int value ) const
        {
            GL_ASSERT( glUniform1i( glGetUniformLocation( m_shaderId, name ), value ) );
        }

        void ShaderProgram::setUniform( const char* name, unsigned int value ) const
        {
            GL_ASSERT( glUniform1ui( glGetUniformLocation( m_shaderId, name ), value ) );
        }

        void ShaderProgram::setUniform( const char* name, float value ) const
        {
            GL_ASSERT( glUniform1f( glGetUniformLocation( m_shaderId, name ), value ) );
        }

        void ShaderProgram::setUniform( const char* name, double value ) const
        {
            float v = static_cast<float>(value);
            GL_ASSERT( glUniform1f( glGetUniformLocation( m_shaderId, name ), v ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector2f& value ) const
        {
            GL_ASSERT( glUniform2fv( glGetUniformLocation( m_shaderId, name ), 1, value.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector2d& value ) const
        {
            Core::Vector2f v = value.cast<float>();
            GL_ASSERT( glUniform2fv( glGetUniformLocation( m_shaderId, name ), 1, v.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector3f& value ) const
        {
            GL_ASSERT( glUniform3fv( glGetUniformLocation( m_shaderId, name ), 1, value.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector3d& value ) const
        {
            Core::Vector3f v = value.cast<float>();
            GL_ASSERT( glUniform3fv( glGetUniformLocation( m_shaderId, name ), 1, v.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector4f& value ) const
        {
            GL_ASSERT( glUniform4fv( glGetUniformLocation( m_shaderId, name ), 1, value.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector4d& value ) const
        {
            Core::Vector4f v = value.cast<float>();
            GL_ASSERT( glUniform4fv( glGetUniformLocation( m_shaderId, name ), 1, v.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix2f& value ) const
        {
            GL_ASSERT( glUniformMatrix2fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, value.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix2d& value ) const
        {
            Core::Matrix2f v = value.cast<float>();
            GL_ASSERT( glUniformMatrix2fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, v.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix3f& value ) const
        {
            GL_ASSERT( glUniformMatrix3fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, value.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix3d& value ) const
        {
            Core::Matrix3f v = value.cast<float>();
            GL_ASSERT( glUniformMatrix3fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, v.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix4f& value ) const
        {
            GL_ASSERT( glUniformMatrix4fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, value.data() ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix4d& value ) const
        {
            Core::Matrix4f v = value.cast<float>();
            GL_ASSERT( glUniformMatrix4fv( glGetUniformLocation( m_shaderId, name ), 1, GL_FALSE, v.data() ) );
        }

        // TODO : Provide Texture support
        void ShaderProgram::setUniform( const char* name, Texture* tex, int texUnit ) const
        {
            tex->bind( texUnit );
            GL_ASSERT( glUniform1i( glGetUniformLocation( m_shaderId, name ), texUnit ) );
        }
    }
} // namespace Ra
