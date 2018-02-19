#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>

/**
 * @todo : make default shader configuration/default shader programm usable
 * outside of the main radium distribution or
 * find a way to ensure that "plain.glsl.*" is always reachable
 * (and change the name of this).
 * @see TranslateGizmo and RotateGizmo that explicitely try to load the shader
 * "Plain" that is main-app dependant!
 *
 * \see issue #194
 */

#if 0
/** The following must define default shader. Perhaps this is not the good place for this and surely, this will need an
 * indepth modification of the shader system to ensure that default program is always correctly managed
 *      - creation/compilation at the first use
 *      - Find a way to notify client that the default shader was provided instead of the requested one
 *      - ...
 */
static const std::string defaultVertexShader(
                                             "struct Transform {\n"
                                             "    mat4 model;\n"
                                             "    mat4 view;\n"
                                             "    mat4 proj;\n"
                                             "    mat4 mvp;\n"
                                             "    mat4 modelView;\n"
                                             "    mat4 worldNormal;\n"
                                             "    mat4 viewNormal;\n"
                                             "};\n"
                                             "layout (location = 0) in vec3 in_position;\n"
                                             "layout (location = 4) in vec3 in_texcoord;\n"
                                             "layout (location = 5) in vec4 in_color;\n"
                                             "\n"
                                             "uniform Transform transform;\n"
                                             "uniform int drawFixedSize;\n"
                                             "\n"
                                             "layout (location = 0) out vec3 out_position;\n"
                                             "layout (location = 1) out vec3 out_texcoord;\n"
                                             "layout (location = 2) out vec3 out_color;\n"
                                             "\n"
                                             "void main()\n"
                                             "{\n"
                                             "    mat4 mvp;\n"
                                             "    if ( drawFixedSize > 0 )\n"
                                             "    {\n"
                                             "        // distance to camera\n"
                                             "        mat4 modelView = transform.view * transform.model;\n"
                                             "        float d = length( modelView[3].xyz );\n"
                                             "        mat3 scale3 = mat3(d);\n"
                                             "        mat4 scale = mat4(scale3);\n"
                                             "        mat4 model = transform.model * scale;\n"
                                             "        mvp = transform.proj * transform.view * model;\n"
                                             "    }\n"
                                             "    else\n"
                                             "    {\n"
                                             "        mvp = transform.proj * transform.view * transform.model;\n"
                                             "    }\n"
                                             "\n"
                                             "    gl_Position = mvp * vec4(in_position.xyz, 1.0);\n"
                                             "    out_color = in_color.xyz;\n"
                                             "    out_texcoord = in_texcoord;\n"
                                             "\n"
                                             "    vec4 pos = transform.model * vec4(in_position, 1.0);\n"
                                             "    pos /= pos.w;\n"
                                             "    out_position = vec3(pos);\n"
                                             "}\n"
                                             );
static const std::string defaultFragmentShader(
                                               "struct Material\n"
                                               "{\n"
                                               "    vec4 kd;\n"
                                               "    vec4 ks;    \n"
                                               "\n"
                                               "    float ns;\n"
                                               "    float alpha;\n"
                                               "\n"
                                               "    Textures tex;\n"
                                               "};\n"
                                               "struct Attenuation\n"
                                               "{\n"
                                               "    float constant;\n"
                                               "    float linear;\n"
                                               "    float quadratic;\n"
                                               "};\n"
                                               "\n"
                                               "struct DirectionalLight\n"
                                               "{\n"
                                               "    vec3 direction;\n"
                                               "};\n"
                                               "\n"
                                               "struct PointLight\n"
                                               "{\n"
                                               "    vec3 position;\n"
                                               "    Attenuation attenuation;\n"
                                               "};\n"
                                               "\n"
                                               "struct SpotLight\n"
                                               "{\n"
                                               "    vec3 position;\n"
                                               "    vec3 direction;\n"
                                               "\n"
                                               "    Attenuation attenuation;\n"
                                               "\n"
                                               "    float innerAngle;\n"
                                               "    float outerAngle;\n"
                                               "};\n"
                                               "\n"
                                               "struct Light\n"
                                               "{\n"
                                               "    int type;\n"
                                               "    vec4 color;\n"
                                               "\n"
                                               "    DirectionalLight directional;\n"
                                               "    PointLight point;\n"
                                               "    SpotLight spot;\n"
                                               "};\n"
                                               "uniform Material material;\n"
                                               "uniform Light light;\n"
                                               "\n"
                                               "layout (location = 0) in vec3 in_position;\n"
                                               "layout (location = 1) in vec3 in_texcoord;\n"
                                               "layout (location = 2) in vec3 in_color;\n"
                                               "\n"
                                               "out vec4 out_color;\n"
                                               "\n"
                                               "void main()\n"
                                               "{\n"
                                               "    if (material.tex.hasAlpha == 1 && texture(material.tex.alpha, in_texcoord.st).r < 0.1)\n"
                                               "    {\n"
                                               "        discard;\n"
                                               "    }\n"
                                               "    \n"
                                               "    if ( material.tex.hasKd == 1 )\n"
                                               "    {\n"
                                               "        out_color = vec4(texture(material.tex.kd, in_texcoord.st).rgb, 1);\n"
                                               "    }\n"
                                               "    else\n"
                                               "    {\n"
                                               "        out_color = vec4(in_color.rgb, 1.0);\n"
                                               "    }\n"
                                               "}\n"
                                               );

#else
static const std::string defaultVertexShader("Shaders/Default.vert.glsl");
static const std::string defaultFragmentShader("Shaders/Default.frag.glsl");
#endif

namespace Ra
{
    namespace Engine
    {
        
        ShaderConfiguration ShaderConfiguration::m_defaultShaderConfig("DefaultShader", defaultVertexShader, defaultFragmentShader);
        
        ShaderConfiguration::ShaderConfiguration(const std::string& name)
        : m_name(name), m_version("#version 410")
        {
        }
        
        ShaderConfiguration::ShaderConfiguration(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader)
        : m_name(name), m_version("#version 410")
        {
            m_shaders[ShaderType_VERTEX] = vertexShader;
            m_shaders[ShaderType_FRAGMENT] = fragmentShader;
        }
        
        void ShaderConfiguration::addShader(ShaderType type, const std::string& name)
        {
            m_shaders[type] = name;
        }
        
        void ShaderConfiguration::addProperty(const std::string& prop )
        {
            m_properties.insert( "#define " + prop );
        }
        
        void ShaderConfiguration::addProperties( const std::list<std::string>& props )
        {
            for ( const auto& prop : props )
            {
                m_properties.insert( "#define " + prop );
            }
        }
        
        void ShaderConfiguration::removeProperty( const std::string& prop )
        {
            m_properties.erase( "#define " + prop );
        }

        void ShaderConfiguration::addInclude(const std::string& incl, ShaderType type )
        {
            m_includes.emplace_back( "#include " + incl, type );
        }

        void ShaderConfiguration::addIncludes(const std::list<std::string>& incls, ShaderType type )
        {
            for ( const auto& incl : incls )
            {
                m_includes.emplace_back( "#include " + incl, type );
            }
        }

        void ShaderConfiguration::removeInclude(const std::string& incl, ShaderType type)
        {
            // TODO (Hugo)
            //m_properties.erase( "#include " + prop );
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
                    if ( m_includes.size() == o.m_includes.size() )
                    {
                        if ( m_includes.size() == 0 )
                        {
                            res = false;
                        }
                        else
                        {
                            auto lit = m_includes.begin();
                            auto rit = o.m_includes.begin();

                            for ( ; ( lit != m_includes.end() ) && ( *lit == *rit ); ++lit, ++rit );

                            if ( lit == m_includes.end() )
                            {
                                res = false;
                            }
                            else
                            {
                                res = *lit < *rit;
                            }
                        }
                    }
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

        const std::vector< std::pair<std::string, ShaderType> >& ShaderConfiguration::getIncludes() const
        {
            return m_includes;
        }
        
    }
} // namespace Ra
