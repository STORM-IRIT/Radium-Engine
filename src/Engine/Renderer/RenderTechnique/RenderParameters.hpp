#ifndef RADIUMENGINE_RENDERPARAMETERS_HPP
#define RADIUMENGINE_RENDERPARAMETERS_HPP

#include <Engine/RaEngine.hpp>

#include <set>
#include <vector>

#include <Core/Containers/AlignedAllocator.hpp>
#include <Core/Log/Log.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Engine {
class Texture;
class ShaderProgram;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

/// The RenderParameters calss stores all the parameters used as shaders uniforms
/// for rendering an abject (Material data, Light parameters, transformation matrices...).
class RA_ENGINE_API RenderParameters final {
  public:
    /// The basic parameter interface.
    class Parameter {
      public:
        Parameter() = default;
        Parameter( const char* name ) : m_name( name ) {}
        virtual ~Parameter() = default;

        /// Bind the parameter to the given Shader.
        /// The Shader must declare a uniform with the corresponding name.
        virtual void bind( const ShaderProgram* shader ) const = 0;

        /// The name of the parameter.
        const char* m_name;
    };

    /// Any basic type parameter, can be either integer, scalar, vector of those, vector or matrix.
    template <typename T>
    class TParameter final : public Parameter {
      public:
        TParameter() = default;
        TParameter( const char* name, const T& value ) : Parameter( name ), m_value( value ) {}
        ~TParameter() = default;
        void bind( const ShaderProgram* shader ) const override;

        /// Value of the parameter.
        T m_value;
    };

    /// Texture specific parameter.
    class TextureParameter final : public Parameter {
      public:
        TextureParameter() = default;
        TextureParameter( const char* name, Texture* tex, int texUnit ) :
            Parameter( name ),
            m_texture( tex ),
            m_texUnit( texUnit ) {}
        ~TextureParameter() = default;
        void bind( const ShaderProgram* shader ) const override;

        /// The Texture data.
        Texture* m_texture;

        /// The Texture bind location on the GPU.
        int m_texUnit;
    };

    /// Container for the names of parameters of type T.
    template <typename T>
    class UniformBindableVector final
        : public std::map<
              std::string, T, std::less<std::string>,
              Core::AlignedAllocator<std::pair<const std::string, T>, RA_DEFAULT_ALIGN>> {
      public:
        /// Bind all the parameters.
        void bind( const ShaderProgram* shader ) const;
    };

    using IntParameter = TParameter<int>;
    using UIntParameter = TParameter<uint>;
    using ScalarParameter = TParameter<Scalar>;

    using IntsParameter = TParameter<std::vector<int>>;
    using UIntsParameter = TParameter<std::vector<uint>>;
    // globjects does not handle vectors of double.
    using ScalarsParameter = TParameter<std::vector<float>>;

    using Vec2Parameter = TParameter<Core::Vector2>;
    using Vec3Parameter = TParameter<Core::Vector3>;
    using Vec4Parameter = TParameter<Core::Vector4>;

    using Mat2Parameter = TParameter<Core::Matrix2>;
    using Mat3Parameter = TParameter<Core::Matrix3>;
    using Mat4Parameter = TParameter<Core::Matrix4>;

  public:
    // Fixme: why add and update ?
    /// Add an integer parameter with the given name and value.
    void addParameter( const char* name, int value );

    /// Add an unsigned integer parameter with the given name and value.
    void addParameter( const char* name, uint value );

    /// Add a scalar parameter with the given name and value.
    void addParameter( const char* name, Scalar value );

    /// Add a vector of integers parameter with the given name and values.
    void addParameter( const char* name, std::vector<int> values );

    /// Add a vector of unsigned integers parameter with the given name and values.
    void addParameter( const char* name, std::vector<uint> values );

    /// Add a vector of scalars parameter with the given name and values.
    void addParameter( const char* name, std::vector<Scalar> values );

    /// Add a Vector2 parameter with the given name and value.
    void addParameter( const char* name, const Core::Vector2& value );

    /// Add a Vector3 parameter with the given name and value.
    void addParameter( const char* name, const Core::Vector3& value );

    /// Add a Vector4 parameter with the given name and value.
    void addParameter( const char* name, const Core::Vector4& value );

    /// Add a 2x2 matrix parameter with the given name and value.
    void addParameter( const char* name, const Core::Matrix2& value );

    /// Add a 3x3 matrix parameter with the given name and value.
    void addParameter( const char* name, const Core::Matrix3& value );

    /// Add a 4x4 matrix parameter with the given name and value.
    void addParameter( const char* name, const Core::Matrix4& value );

    /// Add a texture parameter with the given name, texture data and bind location.
    void addParameter( const char* name, Texture* tex, int texUnit );

    // Fixme: why no update for std::vector<T> ?
    /// Update the integer parameter with the given name with the given value.
    void updateParameter( const char* name, int value );

    /// Update the unsigned integer parameter with the given name with the given value.
    void updateParameter( const char* name, uint value );

    /// Update the scalar parameter with the given name with the given value.
    void updateParameter( const char* name, Scalar value );

    /// Update the Vector2 parameter with the given name with the given value.
    void updateParameter( const char* name, const Core::Vector2& value );

    /// Update the Vector3 parameter with the given name with the given value.
    void updateParameter( const char* name, const Core::Vector3& value );

    /// Update the Vector4 parameter with the given name with the given value.
    void updateParameter( const char* name, const Core::Vector4& value );

    /// Update the 2x2 matrix parameter with the given name with the given value.
    void updateParameter( const char* name, const Core::Matrix2& value );

    /// Update the 3x3 matrix parameter with the given name with the given value.
    void updateParameter( const char* name, const Core::Matrix3& value );

    /// Update the 4x4 matrix parameter with the given name with the given value.
    void updateParameter( const char* name, const Core::Matrix4& value );

    /// Add all unregistered parameters from \p params.
    void concatParameters( const RenderParameters& params );

    /// Bind all the parameters.
    void bind( const ShaderProgram* shader ) const;

    /// Print stat info to the Debug output.
    void print() const {
        for ( const auto& p : m_scalarParamsVector )
        {
            LOG( logDEBUG ) << "  " << p.first << " : " << p.second.m_name;
        }
    }

  private:
    // FIXME(Charly): Any way to simplify this a bit ?
    /// The integer parameters.
    UniformBindableVector<IntParameter> m_intParamsVector;

    /// The unsigned integer parameters.
    UniformBindableVector<UIntParameter> m_uintParamsVector;

    /// The scalar parameters.
    UniformBindableVector<ScalarParameter> m_scalarParamsVector;

    /// The integer vector parameters.
    UniformBindableVector<IntsParameter> m_intsParamsVector;

    /// The unsigned integer vector parameters.
    UniformBindableVector<UIntsParameter> m_uintsParamsVector;

    /// The scalar vector parameters.
    UniformBindableVector<ScalarsParameter> m_scalarsParamsVector;

    /// The Vector2 parameters.
    UniformBindableVector<Vec2Parameter> m_vec2ParamsVector;

    /// The Vector3 parameters.
    UniformBindableVector<Vec3Parameter> m_vec3ParamsVector;

    /// The Vector4 parameters.
    UniformBindableVector<Vec4Parameter> m_vec4ParamsVector;

    /// The 2x2 matrix parameters.
    UniformBindableVector<Mat2Parameter> m_mat2ParamsVector;

    /// The 3x3 matrix parameters.
    UniformBindableVector<Mat3Parameter> m_mat3ParamsVector;

    /// The 4x4 matrix parameters.
    UniformBindableVector<Mat4Parameter> m_mat4ParamsVector;

    /// The Texture parameters.
    UniformBindableVector<TextureParameter> m_texParamsVector;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/RenderTechnique/RenderParameters.inl>

#endif // RADIUMENGINE_RENDERPARAMETERS_HPP
