#ifndef RADIUMENGINE_CATMULLCLARK_SUBDIVISION_HPP
#define RADIUMENGINE_CATMULLCLARK_SUBDIVISION_HPP

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/HalfEdge.hpp>

namespace Ra {
namespace Core {
namespace Algorithm {

    /// This class represents the sequence of computations, done by one pass of
    /// the CatmullClark subdivision scheme, to compute the positions of the old/new vertices.
    struct CatmullClarkOperation
    {
        typedef std::pair<uint, Scalar> CombinationPoint;       /// maps a point id with the coefficient applied to it
        typedef std::vector<CombinationPoint> CombinationSet;   /// a list of CombinationPoint from which to compute a new position
        typedef std::pair<uint, CombinationSet> PointOperation; /// maps a point id to the CombinationPoint for it's new position

        uint m_nbVertices; // number of vertices after the subdivision
        VectorArray<VectorNui> m_quads; // the list of quads introduced by the subdivision
        std::vector<PointOperation> m_newPointsOperations;   // the list of operations produced by the subdivision
        std::vector<PointOperation> m_oldPointsOperations; // the list of operations produced by the subdivision
    };

    /// \brief Applies one pass of the Catmull-Clark subdivision scheme to \p mesh.
    ///
    /// The construction operations can be registered into \p operation.
    /// Operations are done considering the mesh faces only, not the triangulation.
    ///
    /// For more details, see:
    ///     https://en.wikipedia.org/wiki/Catmull%E2%80%93Clark_subdivision_surface
    void CatmullClark( TriangleMesh& mesh, HalfEdgeData& heData,
                       CatmullClarkOperation* operation = nullptr,
                       bool register_op = false );

} // namespace Algorithm
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_CATMULLCLARK_SUBDIVISION_HPP
