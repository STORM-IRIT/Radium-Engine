#pragma once

#include <Engine/RaEngine.hpp>

#include <set>
#include <vector>

#include <nlohmann/json.hpp>

#include <Core/Containers/AlignedAllocator.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/BijectiveAssociation.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/Log.hpp>
#include <Core/Utils/StdOptional.hpp>

namespace Ra {
namespace Engine {
namespace Data {

class Texture;
class ShaderProgram;

/**
 * Management of shader parameters with automatic binding of a named parameter to the corresponding
 * glsl uniform.
 */
class RA_ENGINE_API RenderParameters final
{
  public:
    /// Base abstract parameter Class
    ///\todo use globjects Uniform directly (which seems to correspond to the
    /// same data/purpose, even if not well documented
    class Parameter
    {
      public:
        Parameter() = default;
        explicit Parameter( const std::string& name ) : m_name( name ) {}
        virtual ~Parameter() = default;
        /** Bind the parameter uniform on the shader program
         *
         * \param shader The shader to bind to.
         */
        virtual void bind( const Data::ShaderProgram* shader ) const = 0;
        /** The name of the parameter.
         * This must correspond to the name of a Uniform in the shader the Parameter is bound to.
         */
        std::string m_name {};
    };

    /** Typed parameter.
     * Define a Parameter with a typed value
     * \tparam T The type of the parameter's value.
     */
    template <typename T>
    class TParameter final : public Parameter
    {
      public:
        using value_type = T;

        TParameter() = default;
        TParameter( const std::string& name, const T& value ) :
            Parameter( name ), m_value( value ) {}
        ~TParameter() override = default;
        void bind( const Data::ShaderProgram* shader ) const override;
        /// The value of the parameter
        T m_value {};
    };

    /**
     * Special case of a Texture parameter
     */
    class TextureParameter final : public Parameter
    {
      public:
        TextureParameter() = default;
        TextureParameter( const std::string& name, Data::Texture* tex, int texUnit ) :
            Parameter( name ), m_texture( tex ), m_texUnit( texUnit ) {}
        ~TextureParameter() override = default;
        void bind( const Data::ShaderProgram* shader ) const override;

        /// The texture object
        Data::Texture* m_texture { nullptr };
        /// The texture unit where to bind the parameter
        int m_texUnit { -1 };
    };

    /** Set of typed parameters
     * For a given shader Program, all the parameters are stored by type, in several parameter sets.
     *
     * \tparam T The type of parameteres in the set.
     */
    template <typename T>
    class UniformBindableSet final
        : public std::map<
              std::string,
              T,
              std::less<std::string>,
              Core::AlignedAllocator<std::pair<const std::string, T>, EIGEN_MAX_ALIGN_BYTES>>
    {
      public:
        /** Bind the whole parameter set to the corresponding shader uniforms
         *
         * \param shader The shader to bind to.
         */
        void bind( const Data::ShaderProgram* shader ) const;
    };

    ///\todo use array of parameter to templated uniform, as in globjects ?
    /// Parameter of type int
    using IntParameter  = TParameter<int>;
    using BoolParameter = TParameter<bool>;
    /// Parameter of type unsigned int
    using UIntParameter = TParameter<uint>;
    /// Parameter of type Scalar
    using ScalarParameter = TParameter<Scalar>;
    /// Parameter of type vector of int
    using IntsParameter = TParameter<std::vector<int>>;
    /// Parameter of type vector of unsigned int
    using UIntsParameter = TParameter<std::vector<uint>>;

    /// Parameter of type vector of Scalar;
    using ScalarsParameter = TParameter<std::vector<Scalar>>;

    /// Parameter of type Vector2
    using Vec2Parameter = TParameter<Core::Vector2>;
    /// Parameter of type Vector3
    using Vec3Parameter = TParameter<Core::Vector3>;
    /// Parameter of type Vector4
    using Vec4Parameter  = TParameter<Core::Vector4>;
    using ColorParameter = TParameter<Core::Utils::Color>;

    /// Parameter of type Matrix2
    using Mat2Parameter = TParameter<Core::Matrix2>;
    /// Parameter of type Matrix3
    using Mat3Parameter = TParameter<Core::Matrix3>;
    /// Parameter of type Matrix3
    using Mat4Parameter = TParameter<Core::Matrix4>;

  public:
    /**
     * \brief Management of parameter of enum trype
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
        explicit EnumConverter(
            std::initializer_list<std::pair<typename std::underlying_type_t<Enum>, std::string>>
                pairs );

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
     * \brief Search for a converter associated with a parameter
     * \param name
     * \return
     */
    Core::Utils::optional<std::shared_ptr<AbstractEnumConverter>>
    containsEnumConverter( const std::string& name );

    /**
     * \brief Return the string associated to the actual value of a parameter
     * \param name
     * \param
     * \return
     */
    std::string getEnumString( const std::string& name, int value );

    /**
     * \brief set the value of the given parameter, according to a string representation of an enum.
     * \note If there is no EnumConverter associated with the parameter name, do nothing.
     * \param name
     * \param value
     */
    void addParameter( const std::string& name, const std::string& value );

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
    void addParameter( const std::string& name, bool value );
    void addParameter( const std::string& name, int value );
    void addParameter( const std::string& name, uint value );
    void addParameter( const std::string& name, Scalar value );

    void addParameter( const std::string& name, const std::vector<int>& values );
    void addParameter( const std::string& name, const std::vector<uint>& values );
    void addParameter( const std::string& name, const std::vector<Scalar>& values );

    void addParameter( const std::string& name, const Core::Vector2& value );
    void addParameter( const std::string& name, const Core::Vector3& value );
    void addParameter( const std::string& name, const Core::Vector4& value );
    void addParameter( const std::string& name, const Core::Utils::Color& value );

    void addParameter( const std::string& name, const Core::Matrix2& value );
    void addParameter( const std::string& name, const Core::Matrix3& value );
    void addParameter( const std::string& name, const Core::Matrix4& value );

    /**
     * Adding a texture parameter.
     * The default (-1) for the texUnit parameter implies automatic uniform binding for the
     * texture unit associated with the named sampler.
     * If texUnit is given, then uniform binding will be made at this explicit location.
     */
    void addParameter( const std::string& name, Data::Texture* tex, int texUnit = -1 );
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
     *
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

  private:
    /**
     * Storage of the parameters
     * \todo : find a way to simplify this (à la Ra::Core::Geometry::AttribArrayGeometry
     */
    ///\{
    UniformBindableSet<BoolParameter> m_boolParamsVector;
    UniformBindableSet<IntParameter> m_intParamsVector;
    UniformBindableSet<UIntParameter> m_uintParamsVector;
    UniformBindableSet<ScalarParameter> m_scalarParamsVector;

    UniformBindableSet<IntsParameter> m_intsParamsVector;
    UniformBindableSet<UIntsParameter> m_uintsParamsVector;
    UniformBindableSet<ScalarsParameter> m_scalarsParamsVector;

    UniformBindableSet<Vec2Parameter> m_vec2ParamsVector;
    UniformBindableSet<Vec3Parameter> m_vec3ParamsVector;
    UniformBindableSet<Vec4Parameter> m_vec4ParamsVector;
    UniformBindableSet<ColorParameter> m_colorParamsVector;

    UniformBindableSet<Mat2Parameter> m_mat2ParamsVector;
    UniformBindableSet<Mat3Parameter> m_mat3ParamsVector;
    UniformBindableSet<Mat4Parameter> m_mat4ParamsVector;

    UniformBindableSet<TextureParameter> m_texParamsVector;
    /**\}*/
};

/**
 * Interface to define metadata (constraints, description, ...) for the edition of parameter set
 */
class RA_ENGINE_API ParameterSetEditionInterface
{
  public:
    virtual ~ParameterSetEditionInterface() = default;

    /**
     * \brief Get a json containing metadata about the parameters.
     *
     * \return the metadata in json format
     */
    virtual nlohmann::json getParametersMetadata() const = 0;
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

  protected:
    /// The parameters to set for a shader
    RenderParameters m_renderParameters;
};

} // namespace Data
} // namespace Engine
} // namespace Ra

#include <Engine/Data/RenderParameters.inl>
