#include <IO/AssimpLoader/AssimpGeometryDataLoader.hpp>

#include <IO/AssimpLoader/AssimpWrapper.hpp>
#include <assimp/mesh.h>

namespace Ra {
namespace IO {

template <typename T>
void AssimpGeometryDataLoader::fetchAttribute( T* aiData,
                                               int size,
                                               Core::Geometry::MultiIndexedGeometry& data,
                                               Core::Geometry::MeshAttrib a ) const {
    auto attribHandle = data.addAttrib<typename AssimpTypeWrapper<T>::Type>( getAttribName( a ) );
    auto& attribData  = data.vertexAttribs().getDataWithLock( attribHandle );
    attribData.resize( size );
#pragma omp parallel for
    for ( int i = 0; i < size; ++i ) {
        attribData.at( i ) = assimpToCore( aiData[i] );
    }
    data.vertexAttribs().unlock( attribHandle );
}

template <typename T>
void AssimpGeometryDataLoader::fetchIndexLayer( aiFace* faces,
                                                int numFaces,
                                                Core::Geometry::MultiIndexedGeometry& data ) const {
    auto layer    = std::make_unique<T>();
    auto& indices = layer->collection();
    indices.resize( numFaces );
#pragma omp parallel for
    for ( int i = 0; i < numFaces; ++i ) {
        indices[i] = assimpToCore<typename T::IndexType>( faces[i].mIndices, faces[i].mNumIndices );
    }
    data.addLayer( std::move( layer ), false, "indices" );
}

} // namespace IO
} // namespace Ra
