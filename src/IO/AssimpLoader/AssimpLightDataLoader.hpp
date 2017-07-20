#ifndef RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP

#include <IO/RaIO.hpp>
#include <Core/File/DataLoader.hpp>
#include <Core/Math/LinearAlgebra.hpp>

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
    }
}

namespace Ra {
    namespace IO {

    class RA_IO_API AssimpLightDataLoader : public Asset::DataLoader< Asset::LightData > {
    public:
        /// CONSTRUCTOR
        AssimpLightDataLoader( const std::string& filepath, const bool VERBOSE_MODE = false );

        /// DESTRUCTOR
        ~AssimpLightDataLoader();

        /// LOADING
        void loadData( const aiScene* scene, std::vector< std::unique_ptr< Asset::LightData > >& data ) override;

    protected:
        /// QUERY
        inline bool sceneHasLight( const aiScene* scene ) const;

        uint sceneLightSize( const aiScene* scene ) const;

        /// LOADING

        void loadLightData( const aiScene* scene, const aiLight& light, Asset::LightData& data );

        Core::Matrix4 loadLightFrame( const aiScene* scene,
                                      const Core::Matrix4& parentFrame,
                                      Asset::LightData& data  ) const;

        /// NAME
        void fetchName( const aiLight& mesh, Asset::LightData& data ) const;

        /// TYPE
        void fetchType( const aiLight& mesh, Asset::LightData& data ) const;


        /// FRAME
        inline void setFrame( const Core::Matrix4& frame )
        {
            m_frame = frame;
        }

    private:
        std::string m_filepath;
        Core::Transform m_frame;
    };

    } // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP
