#ifndef RADIUMENGINE_SHADERPROGRAM_HPP
#define RADIUMENGINE_SHADERPROGRAM_HPP

#include <Engine/RaEngine.hpp>

#include <set>
#include <string>
#include <array>
#include <list>

#include <Core/CoreMacros.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Engine
    {
        class Texture;

        enum ShaderType : uint
        {
            ShaderType_VERTEX = 0,
            ShaderType_FRAGMENT,
            ShaderType_GEOMETRY,
            ShaderType_TESS_CONTROL,
            ShaderType_TESS_EVALUATION,
            ShaderType_COMPUTE,
            ShaderType_COUNT
        };

        // finding GLSL errors after .glsl includes
        enum LineFound : uint
        {
            NOT_FOUND = 0,
            FOUND_INSIDE,
            FOUND_OUTSIDE,
        };

        // a node representing one file, each include is a leaf
        struct LineErr {
            std::string name;
            uint start;
            uint end;
            std::vector<struct LineErr> subfiles;
        };

        class RA_ENGINE_API ShaderConfiguration
        {
            friend class ShaderProgram;

        public:
            ShaderConfiguration() = default;
            ShaderConfiguration(const std::string& name);
            ShaderConfiguration(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader);

            // Add a shader given its type
            void addShader(ShaderType type, const std::string& name);

            void addProperty(const std::string& prop);
            void addProperties(const std::list<std::string>& props );
            void removeProperty(const std::string& prop);

            /// Tell if a shader configuration has at least a vertex and a fragment shader, or a compute shader.
            /// @todo: Is a shader complete if it only has a compute shader ?
            bool isComplete() const;

            bool operator< (const ShaderConfiguration& other) const;

            std::set<std::string> getProperties() const;

        public:
            std::string m_name;

        private:
            std::array<std::string, ShaderType_COUNT> m_shaders;
            std::set<std::string> m_properties;
        };

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

            // return an error message with wrong file and line - offset
            std::string lineFind(uint line) const;

        private:
            bool parseFile( const std::string& filename, std::string& content );
            std::string load();
            /// @param level Prevent cyclic includes
            std::string preprocessIncludes(const std::string& shader, int level, struct LineErr& lerr);
            void compile( const std::string& shader, const std::set<std::string>& properties );
            bool check();
            bool lineInside(const struct LineErr* node, uint line) const;
            uint lineParseGLMesg(const std::string& msg) const;
            void linePrint(const struct LineErr* node, uint level = 0) const;
            std::string lineFind(const struct LineErr* node, uint line) const;

        private:
            uint m_id;
            std::string m_filename;
            std::string m_filepath;
            uint m_type;
            std::set<std::string> m_properties;
            struct LineErr m_lineerr;
        };

        class RA_ENGINE_API ShaderProgram
        {
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

            void bind() const;
            //void bind(const RenderParameters& params);
            void unbind() const;

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

            void setUniform( const char* name, Texture* tex, int texUnit ) const;

        private:
            //  bool exists(const std::string& filename);
            void loadShader(ShaderType type, const std::string& name, const std::set<std::string>& props);
            uint getTypeAsGLEnum(ShaderType type) const;

            void link();

        private:
            ShaderConfiguration m_configuration;
            uint m_shaderId;
            std::array<ShaderObject*, ShaderType_COUNT> m_shaderObjects;
            std::array<bool, ShaderType_COUNT> m_shaderStatus;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SHADERPROGRAM_HPP
