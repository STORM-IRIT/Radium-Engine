#pragma once
#include <Engine/RaEngine.hpp>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>

#include <Engine/Data/Texture.hpp>

namespace Ra {
namespace Engine {
namespace Data {

/// Sample the volume of a sphere as a random displacement of its sampled surface
class RA_ENGINE_API SphereSampler
{
  public:
    /// Available samplers
    enum class SamplingMethod {
        FIBONACCI,  ///<- Sample the sphere with a fibonacci sequence
        HAMMERSLEY, ///<- Sample the sphere using Hammersley Point Set
        RANDOM,     ///<-- Sample the sphere using uniform random sequence
        GEODESIC,   ///<- Sample the sphere by subdivision of an icosahedron
    };
    /// Generate a sequence of point distributed on the sphere
    /// @param method the sampling method to use (@see SamplingMethod)
    /// @param level the number of point to generate
    explicit SphereSampler( SamplingMethod method, int level = 0 );
    SphereSampler( const SphereSampler& ) = delete;
    SphereSampler& operator=( const SphereSampler& ) = delete;
    SphereSampler( SphereSampler&& )                 = delete;
    SphereSampler& operator=( SphereSampler&& ) = delete;
    /// destructor
    ~SphereSampler() = default;

    /// Get the sampling scheme as a Radium texture
    Ra::Engine::Data::Texture* asTexture();

    /// Return the number of samples generated
    [[nodiscard]] int nbSamples() const { return m_nbPoints; }

  private:
    /// Texture generated on demand to be used with an OpenGL shader
    std::unique_ptr<Ra::Engine::Data::Texture> m_texture { nullptr };
    /// Final number of points
    int m_nbPoints;
    /// The points on the sphere
    Ra::Core::VectorArray<Ra::Core::Vector4> m_points;
};

} // namespace Data
} // namespace Engine
} // namespace Ra
