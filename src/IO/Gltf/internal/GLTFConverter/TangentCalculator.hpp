#pragma once
#include <Core/Asset/GeometryData.hpp>

#include <IO/Gltf/internal/GLTFConverter/mikktspace.h>

namespace Ra {
namespace IO {
namespace GLTF {

// inspired by https://github.com/teared/mikktspace-for-houdini
/**
 * Functor that computes tangent of a triangle mesh.
 * Tangents are computed according to the GLTF specification
 * inspired by https://github.com/teared/mikktspace-for-houdini
 *
 * Note that, as stated in https://github.com/KhronosGroup/glTF-Sample-Models/issues/174, the
 tangents could be
 * computed in the glsl shader by the following code :
 *
 * The "reference" glTF PBR shader contains the following code. This code is intended by the working
 group to be the standard implementation of the shader calculations:

    vec3 pos_dx = dFdx(v_Position);
    vec3 pos_dy = dFdy(v_Position);
    vec3 tex_dx = dFdx(vec3(v_UV, 0.0));
    vec3 tex_dy = dFdy(vec3(v_UV, 0.0));
    vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);

    vec3 ng = normalize(v_Normal);

    t = normalize(t - ng * dot(ng, t));
    vec3 b = normalize(cross(ng, t));
    mat3 tbn = mat3(t, b, ng);

    vec3 n = texture2D(u_NormalSampler, v_UV).rgb;
    n = normalize(tbn * ((2.0 * n - 1.0) * vec3(u_NormalScale, u_NormalScale, 1.0)));

 *
 *
 */
class TangentCalculator
{
  public:
    /** Initialize MikkTSpaceInterface with callbacks and run calculator.
     *
     * @param gdp the geometry data for which tangents must be computed
     * @param basic tru if only tangents must be computed, fals if binormals are required.
     */
    void operator()( Ra::Core::Asset::GeometryData* gdp, bool basic = true );

    /**
     * MikkTSpace callback that returns number of primitives in the geometry.
     * @param context
     * @return
     */
    static int getNumFaces( const SMikkTSpaceContext* context );

    /**
     * MikkTSpace callback that returns number of vertices in the primitive given by index.
     *
     * @param context
     * @param primnum
     * @return
     */
    static int getNumVerticesOfFace( const SMikkTSpaceContext* context, int primnum );

    /**
     * MikkTSpace callback that extract 3-float position of the vertex's point.
     *
     * @param context
     * @param pos
     * @param primnum
     * @param vtxnum
     */
    static void
    getPosition( const SMikkTSpaceContext* context, float pos[], int primnum, int vtxnum );

    /**
     * MikkTSpace callback that extract 3-float vertex normal.
     *
     * @param context
     * @param normal
     * @param primnum
     * @param vtxnum
     */
    static void
    getNormal( const SMikkTSpaceContext* context, float normal[], int primnum, int vtxnum );

    /**
     * MikkTSpace callback that extract 2-float vertex uv.
     *
     * @param context
     * @param uv
     * @param primnum
     * @param vtxnum
     */
    static void
    getTexCoord( const SMikkTSpaceContext* context, float uv[], int primnum, int vtxnum );

    /**
     * MikkTSpace callback that set tangent attribute on the geometry vertex.
     *
     * @param context
     * @param tangentu
     * @param sign
     * @param primnum
     * @param vtxnum
     */
    static void setTSpaceBasic( const SMikkTSpaceContext* context,
                                const float tangentu[],
                                float sign,
                                int primnum,
                                int vtxnum );
    /**
     * MikkTSpace callback that set tangent (tangentu) and binormal (tangentv) attributes on the
     * geometry vertex.
     *
     * @param context
     * @param tangentu
     * @param tangentu
     * @param sign
     * @param primnum
     * @param vtxnum
     */
    static void setTSpace( const SMikkTSpaceContext* context,
                           const float tangentu[],
                           const float tangentv[],
                           float magu,
                           float magv,
                           tbool keep,
                           int primnum,
                           int vtxnum );
};

} // namespace GLTF
} // namespace IO
} // namespace Ra
