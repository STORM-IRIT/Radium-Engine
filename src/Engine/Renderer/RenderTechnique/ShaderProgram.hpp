#ifndef RADIUMENGINE_SHADERPROGRAM_HPP
#define RADIUMENGINE_SHADERPROGRAM_HPP

#include <Engine/RaEngine.hpp>

#include <set>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <memory>

#include <Core/CoreMacros.hpp>
#include <Core/Math/LinearAlgebra.hpp>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>

namespace globjects
{
    class File;
    class Shader;
    class Program;
    class NamedString;
}

namespace Ra
{
    namespace Engine
    {
        class Texture;

        class RA_ENGINE_API ShaderProgram
        {
        public:
            ShaderProgram();
            explicit ShaderProgram( const ShaderConfiguration& shaderConfig );
            virtual ~ShaderProgram();

            void load( const ShaderConfiguration& shaderConfig );
            void reload();

            ShaderConfiguration getBasicConfiguration() const;

            void bind() const;
            void validate() const;
            void unbind() const;

            // Uniform setters
            void setUniform( const char* name, int value ) const;
            void setUniform( const char* name, uint value ) const;
            void setUniform( const char* name, float value ) const;
            void setUniform( const char* name, double value ) const;
            
            void setUniform( const char* name, std::vector<int> value ) const;
            void setUniform( const char* name, std::vector<uint> value ) const;
            void setUniform( const char* name, std::vector<float> value ) const;

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

            void setUniform( const char* name, Texture* tex, int texUnit ) const;

            globjects::Program * getProgramObject() const;

        private:
            void loadShader(ShaderType type, const std::string& name, const std::set<std::string>& props);

            GLenum getTypeAsGLEnum(ShaderType type) const;
            ShaderType getGLenumAsType(GLenum type) const;

            void link();

            std::string preprocessIncludes(const std::string &name, const std::string& shader, int level, int line=0);

        private:
            ShaderConfiguration m_configuration;

            std::array< std::unique_ptr<globjects::Shader>, ShaderType_COUNT > m_shaderObjects;

            std::unique_ptr<globjects::Program> m_program;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SHADERPROGRAM_HPP
