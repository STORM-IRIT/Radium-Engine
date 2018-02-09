#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

#include <globjects/Shader.h>
#include <globjects/Program.h>
#include <globjects/Texture.h>
#include <globjects/NamedString.h>

#include <globjects/base/File.h>
#include <globjects/base/StaticStringSource.h>

#include <fstream>
#include <regex>

#ifdef OS_WINDOWS
#include <direct.h>
#define getCurrentDir _getcwd
#else
#include <unistd.h>
#define getCurrentDir getcwd
#endif

#include <Core/Math/GlmAdapters.hpp>

#include <Engine/Renderer/Texture/Texture.hpp>

namespace Ra
{
    namespace Engine
    {

        ShaderProgram::ShaderProgram()
            : m_program( nullptr )
        {
            for ( uint i = 0; i < m_shaderObjects.size(); ++i )
            {
                m_shaderObjects[i] = nullptr;
            }
        }

        ShaderProgram::ShaderProgram( const ShaderConfiguration& config )
            : ShaderProgram()
        {
            load( config );
        }

        ShaderProgram::~ShaderProgram()
        {

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
            // FIXME : --> for the moment : standard includepaths. Might be controlled per shader ...
            // Paths in which globjects will be looking for shaders includes.
            // "/" refer to the root of the directory structure conaining the shader (i.e. the Shaders/ directory).
            globjects::Shader::IncludePaths includePaths { std::string("/") };

            // FIXED : use auto instead of the fully qualified type
            auto loadedSource = globjects::Shader::sourceFromFile( name );

            // header string that contains #version and pre-declarations ...
            std::string shaderHeader;
            if( type == ShaderType_VERTEX )
            {
                shaderHeader = std::string( "#version 410\n\n"
                                            "out gl_PerVertex {\n"
                                            "    vec4 gl_Position;\n"
                                            "    float gl_PointSize;\n"
                                            "    float gl_ClipDistance[];\n"
                                            "};\n\n" );
            }
            else
            {
                shaderHeader = std::string( "#version 410\n\n");
            }

            for ( const auto& prop : props )
            {
                shaderHeader = shaderHeader + prop + std::string("\n\n");
            }

            auto fullsource = globjects::Shader::sourceFromString( shaderHeader + loadedSource->string() );

            // FIXME Where are defined the global replacement?
            auto shaderSource = globjects::Shader::applyGlobalReplacements( fullsource.get() );


            auto shader = globjects::Shader::create( getTypeAsGLEnum( type ) );
            shader->setIncludePaths( { std::string("/") } );

            // Workaround globject #include bug ...
            std::string preprocessedSource = preprocessIncludes(name, shaderSource->string(), 0);

            auto ptrSource = globjects::Shader::sourceFromString(preprocessedSource);

            shader->setSource(ptrSource.get());

            shader->setName( name );

            shader->compile();

            GL_CHECK_ERROR;
            m_shaderObjects[type].swap( shader );
        }

        GLenum ShaderProgram::getTypeAsGLEnum(ShaderType type) const
        {
            switch(type)
            {
                case ShaderType_VERTEX: return GL_VERTEX_SHADER;
                case ShaderType_FRAGMENT: return GL_FRAGMENT_SHADER;
                case ShaderType_GEOMETRY: return GL_GEOMETRY_SHADER;
                case ShaderType_TESS_EVALUATION: return GL_TESS_EVALUATION_SHADER;
                case ShaderType_TESS_CONTROL: return GL_TESS_CONTROL_SHADER;
#ifndef OS_MACOS
                    // FIXED (Mathias) : GL_COMPUTE_SHADER requires OpenGL >= 4.2, Apple provides OpenGL 4.1
                case ShaderType_COMPUTE: return GL_COMPUTE_SHADER;
#endif
                default: CORE_ERROR("Wrong ShaderType");
            }

            // Should never get there
            return GL_ZERO;
        }

        ShaderType ShaderProgram::getGLenumAsType(GLenum type) const
        {
            switch(type)
            {
                case GL_VERTEX_SHADER: return ShaderType_VERTEX;
                case GL_FRAGMENT_SHADER: return ShaderType_FRAGMENT;
                case GL_GEOMETRY_SHADER: return ShaderType_GEOMETRY;
                case GL_TESS_EVALUATION_SHADER: return ShaderType_TESS_EVALUATION;
                case GL_TESS_CONTROL_SHADER: return ShaderType_TESS_CONTROL;
#ifndef OS_MACOS
                case GL_COMPUTE_SHADER: return ShaderType_COMPUTE;
#endif
                default: CORE_ERROR("Wrong GLenum");
            }

            // Should never get there
            return ShaderType_COUNT;
        }

        void ShaderProgram::load( const ShaderConfiguration& shaderConfig )
        {
            m_configuration = shaderConfig;

            CORE_ERROR_IF( m_configuration.isComplete(), ("Shader program " + shaderConfig.m_name + " misses vertex or fragment shader.").c_str() );

            m_program = globjects::Program::create();

            for (size_t i = 0; i < ShaderType_COUNT; ++i)
            {
                if (m_configuration.m_shaders[i] != "")
                {
                    LOG( logDEBUG ) << "Loading shader " << m_configuration.m_shaders[i];
                    loadShader(ShaderType(i), m_configuration.m_shaders[i], m_configuration.getProperties());
                }
            }

            link();
        }

        void ShaderProgram::link()
        {
            for ( int i=0; i < ShaderType_COUNT; ++i )
            {
                if ( m_shaderObjects[i] )
                {
                    m_program->attach( m_shaderObjects[i].get() );
                }
            }

            m_program->setParameter( GL_PROGRAM_SEPARABLE, GL_TRUE );

            m_program->link();
            GL_CHECK_ERROR;
        }

        void ShaderProgram::bind() const
        {
            m_program->use();
        }

        void ShaderProgram::validate() const
        {
            m_program->validate();
            if (!m_program->isValid()) {
                LOG( logDEBUG ) << m_program->infoLog();
            }
        }

        void ShaderProgram::unbind() const
        {
            m_program->release();
        }

        void ShaderProgram::reload()
        {
            for ( unsigned int i = 0; i < m_shaderObjects.size(); ++i )
            {
                if ( m_shaderObjects[i] != nullptr )
                {
                    LOG( logDEBUG ) << "Reloading shader " << m_shaderObjects[i]->name();

                    m_program->detach( m_shaderObjects[i].get() );
                    loadShader( getGLenumAsType( m_shaderObjects[i]->type() ), m_shaderObjects[i]->name(), m_configuration.getProperties() );
                }
            }

            link();
        }

        ShaderConfiguration ShaderProgram::getBasicConfiguration() const
        {
            ShaderConfiguration basicConfig;

            basicConfig.m_shaders = m_configuration.m_shaders;
            basicConfig.m_name = m_configuration.m_name;

            return basicConfig;
        }

        void ShaderProgram::setUniform( const char* name, int value ) const
        {
            m_program->setUniform( name, value );
        }

        void ShaderProgram::setUniform( const char* name, unsigned int value ) const
        {
            m_program->setUniform(name, value);
        }

        void ShaderProgram::setUniform( const char* name, float value ) const
        {
            m_program->setUniform( name, value );
        }

        void ShaderProgram::setUniform( const char* name, double value ) const
        {
            float v = static_cast<float>(value);

            m_program->setUniform( name, v );
        }
        
        //!

        void ShaderProgram::setUniform( const char* name, std::vector<int> values ) const
        {
            m_program->setUniform( name, values );
        }

        void ShaderProgram::setUniform( const char* name, std::vector<unsigned int> values ) const
        {
            m_program->setUniform( name, values );
        }

        void ShaderProgram::setUniform( const char* name, std::vector<float> values ) const
        {
            m_program->setUniform( name, values );
        }
        
        //!

        void ShaderProgram::setUniform( const char* name, const Core::Vector2f& value ) const
        {
            m_program->setUniform( name, Core::toGlm( value ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector2d& value ) const
        {
            Core::Vector2f v = value.cast<float>();

            m_program->setUniform( name, Core::toGlm( v ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector3f& value ) const
        {
            m_program->setUniform( name, Core::toGlm( value ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector3d& value ) const
        {
            Core::Vector3f v = value.cast<float>();

            m_program->setUniform( name, Core::toGlm( v ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector4f& value ) const
        {
            m_program->setUniform( name, Core::toGlm( value ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Vector4d& value ) const
        {
            Core::Vector4f v = value.cast<float>();

            m_program->setUniform( name, Core::toGlm( v ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix2f& value ) const
        {
            m_program->setUniform( name, Core::toGlm( value ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix2d& value ) const
        {
            Core::Matrix2f v = value.cast<float>();

            m_program->setUniform( name, Core::toGlm( v ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix3f& value ) const
        {
            m_program->setUniform( name, Core::toGlm( value ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix3d& value ) const
        {
            Core::Matrix3f v = value.cast<float>();

            m_program->setUniform( name, Core::toGlm( v ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix4f& value ) const
        {
            m_program->setUniform( name, Core::toGlm( value ) );
        }

        void ShaderProgram::setUniform( const char* name, const Core::Matrix4d& value ) const
        {
            Core::Matrix4f v = value.cast<float>();

            m_program->setUniform( name, Core::toGlm( v ) );
        }
        
        void ShaderProgram::setUniform( const char* name, Texture* tex, int texUnit ) const
        {
            tex->bind( texUnit );

            m_program->setUniform( name, texUnit );
        }

        globjects::Program * ShaderProgram::getProgramObject() const
        {
            return m_program.get();
        }


        /****************************************************
         * Include workaround due to globject bugs
         ****************************************************/
        std::string ShaderProgram::preprocessIncludes(const std::string &name, const std::string& shader, int level, int line)
        {
            CORE_ERROR_IF(level < 32, "Shader inclusion depth limit reached.");

            std::string result = "";
            std::vector<std::string> finalStrings;
            auto shaderLines = Core::StringUtils::splitString(shader, '\n');
            finalStrings.reserve(shaderLines.size());

            uint nline = 0;

            static const std::regex reg("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");

            for (const auto& l : shaderLines)
            {
                std::string line = l;
                std::smatch match;
                if (std::regex_search(l, match, reg))
                {
                    // FIXME : use the includePaths set elsewhere.
                    auto includeNameString = globjects::NamedString::getFromRegistry(std::string("/") + match[1].str());
                    if (includeNameString != nullptr )
                    {

                        line = preprocessIncludes(match[1].str(), includeNameString->string(), level + 1, 0);

                    } else {
                        LOG(logWARNING) << "Cannot open included file " <<  match[1].str() << " at line" << nline << " of file " << name <<". Ignored.";
                        continue;
                    }
                    /*
                     std::string inc;
                     std::string file = m_filepath + match[1].str();
                     if (parseFile(file, inc))
                     {
                     sublerr.start = nline;
                     sublerr.name  = file;
                     lerr.subfiles.push_back(sublerr);
                     
                     line  = preprocessIncludes(inc, level + 1, lerr.subfiles.back());
                     nline = lerr.subfiles.back().end;
                     }
                     else
                     {
                     LOG(logWARNING) << "Cannot open included file " << file << " from " << m_filename << ". Ignored.";
                     continue;
                     }
                     */
                }

                finalStrings.push_back(line);
                ++ nline;
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


    }
} // namespace Ra
