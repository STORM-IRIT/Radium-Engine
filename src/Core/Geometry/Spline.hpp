#ifndef RADIUMENGINE_SPLINE_HPP_
#define RADIUMENGINE_SPLINE_HPP_

#include <Core/Containers/VectorArray.hpp>
#include <Core/RaCore.hpp>
#include <vector>

namespace Ra {
namespace Core {
namespace Geometry {
/**
 * @class Spline
 *
 * @brief Handling spline curves of arbitrary dimensions
 * @note This class use the efficient blossom algorithm to compute a position on
 * the curve.
 * @tparam D: dimension of the curve.
 * @tparam K: order of the curve (min 2).
 */
template <uint D, uint K = 2>
class Spline {
  public:
    /**
     * The type of the nodal vector.
     * This will define the behavior of the spline w.r.t.\ its control points.
     */
    // TODO: allow non uniform nodal vector.
    enum Type {
        UNIFORM,     ///< Not connected to the first and last control points.
        OPEN_UNIFORM ///< Connected to the first and last control points.
    };

    /**
     * Type for points and vectors.
     */
    using Vector = typename Eigen::Matrix<Scalar, D, 1>;

  public:
    inline Spline( Type type = OPEN_UNIFORM );

    /**
     * Set the position of the control points.
     */
    inline void setCtrlPoints( const Core::VectorArray<Vector>& points );

    /**
     * Return the control points of the Spline.
     */
    inline const Core::VectorArray<Vector>& getCtrlPoints() const;

    /**
     * Set the nodal vector type.
     */
    inline void setType( Type type );

    /**
     * Return the point at the curvilinear parameter \p u.
     * \note \p u is clamped to [0;1].
     */
    inline Vector f( Scalar u ) const;

    /** Return the tangent vector at the curvilinear parameter \p u.
     *  \note \p u is clamped to [0;1].
     */
    inline Vector df( Scalar u ) const;

  private:
    /**
     * Assert the spline is correctly initialized.
     */
    inline void assertSplines() const;

    /**
     * Calls setNodeToUniform() or setNodeToOpenUniform() depending on the spline type.
     */
    inline void setNodalVector();

    /**
     * Initialize the nodal vector to be uniform.
     */
    inline void setNodeToUniform();

    /**
     * Initialize the nodal vector to be open uniform.
     */
    inline void setNodeToOpenUniform();

    /**
     * Evaluate the equation of a splines using the blossom algorithm.
     * @param u : the curve parameter which range from the values
     *            [node[k-1]; node[point.size()]]
     * @param point : the control points which size must be at least equal to
     *                the order of the spline (point.size() >= k)
     * @param k : the spline order (degree == k-1)
     * @param node : the nodal vector which defines the speed of the spline
     *               parameter u.
     * @param off : offset to apply to the nodal vector 'node' before reading
     *              from it. This is useful to compute derivatives.
     * \warning The nodal vector size must be equal to (k + point.size()).
     */
    static inline Vector eval( Scalar u, const Core::VectorArray<Vector>& points,
                               const std::vector<Scalar>& node, uint k, int off = 0 );

    /**
     * Recursive part of the blossom algorithm for Spline evaluation.
     */
    static inline Vector evalRec( Scalar u, const Core::VectorArray<Vector>& points,
                                  const std::vector<Scalar>& node, uint k );

  private:
    Core::VectorArray<Vector> m_points; ///< Control points
    Core::VectorArray<Vector> m_vecs;   ///< Control points differences
    std::vector<Scalar> m_node;         ///< Nodal vector
    Type m_type;                        ///< Nodal vector type
};
} // namespace Geometry
} // namespace Core
} // namespace Ra

#include <Core/Geometry/Spline.inl>

#endif
