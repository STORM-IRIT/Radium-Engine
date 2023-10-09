#pragma once

#include <Engine/RaEngine.hpp>

#include <vector>

#include <nlohmann/json.hpp>

#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/EnumConverter.hpp>
#include <Core/Utils/Log.hpp>
#include <Core/Utils/StdOptional.hpp>

#include <Core/Containers/VariableSet.hpp>

#include <Engine/Data/ShaderProgram.hpp>

namespace Ra {
namespace Engine {
namespace Data {

class Texture;

/**
 * Management of shader parameters with automatic binding of a named parameter to the corresponding
 * glsl uniform.
 * \note Automatic binding is only available for supported type described in BindableTypes.
 * \note Enums are stored according to their underlying_type. Enum management is automatic except
 * when requesting for the associated uniformBindableSet. To access bindable set containing a given
 * enum with type Enum, use `getAllVariable<typename std::underlying_type<typename
 * Enum>::type>`
 *
 */
class RA_ENGINE_API RenderParameters final : public Core::VariableSet
{
  public:
    /**
     * \brief Special type for Texture parameter
     */
    using TextureInfo = std::pair<Data::Texture*, int>;

    /// \brief Aliases for bindable parameter types
    /// \{

    /// List of bindable types, to be used with static visitors
    using BindableTypes = Core::Utils::TypeList<bool,
                                                Core::Utils::Color,
                                                int,
                                                uint,
                                                Scalar,
                                                TextureInfo,
                                                std::vector<int>,
                                                std::vector<uint>,
                                                std::vector<Scalar>,
                                                Core::Vector2,
                                                Core::Vector3,
                                                Core::Vector4,
                                                Core::Matrix2,
                                                Core::Matrix3,
                                                Core::Matrix4,
                                                std::reference_wrapper<RenderParameters>,
                                                std::reference_wrapper<const RenderParameters>>;

    /** Set of typed parameters
     * For a given shader Program, all the parameters are stored by type, using Core::VariableSet as
     * container.
     *
     * \tparam T The type of parameters in the set.
     */
    template <typename T>
    using UniformBindableSet = Core::VariableSet::VariableContainer<T>;

    /** Handle to a bindable parameters.
     *  A handle is an iterator on a pair <name, value> such that the value is of type T
     *
     * \tparam T The type of parameter in the set.
     */
    template <typename T>
    using UniformVariable = Core::VariableSet::VariableHandle<T>;
    /// \}

    /**
     * Overloaded operators to set shader parameters
     * \{
     */

    using VariableSet::setVariable;

    /**
     * \brief Adding a texture parameter.
     * \tparam T The type of parameter to add. Must be derived from Texture for this overload.
     * \param name Name of the parameter
     * \param tex Texture to add in the parameterSet
     * \param texUnit Texture unit associated with the texture object.
     * The default (-1) for the texUnit parameter implies automatic uniform binding for the
     * texture unit associated with the named sampler.
     * If texUnit is given, then uniform binding will be made at this explicit location.
     */
    template <typename T,
              typename std::enable_if<std::is_base_of<Data::Texture, T>::value, bool>::type = true>
    void setTexture( const std::string& name, T* tex, int texUnit = -1 ) {
        setVariable( name, TextureInfo { tex, texUnit } );
    }

    /**\}*/

    /** Bind the parameter uniform on the shader program
     * \note, this will only bind the supported parameter types.
     * \param shader The shader to bind to.
     */
    void bind( const Data::ShaderProgram* shader ) const;

    /**
     * \brief Test if parameters of type T are stored
     * \tparam T
     * \return an optional, empty if the ParameterSet does not exists or whose value is
     * a **non owning** pointer to the ParameterSet collection if it exists. This **non owning**
     * pointer remains valid as long as the RenderParameter exists and contains the given type.
     */
    template <typename T>
    Core::Utils::optional<UniformBindableSet<T>*> existsVariableType() const;

    /**
     * Check if a typed parameter exists
     * \tparam T the type of the parameter to get
     * \param name The name of the parameter to get
     * \return true if the parameter exists
     */
    template <typename T>
    Core::Utils::optional<UniformVariable<T>> existsVariable( const std::string& name ) const;

    /**
     * \brief Get a typed parameter
     * \tparam T the type of the parameter to get
     * \param name The name of the parameter to get
     * \return The corresponding parameter
     * \throw std::out_of_range if the container does not have an parameter with the specified name
     */
    /// \{
    template <typename T>
    const T& getParameter( const std::string& name ) const;
    template <typename T>
    T& getParameter( const std::string& name );
    /// \}

  private:
    /**
     * \brief Static visitor to bind the stored parameters.
     * \note Binds only statically supported types. To bind unsupported types, use a custom
     * dynamic visitor and the RenderParameter::visit method.
     */
    class StaticParameterBinder
    {
      public:
        /// Type supported by the binder
        using types = BindableTypes;

        /**
         * \brief Binds a color parameter as this requires special access to the parameter value.
         */
        void operator()( const std::string& name,
                         const Ra::Core::Utils::Color& p,
                         const Data::ShaderProgram* shader ) {
            shader->setUniform( name.c_str(), Ra::Core::Utils::Color::VectorType( p ) );
        }

        /**
         * \brief Binds a Texture parameter as this requires special access to the parameter value.
         */
        void operator()( const std::string& name,
                         const RenderParameters::TextureInfo& p,
                         const Data::ShaderProgram* shader ) {
            auto [tex, texUnit] = p;
            if ( texUnit == -1 ) { shader->setUniformTexture( name.c_str(), tex ); }
            else { shader->setUniform( name.c_str(), tex, texUnit ); }
        }

        /**
         * \brief Binds a embedded const RenderParameter.
         * This allows to build hierarchies of RenderParameters.
         */
        template <typename T>
        void operator()( const std::string& /*name*/,
                         const std::reference_wrapper<T>& p,
                         const Data::ShaderProgram* shader ) {
            p.get().bind( shader );
        }

        /**
         * \brief Bind any type of parameter that do not requires special access
         */
        template <typename T>
        void operator()( const std::string& name, const T& p, const Data::ShaderProgram* shader ) {
            shader->setUniform( name.c_str(), p );
        }
    };

    /** \brief Functor to bind parameters
     * There will be only one StaticParameterBinder used by all RenderParameters
     */
    static StaticParameterBinder s_binder;
};

/**
 * Interface to define metadata (constraints, description, ...) for the edition of parameter set
 */
class RA_ENGINE_API ParameterSetEditingInterface
{
  public:
    virtual ~ParameterSetEditingInterface() = default;

    /**
     * \brief Get a json containing metadata about the parameters.
     *
     * \return the metadata in json format
     */
    virtual nlohmann::json getParametersMetadata() const = 0;

    /// Load the ParameterSet  description
    static void loadMetaData( const std::string& basename, nlohmann::json& destination );
};

/**
 * Shader program parameter provider.
 * a ShaderParameterProvider is an object that is associated to a render technique to provide the
 * uniform parameter set for the program. When an RenderObject is drawn using a given
 * rendertechnique, the ShaderParameterProvider associated to the renderTechnique is responsible to
 * set all the uniforms needed by the rendertechnique.
 */
class RA_ENGINE_API ShaderParameterProvider
{
  public:
    virtual ~ShaderParameterProvider() = default;
    virtual RenderParameters& getParameters() { return m_renderParameters; }
    virtual const RenderParameters& getParameters() const { return m_renderParameters; }
    /**
     * \brief Update the OpenGL states used by the ShaderParameterProvider.
     * These state could be the ones from an associated material (textures, precomputed tables or
     * whatever data associated to the material)  or some parameters that are
     * specific to the provider semantic.
     */
    virtual void updateGL() = 0;

    /**
     * \brief Update the attributes of the ShaderParameterProvider to their actual values stored in
     * the renderParameters.
     */
    virtual void updateFromParameters() {};

    /**
     * \brief Get the list of properties the provider might use in a shader.
     * Each property will be added to the shader used for rendering under the form
     * "#define theProperty" when the provider is associated with the render technique.
     *
     * The default implementation returns an empty list.
     *
     * \todo : Validate this proposal
     */
    virtual std::list<std::string> getPropertyList() const { return {}; };

  private:
    /// The parameters to set for a shader
    /// replace this by coreVariables
    RenderParameters m_renderParameters;
};

template <typename T>
inline Core::Utils::optional<RenderParameters::UniformBindableSet<T>*>
RenderParameters::existsVariableType() const {
    if constexpr ( std::is_enum<T>::value ) {
        // Do not return
        // existsVariableType< typename std::underlying_type< T >::type >();
        // to prevent misuse of this function. The user should infer this with another logic.
        return {};
    }
    else { return Core::VariableSet::existsVariableType<T>(); }
}

template <typename T>
inline Core::Utils::optional<RenderParameters::UniformVariable<T>>
RenderParameters::existsVariable( const std::string& name ) const {
    if constexpr ( std::is_enum<T>::value ) {
        return Core::VariableSet::existsVariable<typename std::underlying_type<T>::type>( name );
    }
    else { return Core::VariableSet::existsVariable<T>( name ); }
}

template <typename T>
inline const T& RenderParameters::getParameter( const std::string& name ) const {
    if constexpr ( std::is_enum<T>::value ) {
        // need to cast to take into account the way enums are managed in the RenderParameters
        return reinterpret_cast<const T&>(
            getVariable<typename std::underlying_type<T>::type>( name ) );
    }
    else { return getVariable<T>( name ); }
}

template <typename T>
inline T& RenderParameters::getParameter( const std::string& name ) {
    return const_cast<T&>( const_cast<const RenderParameters*>( this )->getParameter<T>( name ) );
}

} // namespace Data
} // namespace Engine
} // namespace Ra
