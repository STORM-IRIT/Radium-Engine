#pragma once

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
 * Management of shader parameters with automatic binding of a named parameter to the corresponding
 * glsl uniform.
 */
class RA_ENGINE_API RenderParameters final
{
  public:
    class Parameter
    {
      public:
        Parameter() = default;
        explicit Parameter( const char* name ) : m_name( name ) {}
        virtual ~Parameter()                                   = default;
        virtual void bind( const ShaderProgram* shader ) const = 0;

        const char* m_name{nullptr};
    };

    template <typename T>
    class TParameter final : public Parameter
    {
      public:
        TParameter() = default;
        TParameter( const char* name, const T& value ) : Parameter( name ), m_value( value ) {}
        ~TParameter() override = default;
        void bind( const ShaderProgram* shader ) const override;

        T m_value{};
    };

    class TextureParameter final : public Parameter
    {
      public:
        TextureParameter() = default;
        TextureParameter( const char* name, Texture* tex, int texUnit ) :
            Parameter( name ), m_texture( tex ), m_texUnit( texUnit ) {}

        ~TextureParameter() override = default;
        void bind( const ShaderProgram* shader ) const override;

        Texture* m_texture{nullptr};
        int m_texUnit{-1};
    };

    template <typename T>
    class UniformBindableSet final
        : public std::map<
              std::string,
              T,
              std::less<std::string>,
              Core::AlignedAllocator<std::pair<const std::string, T>, EIGEN_MAX_ALIGN_BYTES>>
    {
      public:
        void bind( const ShaderProgram* shader ) const;
    };

    using IntParameter    = TParameter<int>;
    using UIntParameter   = TParameter<uint>;
    using ScalarParameter = TParameter<Scalar>;

    using IntsParameter  = TParameter<std::vector<int>>;
    using UIntsParameter = TParameter<std::vector<uint>>;

    //! globjects seems to not handle vector of double
    using ScalarsParameter = TParameter<std::vector<float>>;

    using Vec2Parameter = TParameter<Core::Vector2>;
    using Vec3Parameter = TParameter<Core::Vector3>;
    using Vec4Parameter = TParameter<Core::Vector4>;

    using Mat2Parameter = TParameter<Core::Matrix2>;
    using Mat3Parameter = TParameter<Core::Matrix3>;
    using Mat4Parameter = TParameter<Core::Matrix4>;

  public:
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

    /**
     * Adding a texture parameter.
     * The default (-1) for the texUnit parameter implies automatic uniform binding for the
     * texture unit associated with the named sampler.
     * If texUnit is given, then uniform binding will be made at this explicit location.
     */
    void addParameter( const char* name, Texture* tex, int texUnit = -1 );

    void concatParameters( const RenderParameters& params );

    void bind( const ShaderProgram* shader ) const;

    void print() const {
        using namespace Core::Utils; // log
        for ( const auto& p : m_scalarParamsVector )
        {
            LOG( logDEBUG ) << "  " << p.first << " : " << p.second.m_name;
        }
    }

  private:
    // Radium V2 : Any way to simplify this a bit ? See Mesh attribs
    UniformBindableSet<IntParameter> m_intParamsVector;
    UniformBindableSet<UIntParameter> m_uintParamsVector;
    UniformBindableSet<ScalarParameter> m_scalarParamsVector;

    UniformBindableSet<IntsParameter> m_intsParamsVector;
    UniformBindableSet<UIntsParameter> m_uintsParamsVector;
    UniformBindableSet<ScalarsParameter> m_scalarsParamsVector;

    UniformBindableSet<Vec2Parameter> m_vec2ParamsVector;
    UniformBindableSet<Vec3Parameter> m_vec3ParamsVector;
    UniformBindableSet<Vec4Parameter> m_vec4ParamsVector;

    UniformBindableSet<Mat2Parameter> m_mat2ParamsVector;
    UniformBindableSet<Mat3Parameter> m_mat3ParamsVector;
    UniformBindableSet<Mat4Parameter> m_mat4ParamsVector;

    UniformBindableSet<TextureParameter> m_texParamsVector;
};

/**
 * Shader program parameter provider.
 * a ShaderParameterProvider is an object that is associated to a render technique to provide the
 * uniform parameter set for the program. When an RenderObject is drawn using a given
 * rendertechnique, the ShaderParameterProvider associated to the renderTechnique is responsible to
 * set all the uniforms needed by the rendertechnique.
 */
class ShaderParameterProvider
{
  public:
    virtual ~ShaderParameterProvider() = default;
    const RenderParameters& getParameters() const { return m_renderParameters; }
    /**
     * Update the OpenGL states used by the ShaderParameterProvider.
     * These state could be the ones from an associated material (textures, precomputed tables or
     * whatever data associated to the material)  or some parameters that are
     * specific to the provider sementic.
     */
    virtual void updateGL() = 0;

  protected:
    RenderParameters m_renderParameters;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/RenderTechnique/RenderParameters.inl>
