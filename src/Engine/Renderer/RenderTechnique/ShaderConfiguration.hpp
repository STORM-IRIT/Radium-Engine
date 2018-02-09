#ifndef RADIUMENGINE_SHADERCONFIGURATION_HPP
#define RADIUMENGINE_SHADERCONFIGURATION_HPP

#include <Engine/RaEngine.hpp>

#include <set>
#include <string>
#include <array>
#include <list>

namespace Ra
{
    namespace Engine
    {
        
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
        
        /// A struct used to create shader programs later on.
        /// A ShaderConfiguration should be added once to the ShaderConfigurationFactory,
        /// then the factory must be used to retrieve the added shader configurations.
        /// Typical use case :
        ///     /**************************** CREATION ****************************/
        ///     // Create the shader configuration once (see MainApplication::addBasicShaders for example)
        ///     ShaderConfiguration config("MyConfig");
        ///     // Add shader files to the config (note that the file extensions can be whatever you want)
        ///     config.addShader(ShaderType_VERTEX, "path/to/shader.vert.glsl");
        ///     config.addShader(ShaderType_FRAGMENT, "path/to/shader.frag.glsl");
        ///     // Same for other shader types. Vertex and fragment are required, other are optional
        ///     // Note that, for a compute shader, only the compute shader is needed.
        ///
        ///     // Add the configuration to the factory
        ///     ShaderConfigurationFactory::addConfiguration(config);
        ///
        ///     /**************************** USAGE ****************************/
        ///     // When you want to reuse created shader configurations, just set it using
        ///     auto config = ShaderConfigurationFactory::getConfiguration("MyConfig");
        ///     // You can then pass it to createRenderObject for example
        ///     createRenderObject(name, component, RenderObjectType::Fancy, mesh, config, material);
        class RA_ENGINE_API ShaderConfiguration
        {
            friend class ShaderProgram;

        public:
            ShaderConfiguration() = default;
            /// Initializes a shader configuration with a name
            /// Warning: This does not query the corresponding configuration in the ShaderConfigurationFactory
            /// The proper way to do this is by calling
            /// ShaderConfigurationFactory::getConfiguration(name);
            ShaderConfiguration(const std::string& name);
            
            /// Initializes a configuration with a name, a vertex and a fragment shader
            /// This does not add the configuration to the factory
            /// ShaderConfigurationFactory::addConfiguration(config) must be called.
            ShaderConfiguration(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader);
            
            // Add a shader given its type
            void addShader(ShaderType type, const std::string& name);
            
            /// Add a property in the form of a #define
            /// The same shader files with different properties leads to different shader programs
            void addProperty(const std::string& prop);
            void addProperties(const std::list<std::string>& props);
            void removeProperty(const std::string& prop);

            /// Add a property in the form of an #include
            /// The same shader files with different properties leads to different shader programs
            void addInclude(const std::string& prop);
            void addIncludes(const std::list<std::string>& props);
            void removeInclude(const std::string& prop);
            
            /// Tell if a shader configuration has at least a vertex and a fragment shader, or a compute shader.
            bool isComplete() const;
            
            bool operator< (const ShaderConfiguration& other) const;
            
            std::set<std::string> getProperties() const;
            
            // get default shader configuration
            static ShaderConfiguration getDefaultShaderConfig() { return m_defaultShaderConfig; }
            
        public:
            std::string m_name;

        private:
            std::array<std::string, ShaderType_COUNT> m_shaders;

            std::set<std::string> m_properties;
            
            static ShaderConfiguration m_defaultShaderConfig;
        };
        
    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SHADERCONFIGURATION_HPP
