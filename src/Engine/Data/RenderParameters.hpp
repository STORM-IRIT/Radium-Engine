#pragma once

#include <Engine/RaEngine.hpp>

#include <vector>

#include <nlohmann/json.hpp>

#include <Core/Types.hpp>
#include <Core/Utils/BijectiveAssociation.hpp>
#include <Core/Utils/Color.hpp>
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
 * enum with type Enum, use `getParameterSet<TParameter<typename std::underlying_type<typename
 * Enum>::type>>`
 *
 */
class RA_ENGINE_API RenderParameters final
{
  public:
    /**
     * \brief Special type for Texture parameter
     */
    using TextureInfo = std::pair<Data::Texture*, int>;

    /// \brief Aliases for bindable parameter types
    /// \{
    /// Parameter of type bool
    using BoolParameter = bool;
    /// Parameter of type int
    using IntParameter = int;
    /// Parameter of type unsigned int
    using UIntParameter = uint;
    /// Parameter of type Scalar
    using ScalarParameter = Scalar;
    /// Parameter of Type TextureInfo (Texture pointer + texture unit)
    using TextureParameter = TextureInfo;
    /// Parameter of type vector of int
    using IntsParameter = std::vector<int>;
    /// Parameter of type vector of unsigned int
    using UIntsParameter = std::vector<uint>;
    /// Parameter of type vector of Scalar;
    using ScalarsParameter = std::vector<Scalar>;
    /// Parameter of type Vector2
    using Vec2Parameter = Core::Vector2;
    /// Parameter of type Vector3
    using Vec3Parameter = Core::Vector3;
    /// Parameter of type Vector4
    using Vec4Parameter  = Core::Vector4;
    using ColorParameter = Core::Utils::Color;
    /// Parameter of type Matrix2
    using Mat2Parameter = Core::Matrix2;
    /// Parameter of type Matrix3
    using Mat3Parameter = Core::Matrix3;
    /// Parameter of type Matrix3
    using Mat4Parameter = Core::Matrix4;

    /// List of bindable types, to be used with static visitors
    using BindableTypes = Core::Utils::TypeList<BoolParameter,
                                                ColorParameter,
                                                IntParameter,
                                                UIntParameter,
                                                ScalarParameter,
                                                TextureParameter,
                                                IntsParameter,
                                                UIntsParameter,
                                                ScalarsParameter,
                                                Vec2Parameter,
                                                Vec3Parameter,
                                                Vec4Parameter,
                                                Mat2Parameter,
                                                Mat3Parameter,
                                                Mat4Parameter,
                                                std::reference_wrapper<RenderParameters>>;
    /// \}

    /** Set of typed parameters
     * For a given shader Program, all the parameters are stored by type, using Core::VariableSet as
     * container.
     *
     * \tparam T The type of parameteres in the set.
     */
    template <typename T>
    using UniformBindableSet = Core::VariableSet::VariableContainer<T>;

  public:
    /**
     * \brief Management of parameter of enum type.
     * This allow to set the parameter using a string representation of their value.
     * Setting the parameter directly from the value is supported as for any other parameter but
     * user should take care to call the right overloaded function given the underlying enumeration
     * type. This is due to unscoped enum being implicitly convertible to any integral type.
     * (https://en.cppreference.com/w/cpp/language/enum)
     * \{
     */
    /**
     * \brief This class allows to set and manipulate parameter as enumerations either using a
     * string representation of the enumeration or its value.
     */
    class AbstractEnumConverter
    {
      public:
        virtual ~AbstractEnumConverter() = default;
        /**
         * \brief Set the value of the enum corresponding to the given string and matching the
         * enumeration underlying type.
         * \param p the renderParameter to update.
         * \param name The name of the enum to set.
         * \param enumerator The enumerator, in std::string form.
         */
        virtual void setEnumValue( RenderParameters& p,
                                   const std::string& name,
                                   const std::string& enumerator ) const = 0;

        /**
         * \brief Get the string form of an enumeration value.
         * \param v the value of the enumeration, implicitly converted to int for unscope enum,
         * explicitely converted for scoped one.
         * \return the string associated to this value
         */
        virtual std::string getEnumerator( int v ) const = 0;

        /**
         * \brief Get all the string forms of the enumeration.
         * \return the vector of strings associated to the enumeration.
         */
        virtual std::vector<std::string> getEnumerators() const = 0;

        /**
         * \brief Get the value, converted to int, of the enumeration given its string expression
         * \param v the string defining the enumerator
         * \return the value of the enumerator
         * \note  This method does not respect the underlying type of enumerations as it returns
         * always an int. Use setEnumValue to modify the parameter set in a type safe way.
         */
        virtual int getEnumerator( const std::string& v ) const = 0;
    };

    /**
     * \brief This class manage the bijective association between string and integral representation
     * of an enumeration.
     *
     * \tparam Enum the type of the enumeration to manage
     */
    template <typename Enum>
    class EnumConverter : public AbstractEnumConverter
    {
      public:
        using EnumBaseType = typename std::underlying_type_t<Enum>;
        explicit EnumConverter( std::initializer_list<std::pair<EnumBaseType, std::string>> pairs );

        void setEnumValue( RenderParameters& p,
                           const std::string& name,
                           const std::string& enumerator ) const override;
        std::string getEnumerator( int v ) const override;
        int getEnumerator( const std::string& v ) const override;
        std::vector<std::string> getEnumerators() const override;

      private:
        Core::Utils::BijectiveAssociation<typename std::underlying_type_t<Enum>, std::string>
            m_valueToString;
    };

    /**
     * \brief Associate a converter for enumerated type to the given parameter name
     * \param name
     * \param converter
     */
    void addEnumConverter( const std::string& name,
                           std::shared_ptr<AbstractEnumConverter> converter );

    /**
     * \brief Search for a converter associated with an enumeration parameter
     * \param name the name of the parameter
     * \return an optional containing the converter or false if no converter is found.
     */
    Core::Utils::optional<std::shared_ptr<AbstractEnumConverter>>
    getEnumConverter( const std::string& name );

    /**
     * \brief Return the string associated to the actual value of a parameter
     * \param name
     * \param
     * \return
     */
    std::string getEnumString( const std::string& name, int value );

  private:
    /**
     * \brief Store the enumeration converter.
     * By storing a shared_ptr, the same converter could be used for several parameters.
     */
    std::map<std::string, std::shared_ptr<AbstractEnumConverter>> m_enumConverters;

    /**\}*/

  public:
    /**
     * Overloaded operators to set shader parameters
     * \{
     */

    template <typename T>
    void addParameter( const std::string& name,
                       T value,
                       typename std::enable_if<!std::is_class<T> {}, bool>::type = true );

    template <typename T, typename std::enable_if<std::is_class<T> {}, bool>::type = true>
    void addParameter( const std::string& name, const T& value );

    /**
     * Adding a texture parameter.
     * The default (-1) for the texUnit parameter implies automatic uniform binding for the
     * texture unit associated with the named sampler.
     * If texUnit is given, then uniform binding will be made at this explicit location.
     */
    template <typename T,
              typename std::enable_if<std::is_base_of<Data::Texture, T>::value, bool>::type = true>
    void addParameter( const std::string& name, T* tex, int texUnit = -1 );

    /**
     * \brief set the value of the given parameter, according to a string representation of an enum.
     * \note If there is no EnumConverter associated with the parameter name, do nothing.
     * \param name
     * \param value
     */
    void addParameter( const std::string& name, const std::string& value );
    void addParameter( const std::string& name, const char* value );

    /**
     * \brief add a render parameter variable
     * \param name
     * \param value
     */
    void addParameter( const std::string& name, RenderParameters& value );

    /**\}*/

    /**
     * Merges a RenderParameters \a params with this
     * \param params the render parameter to merge with the current.
     * Existing parameter value are kept from this
     * \see mergeReplaceParameters
     */
    void mergeKeepParameters( const RenderParameters& params );

    /**
     * Merges a RenderParameters \a params with this
     * \param params the render parameter to merge with the current.
     * Existing parameter values are replaced by params's one.
     * \see mergeKeepParameters
     */
    void mergeReplaceParameters( const RenderParameters& params );

    /** Bind the parameter uniform on the shader program
     * \note, this will only bind the supported parameter types.
     * \param shader The shader to bind to.
     */
    void bind( const Data::ShaderProgram* shader ) const;

    /**
     * Get a typed parameter set
     * \tparam T the type of the parameter set to get
     * \return The corresponding set parameter
     */
    template <typename T>
    const UniformBindableSet<T>& getParameterSet() const;

    /**
     * \brief Test if parameters of type T are stored
     * \tparam T
     * \return an optional, empty if the ParameterSet does not exists or whose value is
     * a **non owning** pointer to the ParameterSet collection if it exists. This **non owning**
     * pointer remains valid as long as the RenderParameter exists and contains the given type.
     */
    template <typename T>
    std::optional<UniformBindableSet<T>*> hasParameterSet() const;

    /**
     * Check if a typed parameter exists
     * \tparam T the type of the parameter to get
     * \param name The name of the parameter to get
     * \return true if the parameter exists
     */
    template <typename T>
    bool containsParameter( const std::string& name ) const;

    /**
     * Get a typed parameter
     * \tparam T the type of the parameter to get
     * \param name The name of the parameter to get
     * \return The corresponding parameter
     * \throw std::out_of_range if the container does not have an parameter with the specified name
     */
    template <typename T>
    const T& getParameter( const std::string& name ) const;

    /** Visit the parameter using any kind of visitor
     */
    template <typename V>
    void visit( V& visitor ) const;

    template <typename V, typename T>
    void visit( V& visitor, T& userParams ) const;

    template <typename V, typename T>
    void visit( V& visitor, T&& userParams ) const;

    /// \brief Get access to the parameter storage
    /// \return the Core::VariableSet storing the parameters
    /// \{
    const Core::VariableSet& getStorage() const { return m_parameterSets; }
    Core::VariableSet& getModifiableStorage() const {
        return const_cast<Core::VariableSet&>( m_parameterSets );
    }
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
            else {
                shader->setUniform( name.c_str(), tex, texUnit );
            }
        }

        /**
         * \brief Binds a embedded RenderParameter.
         * This allows to build hierarchies of RenderParameters.
         */
        void operator()( const std::string& /*name*/,
                         const std::reference_wrapper<RenderParameters>& p,
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

    /**
     * Storage of the parameters
     */
    Core::VariableSet m_parameterSets;
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

template <typename Enum>
inline RenderParameters::EnumConverter<Enum>::EnumConverter(
    std::initializer_list<std::pair<typename std::underlying_type_t<Enum>, std::string>> pairs ) :
    AbstractEnumConverter(), m_valueToString { pairs } {}

template <typename Enum>
inline void
RenderParameters::EnumConverter<Enum>::setEnumValue( RenderParameters& p,
                                                     const std::string& name,
                                                     const std::string& enumerator ) const {
    auto v = m_valueToString.key( enumerator );
    p.addParameter( name, static_cast<Enum>( v ) );
}

template <typename Enum>
inline std::string RenderParameters::EnumConverter<Enum>::getEnumerator( int v ) const {
    return m_valueToString( std::underlying_type_t<Enum>( v ) );
}

template <typename Enum>
inline int RenderParameters::EnumConverter<Enum>::getEnumerator( const std::string& v ) const {
    return m_valueToString.key( v );
}

template <typename Enum>
std::vector<std::string> RenderParameters::EnumConverter<Enum>::getEnumerators() const {
    std::vector<std::string> keys;
    keys.reserve( m_valueToString.size() );
    for ( const auto& p : m_valueToString ) {
        keys.push_back( p.second );
    }
    return keys;
}

template <typename T>
inline void
RenderParameters::addParameter( const std::string& name,
                                T value,
                                typename std::enable_if<!std::is_class<T> {}, bool>::type ) {
    if constexpr ( std::is_enum<T>::value ) {
        auto v = static_cast<typename std::underlying_type<T>::type>( value );
        m_parameterSets.insertOrAssignVariable( name, v );
    }
    else {
        m_parameterSets.insertOrAssignVariable( name, value );
    }
}

template <typename T, typename std::enable_if<std::is_class<T> {}, bool>::type>
inline void RenderParameters::addParameter( const std::string& name, const T& value ) {
    m_parameterSets.insertOrAssignVariable( name, value );
}

template <typename T, typename std::enable_if<std::is_base_of<Data::Texture, T>::value, bool>::type>
inline void RenderParameters::addParameter( const std::string& name, T* tex, int texUnit ) {
    addParameter( name, TextureInfo { tex, texUnit } );
}

inline void RenderParameters::addParameter( const std::string& name, RenderParameters& value ) {
    m_parameterSets.insertOrAssignVariable( name, std::ref( value ) );
}

template <typename T>
inline const RenderParameters::UniformBindableSet<T>& RenderParameters::getParameterSet() const {
    return m_parameterSets.getAllVariables<T>();
}

template <typename T>
inline std::optional<RenderParameters::UniformBindableSet<T>*>
RenderParameters::hasParameterSet() const {
    if constexpr ( std::is_enum<T>::value ) {
        // Do not return
        // m_parameterSets.existsVariableType< typename std::underlying_type< T >::type >();
        // to prevent misusage of this function. The user should infer this with another logic.
        return {};
    }
    else {
        return m_parameterSets.existsVariableType<T>();
    }
}

template <typename T>
inline bool RenderParameters::containsParameter( const std::string& name ) const {
    if constexpr ( std::is_enum<T>::value ) {
        return m_parameterSets.existsVariable<typename std::underlying_type<T>::type>( name );
    }
    else {
        return m_parameterSets.existsVariable<T>( name );
    }
}

template <typename T>
inline const T& RenderParameters::getParameter( const std::string& name ) const {
    if constexpr ( std::is_enum<T>::value ) {
        // need to cast to take into account the way enums are managed in the RenderParameters
        return reinterpret_cast<const T&>(
            m_parameterSets.getVariable<typename std::underlying_type<T>::type>( name ) );
    }
    else {
        return m_parameterSets.getVariable<T>( name );
    }
}

template <typename V>
void RenderParameters::visit( V& visitor ) const {
    if constexpr ( std::is_base_of<Core::VariableSet::DynamicVisitorBase, V>::value ) {
        m_parameterSets.visitDynamic( visitor );
    }
    else {
        m_parameterSets.visit( visitor );
    }
}

template <typename V, typename T>
void RenderParameters::visit( V& visitor, T& userParams ) const {
    if constexpr ( std::is_base_of<Core::VariableSet::DynamicVisitorBase, V>::value ) {
        m_parameterSets.visitDynamic( visitor, std::forward<T&>( userParams ) );
    }
    else {
        m_parameterSets.visit( visitor, std::forward<T&>( userParams ) );
    }
}

template <typename V, typename T>
void RenderParameters::visit( V& visitor, T&& userParams ) const {
    if constexpr ( std::is_base_of<Core::VariableSet::DynamicVisitorBase, V>::value ) {
        m_parameterSets.visitDynamic( visitor, std::forward<T&&>( userParams ) );
    }
    else {
        m_parameterSets.visit( visitor, std::forward<T&&>( userParams ) );
    }
}

} // namespace Data
} // namespace Engine
} // namespace Ra
