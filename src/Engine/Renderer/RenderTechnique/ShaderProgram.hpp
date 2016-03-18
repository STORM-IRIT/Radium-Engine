#ifndef RADIUMENGINE_SHADERPROGRAM_HPP
#define RADIUMENGINE_SHADERPROGRAM_HPP

#include <Engine/RaEngine.hpp>

#include <set>
#include <string>
#include <array>

#include <Core/CoreMacros.hpp>
#include <Core/Math/LinearAlgebra.hpp>

#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>

namespace Ra
{
    namespace Engine
    {
        class Texture;
    }
}

namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API ShaderObject
        {
        public:
            ShaderObject();
            ~ShaderObject();

            bool loadAndCompile( uint type,
                                 const std::string& filename,
                                 const std::set<std::string>& properties );

            bool reloadAndCompile( const std::set<std::string>& properties );

            uint getId() const;

        private:
            bool parseFile( const std::string& filename, std::string& content );
            std::string load();
            void compile( const std::string& shader, const std::set<std::string>& properties );
            bool check();

        private:
            uint m_id;
            std::string m_filename;
            uint m_type;
            std::set<std::string> m_properties;
        };

        class RA_ENGINE_API ShaderProgram
        {
        public:
            // Todo : remove duplicate flag in ShaderConfig
            enum ShaderType
            {
                VERT_SHADER = 0,
                FRAG_SHADER,
                GEOM_SHADER,
                TESC_SHADER,
                TESE_SHADER,
                COMP_SHADER,
                SHADER_TYPE_COUNT
            };

        public:
            ShaderProgram();
            explicit ShaderProgram( const ShaderConfiguration& shaderConfig );
            virtual ~ShaderProgram();

            void load( const ShaderConfiguration& shaderConfig );
            void reload();

            bool isOk() const;

            ShaderConfiguration getBasicConfiguration() const;
            //void addProperty(const std::string& property);
            //void delProperty(const std::string& property);
            //void removeAllProperties() { m_properties.clear(); }

            void bind();
            //void bind(const RenderParameters& params);
            void unbind();

            uint getId() const;

            // Uniform setters
            void setUniform( const char* name, int value ) const;
            void setUniform( const char* name, uint value ) const;
            void setUniform( const char* name, float value ) const;
            void setUniform( const char* name, double value ) const;

            void setUniform( const char* name, const Core::Vector2f& value ) const;
            void setUniform( const char* name, const Core::Vector2d& value ) const;
            void setUniform( const char* name, const Core::Vector3f& value ) const;
            void setUniform( const char* name, const Core::Vector3d& value ) const;
            void setUniform( const char* name, const Core::Vector4f& value ) const;
            void setUniform( const char* name, const Core::Vector4d& value ) const;

            void setUniform( const char* name, const Core::Matrix2f& value ) const;
            void setUniform( const char* name, const Core::Matrix2d& value ) const;
            void setUniform( const char* name, const Core::Matrix3f& value ) const;
            void setUniform( const char* name, const Core::Matrix3d& value ) const;
            void setUniform( const char* name, const Core::Matrix4f& value ) const;
            void setUniform( const char* name, const Core::Matrix4d& value ) const;

            // TODO (Charly) : Add Texture support
            void setUniform( const char* name, Texture* tex, int texUnit ) const;

        private:
            //  bool exists(const std::string& filename);
            void loadShader(ShaderType type, const std::string& name, const std::set<std::string>& props);
            uint getTypeAsGLEnum(ShaderType type) const;
            std::string getExtensionGivenType(ShaderType type) const;

            void link();

        private:
            ShaderConfiguration m_configuration;
            uint m_shaderId;
            std::array<ShaderObject*, SHADER_TYPE_COUNT> m_shaderObjects;
            std::array<bool, SHADER_TYPE_COUNT> m_shaderStatus;

            bool m_binded;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SHADERPROGRAM_HPP
