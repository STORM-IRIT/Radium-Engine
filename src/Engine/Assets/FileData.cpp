#include <Engine/Assets/FileData.hpp>

#include <ctime>
#include <Core/Log/Log.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Engine/Assets/AssimpGeometryDataLoader.hpp>
#include <Engine/Assets/AssimpHandleDataLoader.hpp>
#include <Engine/Assets/AssimpAnimationDataLoader.hpp>

namespace Ra {
namespace Asset {



/// CONSTRUCTOR
FileData::FileData( const std::string& filename,
                    const bool         VERBOSE_MODE ) :
    m_filename( filename ),
    m_loadingTime( 0.0 ),
    m_geometryData(),
    m_handleData(),
    m_animationData(),
    m_processed( false ),
    m_verbose( VERBOSE_MODE ) {
    loadFile();
}



/// DESTRUCTOR
FileData::~FileData() { }



/// LOAD
void FileData::loadFile( const bool FORCE_RELOAD ) {
    if( isProcessed() && FORCE_RELOAD ) {
        std::string filename = getFileName();
        reset();
        setFileName( filename );
    }

    if( !isInitialized() ) {
        return;
    }

    // File extension check
    // - If we decide to deal with user-defined file, here we should check if we are dealing with one of them or not

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( getFileName(),
                                              aiProcess_Triangulate           | // This could/should be taken away if we want to deal with mesh types other than trimehses
                                              aiProcess_JoinIdenticalVertices |
                                              aiProcess_GenSmoothNormals      |
                                              aiProcess_SortByPType           |
                                              aiProcess_FixInfacingNormals    |
                                              aiProcess_CalcTangentSpace      |
                                              aiProcess_GenUVCoords );

    // File was not loaded
    if( scene == nullptr ) {
        LOG( logERROR ) << "Error while loading file \"" << getFileName() << "\" : " << importer.GetErrorString() << ".";
        return;
    }

    if( m_verbose ) {
        LOG( logDEBUG ) << "File Loading begin...";
    }

    std::clock_t startTime;
    startTime = std::clock();

    AssimpGeometryDataLoader geometryLoader( Core::StringUtils::getDirName( getFileName() ), m_verbose );
    geometryLoader.loadData( scene, m_geometryData );

    AssimpHandleDataLoader handleLoader( m_verbose );
    handleLoader.loadData( scene, m_handleData );

    AssimpAnimationDataLoader animationLoader( m_verbose );
    animationLoader.loadData( scene, m_animationData );

    m_loadingTime = ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

    if( m_verbose ) {
        LOG( logDEBUG ) << "File Loading end.";
        displayInfo();
    }

    m_processed = true;
}



} // namespace Asset
} // namespace Ra

