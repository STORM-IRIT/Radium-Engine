#pragma once
#include <Engine/RaEngine.hpp>

#include <Core/Types.hpp>
#include <Engine/Data/Mesh.hpp>
#include <Engine/Data/Texture.hpp>

#include <string>

namespace Ra {
namespace Engine {
namespace Data {
class ShaderProgram;
struct ViewingParameters;

/// Define a spherical, infinite light source.
/// This object could be used for texturing skyboxes or to implement infinite lighting.
/// An operator to convert an envmap to SH matrices for irradiance mapping is defined.
class RA_ENGINE_API EnvironmentTexture
{
  public:
    enum class EnvMapType { ENVMAP_PFM = 0, ENVMAP_CUBE, ENVMAP_LATLON };
    /**
     * Constructors and destructor follow the 'rule of five'
     *  https://en.cppreference.com/w/cpp/language/rule_of_three
     */
    /** @{ */

    /**
     * Construct an envmap from a file.
     * Supported image component of the envmap are the following.
     *
     *   - a pfm file for cross cubemap
     *   - a list of png or jpeg for individual cube map faces (see below for naming convention)
     *   - a single png, exr, hdr, jpg file for Spherical equirectangular envmap
     *
     * If the cubemap is defined by a list of files, they must be named according to the
     * corresponding face of the cube :
     * *posx* or *-X-plux*  : +X face of the cube <br/>
     * *negx* or *-X-minux* : -X face of the cube <br/>
     * *posy* or *-Y-plux* : +Y face of the cube <br/>
     * *negy* or *-Y-minux* : -Y face of the cube <br/>
     * *posz* or *-Z-plux* : +Z face of the cube <br/>
     * *negz* or *-Z-minux* : -Z face of the cube <br/>
     * @param mapName The list of filenames
     * @param type The file type. Supported file types are PFM for single files or whatever is
     * supported by stb for per-face files.
     * @param isSkybox indicates if the envmap must be associated with a sky box and rendered like
     * this.
     *
     * @note The envmap is trasnformed at loading/build time to the Radium global frame.
     * @todo : if the file given to the constructor does not exist, generates a white envmap ?
     */
    explicit EnvironmentTexture( const std::string& mapName, bool isSkybox = false );
    EnvironmentTexture( const EnvironmentTexture& ) = delete;
    EnvironmentTexture& operator=( const EnvironmentTexture& ) = delete;
    EnvironmentTexture( EnvironmentTexture&& )                 = default;
    EnvironmentTexture& operator=( EnvironmentTexture&& ) = default;
    /// destructor
    ~EnvironmentTexture() = default;
    /**@}*/

    std::string getImageName() const;
    EnvMapType getImageType() const;

    /**
     * Saves the spherical image representing the SH-encoded envmap
     * @param filename
     */
    void saveShProjection( const std::string filename );

    /**
     * Return the SH Matrix corresponding to the given color channel.
     * @param channel
     * @return
     */
    Ra::Core::Matrix4 getShMatrix( int channel );

    /**
     * Render the envmap as a textured cube. This method does nothing if the envmap is not a skybox
     * @param viewParams
     */
    void render( const Ra::Engine::Data::ViewingParameters& viewParams );

    /**
     * Set the state of the sky bos
     * @param state true to render the skybos, false to just use the SH coefficients
     */
    void setSkybox( bool state );

    void setStrength( float s );
    float getStrength() const;
    /**
     * @return true if the envmap is a skybox and might be rendered.
     */
    bool isSkybox() const;

    /**
     * @return the cubemap texture defining the environment
     */
    Ra::Engine::Data::Texture* getEnvironmentTexture();

    void updateGL();

  private:
    void initializeTexture();

    void setupTexturesFromPfm();
    void setupTexturesFromCube();
    void setupTexturesFromSphericalEquiRectangular();

    void computeSHMatrices();
    void updateCoeffs( float* hdr, float x, float y, float z, float domega );
    float* getPixel( float x, float y, float z );
    void tomatrix();
    Ra::Engine::Data::Texture* getSHImage();

    std::string m_name;
    EnvMapType m_type;

    // The raw pixel values of the envmap
    float* m_skyData[6];
    size_t m_width { 0 };
    size_t m_height { 0 };
    float m_shcoefs[9][3];
    Ra::Core::Matrix4 m_shMatrices[3];
    std::unique_ptr<Ra::Engine::Data::Texture> m_shtexture { nullptr };
    /// Is the envmap a Skybox ?
    bool m_isSkyBox { false };
    /// The mesh of the skybox
    std::unique_ptr<Ra::Engine::Data::Mesh> m_displayMesh { nullptr };
    /// The texture of the skybox
    std::unique_ptr<Ra::Engine::Data::Texture> m_skyTexture { nullptr };
    /// The Lod to use for the skybox
    float m_environmentStrength { 1 };
    /// The shader for the skybox
    const Ra::Engine::Data::ShaderProgram* m_skyShader { nullptr };
    bool m_glReady { false };
};
} // namespace Data
} // namespace Engine
} // namespace Ra

#include <Engine/Data/EnvironmentTexture.inl>
