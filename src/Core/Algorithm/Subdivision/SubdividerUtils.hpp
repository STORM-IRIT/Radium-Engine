#ifndef RADIUMENGINE_SUBDIVIDERUTILS_H
#define RADIUMENGINE_SUBDIVIDERUTILS_H

#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>

namespace Ra {
namespace Core {

/// \name Deal with normals
/// Utils to deal with normals.
///@{

/// Create a new property for normals on faces of \p mesh.
/// \note This new property will have to be propagated onto the newly created
/// halfedges with copyNormal().
inline void createNormalPropOnFaces( TopologicalMesh& mesh,
                                     OpenMesh::FPropHandleT<TopologicalMesh::Normal>& copy ) {
    auto nph = mesh.halfedge_normals_pph();
    mesh.add_property( copy, mesh.property( nph ).name() + "_subdiv_copy_F" );
}

/// Remove face property \p prop from \p mesh.
inline void clearProp( TopologicalMesh& mesh,
                       OpenMesh::FPropHandleT<TopologicalMesh::Normal>& prop ) {
    mesh.remove_property( prop );
    prop = OpenMesh::FPropHandleT<TopologicalMesh::Normal>();
}

/// Copy the normal property from \p input_heh to \p copy_heh.
inline void copyNormal( TopologicalMesh& mesh, const TopologicalMesh::HalfedgeHandle& input_heh,
                        const TopologicalMesh::HalfedgeHandle& copy_heh ) {
    auto nph = mesh.halfedge_normals_pph();
    mesh.property( nph, copy_heh ) = mesh.property( nph, input_heh );
}

/// Copy the face normal property \p fProp from \p fh to \p heh.
/// \note \p fProp must have been previously created through createNormalPropOnFaces().
inline void copyNormalFromFace( TopologicalMesh& mesh, const TopologicalMesh::FaceHandle& fh,
                                const TopologicalMesh::HalfedgeHandle& heh,
                                const OpenMesh::FPropHandleT<TopologicalMesh::Normal>& fProp ) {
    auto nph = mesh.halfedge_normals_pph();
    mesh.property( nph, heh ) = mesh.property( fProp, fh );
}

/// Interpolate normal property on edge center (after edge split).
inline void interpolateNormal( TopologicalMesh& mesh, const TopologicalMesh::HalfedgeHandle& in_a,
                               const TopologicalMesh::HalfedgeHandle& in_b,
                               const TopologicalMesh::HalfedgeHandle& out, Scalar f ) {
    auto nph = mesh.halfedge_normals_pph();
    mesh.property( nph, out ) =
        ( ( 1 - f ) * mesh.property( nph, in_a ) + f * mesh.property( nph, in_b ) ).normalized();
}

/// Interpolate normal property on face center.
/// \note \p fProp must have been previously created through createNormalPropOnFaces().
inline void
interpolateNormalOnFaces( TopologicalMesh& mesh, const TopologicalMesh::FaceHandle& fh,
                          const OpenMesh::FPropHandleT<TopologicalMesh::Normal>& fprop ) {
    auto nph = mesh.halfedge_normals_pph();

    // init sum to first
    auto heh = mesh.halfedge_handle( fh );
    mesh.property( fprop, fh ) = mesh.property( nph, heh );
    heh = mesh.next_halfedge_handle( heh );

    // sum others
    for ( int i = 1; i < mesh.valence( fh ); ++i )
    {
        mesh.property( fprop, fh ) += mesh.property( nph, heh );
        heh = mesh.next_halfedge_handle( heh );
    }

    // normalize
    mesh.property( fprop, fh ) = mesh.property( fprop, fh ).normalized();
}
///@}

/// \name Deal with custom properties
/// Utils to deal with custom properties of any type.
///@{

/// Create a new property for each \p input properties of \p mesh on faces.
/// \note This new property will have to be propagated onto the newly created
/// halfedges with copyProps().
template <typename T>
void createPropsOnFaces( TopologicalMesh& mesh, const std::vector<OpenMesh::HPropHandleT<T>>& input,
                         std::vector<OpenMesh::FPropHandleT<T>>& output ) {
    output.reserve( input.size() );
    for ( const auto& oh : input )
    {
        OpenMesh::FPropHandleT<T> oh_;
        mesh.add_property( oh_, mesh.property( oh ).name() + "_subdiv_copy_F" );
        output.push_back( oh_ );
    }
}

/// Remove \p props from \p mesh.
template <typename T>
void clearProps( TopologicalMesh& mesh, std::vector<OpenMesh::FPropHandleT<T>>& props ) {
    for ( auto& oh : props )
    {
        mesh.remove_property( oh );
    }
    props.clear();
}

/// Copy \p props properties from \p input_heh to \p copy_heh.
template <typename T>
void copyProps( TopologicalMesh& mesh, const TopologicalMesh::HalfedgeHandle& input_heh,
                const TopologicalMesh::HalfedgeHandle& copy_heh,
                const std::vector<OpenMesh::HPropHandleT<T>>& props ) {
    for ( const auto& oh : props )
    {
        mesh.property( oh, copy_heh ) = mesh.property( oh, input_heh );
    }
}

/// Copy face properties \p props from \p fh to \p heh.
/// \note \p fProps must have been previously created through createPropsOnFaces().
template <typename T>
void copyPropsFromFace( TopologicalMesh& mesh, const TopologicalMesh::FaceHandle& fh,
                        const TopologicalMesh::HalfedgeHandle& heh,
                        const std::vector<OpenMesh::FPropHandleT<T>>& fprops,
                        const std::vector<OpenMesh::HPropHandleT<T>>& hprops ) {
    for ( uint i = 0; i < hprops.size(); ++i )
    {
        auto hp = hprops[i];
        auto fp = fprops[i];
        mesh.property( hp, heh ) = mesh.property( fp, fh );
    }
}

/// Interpolate \p props on edge center (after edge split).
template <typename T>
void interpolateProps( TopologicalMesh& mesh, const TopologicalMesh::HalfedgeHandle& in_a,
                       const TopologicalMesh::HalfedgeHandle& in_b,
                       const TopologicalMesh::HalfedgeHandle& out, Scalar f,
                       const std::vector<OpenMesh::HPropHandleT<T>>& props ) {
    // interpolate properties
    for ( const auto& oh : props )
    {
        mesh.property( oh, out ) =
            ( 1 - f ) * mesh.property( oh, in_a ) + f * mesh.property( oh, in_b );
    }
}

/// Interpolate \p hprops on face center.
/// \note \p fProps must have been previously created through createPropsOnFaces().
template <typename T>
void interpolatePropsOnFaces( TopologicalMesh& mesh, const TopologicalMesh::FaceHandle& fh,
                              const std::vector<OpenMesh::HPropHandleT<T>>& hprops,
                              const std::vector<OpenMesh::FPropHandleT<T>>& fprops ) {
    auto heh = mesh.halfedge_handle( fh );
    const int valence = mesh.valence( fh );

    // init sum to first
    for ( int j = 0; j < fprops.size(); ++j )
    {
        auto hp = hprops[j];
        auto fp = fprops[j];
        mesh.property( fp, fh ) = mesh.property( hp, heh );
    }
    heh = mesh.next_halfedge_handle( heh );
    // sum others
    for ( int i = 1; i < valence; ++i )
    {
        for ( int j = 0; j < hprops.size(); ++j )
        {
            auto hp = hprops[j];
            auto fp = fprops[j];
            mesh.property( fp, fh ) += mesh.property( hp, heh );
        }
        heh = mesh.next_halfedge_handle( heh );
    }
    // normalize
    for ( int j = 0; j < fprops.size(); ++j )
    {
        auto fp = fprops[j];
        mesh.property( fp, fh ) = mesh.property( fp, fh ) / valence;
    }
}
///@}

/// \name Deal with all attributes
/// Utils to deal with the normal and custom properties.
///@{

/// Create a new property for each property of \p mesh on faces.
/// Outputs the new face properties handles in the corresponding output parameters.
/// \note These new properties will have to be propagated onto the newly created
/// halfedges with copyAllProps().
inline void createAllPropsOnFaces( TopologicalMesh& mesh,
                                   OpenMesh::FPropHandleT<TopologicalMesh::Normal>& normalProp,
                                   std::vector<OpenMesh::FPropHandleT<Scalar>>& floatProps,
                                   std::vector<OpenMesh::FPropHandleT<Vector2>>& vec2Props,
                                   std::vector<OpenMesh::FPropHandleT<Vector3>>& vec3Props,
                                   std::vector<OpenMesh::FPropHandleT<Vector4>>& vec4Props ) {
    createNormalPropOnFaces( mesh, normalProp );
    createPropsOnFaces( mesh, mesh.getFloatPropsHandles(), floatProps );
    createPropsOnFaces( mesh, mesh.getVector2PropsHandles(), vec2Props );
    createPropsOnFaces( mesh, mesh.getVector3PropsHandles(), vec3Props );
    createPropsOnFaces( mesh, mesh.getVector4PropsHandles(), vec4Props );
}

/// Remove all the given properties from \p mesh.
inline void clearAllProps( TopologicalMesh& mesh,
                           OpenMesh::FPropHandleT<TopologicalMesh::Normal>& normalProp,
                           std::vector<OpenMesh::FPropHandleT<Scalar>>& floatProps,
                           std::vector<OpenMesh::FPropHandleT<Vector2>>& vec2Props,
                           std::vector<OpenMesh::FPropHandleT<Vector3>>& vec3Props,
                           std::vector<OpenMesh::FPropHandleT<Vector4>>& vec4Props ) {
    clearProp( mesh, normalProp );
    clearProps( mesh, floatProps );
    clearProps( mesh, vec2Props );
    clearProps( mesh, vec3Props );
    clearProps( mesh, vec4Props );
}

/// Copy all properties from \p input_heh to \p copy_heh.
inline void copyAllProps( TopologicalMesh& mesh, const TopologicalMesh::HalfedgeHandle& input_heh,
                          const TopologicalMesh::HalfedgeHandle& copy_heh ) {
    copyNormal( mesh, input_heh, copy_heh );
    copyProps( mesh, input_heh, copy_heh, mesh.getFloatPropsHandles() );
    copyProps( mesh, input_heh, copy_heh, mesh.getVector2PropsHandles() );
    copyProps( mesh, input_heh, copy_heh, mesh.getVector3PropsHandles() );
    copyProps( mesh, input_heh, copy_heh, mesh.getVector4PropsHandles() );
}

/// Copy all given face properties from \p fh to \p heh.
/// \note Each property must have been previously created either all at once
/// through createAllPropsOnFaces(), or individually through
/// createNormalPropOnFaces() and createPropsOnFaces().
inline void copyAllPropsFromFace( TopologicalMesh& mesh, const TopologicalMesh::FaceHandle& fh,
                                  const TopologicalMesh::HalfedgeHandle& heh,
                                  OpenMesh::FPropHandleT<TopologicalMesh::Normal>& normalProp,
                                  std::vector<OpenMesh::FPropHandleT<Scalar>>& floatProps,
                                  std::vector<OpenMesh::FPropHandleT<Vector2>>& vec2Props,
                                  std::vector<OpenMesh::FPropHandleT<Vector3>>& vec3Props,
                                  std::vector<OpenMesh::FPropHandleT<Vector4>>& vec4Props ) {
    copyNormalFromFace( mesh, fh, heh, normalProp );
    copyPropsFromFace( mesh, fh, heh, floatProps, mesh.getFloatPropsHandles() );
    copyPropsFromFace( mesh, fh, heh, vec2Props, mesh.getVector2PropsHandles() );
    copyPropsFromFace( mesh, fh, heh, vec3Props, mesh.getVector3PropsHandles() );
    copyPropsFromFace( mesh, fh, heh, vec4Props, mesh.getVector4PropsHandles() );
}

/// Interpolate all properties on edge center (after edge split).
inline void interpolateAllProps( TopologicalMesh& mesh, const TopologicalMesh::HalfedgeHandle& in_a,
                                 const TopologicalMesh::HalfedgeHandle& in_b,
                                 const TopologicalMesh::HalfedgeHandle& out, Scalar f ) {
    interpolateNormal( mesh, in_a, in_b, out, f );
    interpolateProps( mesh, in_a, in_b, out, f, mesh.getFloatPropsHandles() );
    interpolateProps( mesh, in_a, in_b, out, f, mesh.getVector2PropsHandles() );
    interpolateProps( mesh, in_a, in_b, out, f, mesh.getVector3PropsHandles() );
    interpolateProps( mesh, in_a, in_b, out, f, mesh.getVector4PropsHandles() );
}

/// Interpolate \p hprops on face center.
/// \note Each property must have been previously created either all at once
/// through createAllPropsOnFaces(), or individually through
/// createNormalPropOnFaces() and createPropsOnFaces().
inline void interpolateAllPropsOnFaces( TopologicalMesh& mesh,
                                        const TopologicalMesh::FaceHandle& fh,
                                        OpenMesh::FPropHandleT<TopologicalMesh::Normal>& normalProp,
                                        std::vector<OpenMesh::FPropHandleT<Scalar>>& floatProps,
                                        std::vector<OpenMesh::FPropHandleT<Vector2>>& vec2Props,
                                        std::vector<OpenMesh::FPropHandleT<Vector3>>& vec3Props,
                                        std::vector<OpenMesh::FPropHandleT<Vector4>>& vec4Props ) {
    interpolateNormalOnFaces( mesh, fh, normalProp );
    interpolatePropsOnFaces( mesh, fh, mesh.getFloatPropsHandles(), floatProps );
    interpolatePropsOnFaces( mesh, fh, mesh.getVector2PropsHandles(), vec2Props );
    interpolatePropsOnFaces( mesh, fh, mesh.getVector3PropsHandles(), vec3Props );
    interpolatePropsOnFaces( mesh, fh, mesh.getVector4PropsHandles(), vec4Props );
}
///@}

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_SUBDIVIDERUTILS_H
