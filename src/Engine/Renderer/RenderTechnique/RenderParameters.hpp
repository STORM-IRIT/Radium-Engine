#ifndef RADIUMENGINE_RENDERPARAMETERS_HPP
#define RADIUMENGINE_RENDERPARAMETERS_HPP

#include <Engine/RaEngine.hpp>

#include <set>
#include <vector>

#include <Core/Containers/AlignedAllocator.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Engine {
class Texture;
class ShaderProgram;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {
/**
 * The RenderParameters class stores all the parameters used as shaders uniforms
 * for object rendering (Material data, Light parameters, transformation matrices...).
 */
class RA_ENGINE_API RenderParameters final {
  public:
    /**
     * The basic parameter interface.
     */
    class Parameter {
      public:
        Parameter() = default;

        explicit Parameter( const char* name ) : m_name( name ) {}

        virtual ~Parameter() = default;
        /**
         * Bind the parameter to the given Shader.
         * The Shader must declare a uniform with the corresponding name.
         */
        virtual void bind( const ShaderProgram* shader ) const = 0;

        /**
         * The name of the parameter, i.e.\ shader variable name.
         */
        const char* m_name{nullptr};
    };

    /**
     * Generic parameter type, can be either integer, scalar, vector of those, vector or matrix.
     */
    template <typename T>
    class TParameter final : public Parameter {
      public:
        TParameter() = default;

        TParameter( const char* name, const T& value ) : Parameter( name ), m_value( value ) {}

        ~TParameter() override = default;

        void bind( const ShaderProgram* shader ) const override;

        /**
         * Value of the parameter.
         */
        T m_value{};
    };

    /**
     * Texture specific parameter.
     */
    class TextureParameter final : public Parameter {
      public:
        TextureParameter() = default;

        TextureParameter( const char* name, Texture* tex, int texUnit ) :
            Parameter( name ),
            m_texture( tex ),
            m_texUnit( texUnit ) {}

        ~TextureParameter() override = default;

        void bind( const ShaderProgram* shader ) const override;

        /**
         * The Texture data.
         */
        Texture* m_texture{nullptr};

        /**
         * The Texture bind location on the GPU.
         */
        int m_texUnit{-1};
    };

    /**
     * Container for the names of parameters of type T.
     */
    template <typename T>
    class UniformBindableVector final
        : public std::map<
              std::string, T, std::less<std::string>,
              Core::AlignedAllocator<std::pair<const std::string, T>, EIGEN_MAX_ALIGN_BYTES>> {
      public:
        /**
         * Bind all the parameters.
         */
        void bind( const ShaderProgram* shader ) const;
    };

    /// \name Convenience typedefs
    /// \{
    using IntParameter = TParameter<int>;
    using UIntParameter = TParameter<uint>;
    using ScalarParameter = TParameter<Scalar>;

    using IntsParameter = TParameter<std::vector<int>>;
    using UIntsParameter = TParameter<std::vector<uint>>;

    // globjects seems to not handle vector of double
    using ScalarsParameter = TParameter<std::vector<float>>;

    using Vec2Parameter = TParameter<Core::Vector2>;
    using Vec3Parameter = TParameter<Core::Vector3>;
    using Vec4Parameter = TParameter<Core::Vector4>;

    using Mat2Parameter = TParameter<Core::Matrix2>;
    using Mat3Parameter = TParameter<Core::Matrix3>;
    using Mat4Parameter = TParameter<Core::Matrix4>;
    /// \}

  public:
    /** \name Adding parameters
     * Add a parameter of the given type, with the given name and value.
     */
    /// \{
    void addParameter( const char* name, int value );

    void addParameter( const char* name, uint value );

    void addParameter( const char* name, Scalar value );

    void addParameter( const char* name, std::vector<int> values );

    void addParameter( const char* name, std::vector<uint> values );

    void addParameter( const char* name, std::vector<Scalar> values );

    void addParameter( const char* name, const Core::Vector2& value );

    void addParameter( const char* name, const Core::Vector3& value );

    void addParameter( const char* name, const Core::Vector4& value );

    void addParameter( const char* name, const Core::Matrix2& value );

    void addParameter( const char* name, const Core::Matrix3& value );

    void addParameter( const char* name, const Core::Matrix4& value );

    void addParameter( const char* name, Texture* tex, int texUnit );
    /// \}

    /** \name Updating parameters
     * Update the value of the parameter of the given type with the given name.
     */
    // FIXME: why add and update ?
    /// \{
    void updateParameter( const char* name, int value );

    void updateParameter( const char* name, uint value );

    void updateParameter( const char* name, Scalar value );

    void updateParameter( const char* name, const Core::Vector2& value );

    void updateParameter( const char* name, const Core::Vector3& value );

    void updateParameter( const char* name, const Core::Vector4& value );

    void updateParameter( const char* name, const Core::Matrix2& value );

    void updateParameter( const char* name, const Core::Matrix3& value );

    void updateParameter( const char* name, const Core::Matrix4& value );
    /// \}

    /**
     * Add all parameters from \p params.
     */
    void concatParameters( const RenderParameters& params );

    /**
     * Bind all the parameters.
     */
    void bind( const ShaderProgram* shader ) const;

    /**
     * Print stat info to the Debug output.
     */
    void print() const {
        using namespace Core::Utils; // log
        for ( const auto& p : m_scalarParamsVector )
        {
            LOG( logDEBUG ) << "  " << p.first << " : " << p.second.m_name;
        }
    }

  private:
    /// \name Parameters
    // Radium V2 : Any way to simplify this a bit ?
    /// \{
    UniformBindableVector<IntParameter> m_intParamsVector;

    UniformBindableVector<UIntParameter> m_uintParamsVector;

    UniformBindableVector<ScalarParameter> m_scalarParamsVector;

    UniformBindableVector<IntsParameter> m_intsParamsVector;

    UniformBindableVector<UIntsParameter> m_uintsParamsVector;

    UniformBindableVector<ScalarsParameter> m_scalarsParamsVector;

    UniformBindableVector<Vec2Parameter> m_vec2ParamsVector;

    UniformBindableVector<Vec3Parameter> m_vec3ParamsVector;

    UniformBindableVector<Vec4Parameter> m_vec4ParamsVector;

    UniformBindableVector<Mat2Parameter> m_mat2ParamsVector;

    UniformBindableVector<Mat3Parameter> m_mat3ParamsVector;

    UniformBindableVector<Mat4Parameter> m_mat4ParamsVector;

    UniformBindableVector<TextureParameter> m_texParamsVector;
    /// \}
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/RenderTechnique/RenderParameters.inl>

#endif // RADIUMENGINE_RENDERPARAMETERS_HPP
