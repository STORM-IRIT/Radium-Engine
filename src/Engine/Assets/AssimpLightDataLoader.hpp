#ifndef RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP

#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Assets/DataLoader.hpp>

#include <set>

struct aiScene;
struct aiLight;
struct aiNode;


namespace Ra {
    namespace Engine
    {
        class Light;
    }

    namespace Asset {

class LightData;


class AssimpLightDataLoader : public DataLoader< LightData > {
public:
    /// CONSTRUCTOR
    AssimpLightDataLoader( const std::string& filepath, const bool VERBOSE_MODE = false );

    /// DESTRUCTOR
    ~AssimpLightDataLoader();

    /// LOADING
    void loadData( const aiScene* scene, std::vector< std::unique_ptr< LightData > >& data ) override;

protected:
    /// QUERY
    inline bool sceneHasLight( const aiScene* scene ) const;

    uint sceneLightSize( const aiScene* scene ) const;

    /// LOADING

    void loadLightData(const aiScene* scene, const aiLight& light, LightData& data );

    Core::Matrix4 loadLightFrame(const aiScene* scene,
                        const Core::Matrix4& parentFrame,
                        LightData& data  ) const;

    /// NAME
    void fetchName(const aiLight& mesh, LightData& data) const;

    /// TYPE
    void fetchType( const aiLight& mesh, LightData& data ) const;


    /// FRAME
    inline void setFrame( const Core::Matrix4& frame ) {
        m_frame = frame;
    }

private:
    std::string m_filepath;
    Core::Transform m_frame;
};

} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP
