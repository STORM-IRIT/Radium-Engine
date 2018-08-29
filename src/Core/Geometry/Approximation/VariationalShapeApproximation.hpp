#pragma once

#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <array>
#include <queue>
#include <vector>

namespace Ra {
namespace Core {
namespace Geometry {

/**
 *  The type of metric for the VSA algorithm.
 */
enum class MetricType { L2, L21, LLOYD };

/**
 * @brief The VariationalShapeApproximationBase class
 *
 *        The VariationalShapeApproximationBase class computes the K proxies describing
 *        a surface from a given triangle mesh.
 *
 * @tparam K The number of regions to have.
 * @note It is based on "Variational Shape Approximation" paper.
 * @warning It doesn't implement region teleporting or non-organic shape partitioning.
 */
template <uint K>
class VariationalShapeApproximationBase {
    static_assert( ( K > 0 ), "K must be greater than 0" );

  public:
    using Mesh = TriangleMesh; ///< Mesh class.

    using FaceBarycenter = Vector3Array;   ///< Face barycenters.
    using FaceNormal = Vector3Array;       ///< Face normals.
    using FaceArea = std::vector<Scalar>;  ///< Face areas.
    using FaceRegion = std::vector<uint>;  ///< Face region IDs.
    using FaceVisited = std::vector<bool>; ///< Dirty bits
    using FaceValue = std::vector<Scalar>; ///< Face value containing the color value.
    using FaceAdjacency = std::vector<std::set<TriangleIdx>>;

    using Proxy = std::pair<Vector3, Vector3>;      ///< Proxy structure.
    using ProxyList = std::array<Proxy, K>;         ///< List of proxies.
    using Region = std::vector<TriangleIdx>;        ///< Region structure.
    using RegionList = std::array<Region, K>;       ///< List of regions.
    using Energy = Scalar;                          ///< Energy value.
    using Pair = std::pair<TriangleIdx, uint>;      ///< Triangle-Proxy pair.
    using QueueEntry = std::pair<Energy, Pair>;     ///< Queue entry.
    using QueueEntryList = std::vector<QueueEntry>; ///< List of queue entries.

    /// Priority queue.
    using PriorityQueue = std::priority_queue<QueueEntry, QueueEntryList, std::greater<QueueEntry>>;

    explicit inline VariationalShapeApproximationBase( const Mesh& mesh );

    inline VariationalShapeApproximationBase( const VariationalShapeApproximationBase& other ) =
        default;

    inline VariationalShapeApproximationBase( VariationalShapeApproximationBase&& other ) = default;

    inline ~VariationalShapeApproximationBase();

    /// Initialize the data and set the seed triangles.
    inline void init();

    /// Return true if init() was called. False otherwise.
    inline bool initialized() const;

    /// Execute the algorithm performing the given amount of iterations.
    inline void exec( const uint iteration = 20 );

    /// Execute the algorithm performing the given amount of iterations.
    template <uint Iteration>
    inline void exec();

    /// Returns the i-th region.
    inline const Region& region( const uint i ) const;

    /// Returns the i-th proxy.
    inline const Proxy& proxy( const uint i ) const;

    /// Create the colors values for the faces.
    inline void create_region_color();
    /// Create the colors values for the faces.
    inline void create_energy_color();

    /// Shuffle the regions in order to have less color conflicts.
    inline void shuffle_regions();

  protected:
    /// Initialize the mesh properties and the algorithm data.
    inline void compute_data();

    /// Computes the seed triangles in a randomized fashion way.
    inline void compute_seed();

    /// Performs the geometry partitioning with the current proxies.
    inline void geometry_partitioning();

    /// Add the neighbors of T to the priority queue.
    inline void add_neighbors_to_queue( const TriangleIdx& T, const uint proxy_id );

    /// Performs the Proxy fitting.
    virtual void proxy_fitting() = 0;

    /// Computes the energy function for the given triangle and the given Proxy.
    virtual Energy E( const TriangleIdx& T, const Proxy& P ) const = 0;

  protected:
    const Mesh& m_mesh; ///< The mesh to partition.

    FaceBarycenter m_fbary; ///< The list of face barycenters.
    FaceNormal m_fnormal;   ///< The list of face normals.
    FaceArea m_farea;       ///< The list of face area.
    FaceRegion m_fregion;   ///< The list of regions the faces belong to.
    FaceVisited m_fvisited; ///< Whether the faces have been processed.
    FaceValue m_fvalue;     ///< The list of faces color value.
    FaceAdjacency m_fadj;   ///< The per-face neighboring faces.

    PriorityQueue m_queue; ///< The priority queue for processing the faces.
    RegionList m_region;   ///< The per-region list of faces.
    ProxyList m_proxy;     ///< The list of proxies.

    bool m_init; ///< Is the VSA algorithm initialized.
};

//============================================================================

/**
 * @brief The VariationalShapeApproximation class
 *
 *        The VariationalShapeApproximation class implements the proxy fitting
 *        accordingly to the selected metric.
 */
template <uint K, MetricType Type = MetricType::L2>
class VariationalShapeApproximation : public VariationalShapeApproximationBase<K> {
  public:
    using Proxy = typename VariationalShapeApproximationBase<K>::Proxy;

    using VariationalShapeApproximationBase<K>::VariationalShapeApproximationBase;

    virtual ~VariationalShapeApproximation();

  protected:
    /// Computes the Proxy fitting for L2 metric.
    inline void proxy_fitting() override final;

    /// Computes the energy function for the L2 metric.
    inline Scalar E( const TriangleIdx& T, const Proxy& P ) const override final;
};

//============================================================================

/**
 * @brief This is a specialized version of the VSA for the L21 metric.
 */
template <uint K>
class VariationalShapeApproximation<K, MetricType::L21>
    : public VariationalShapeApproximationBase<K> {
  public:
    using Proxy = typename VariationalShapeApproximationBase<K>::Proxy;

    using VariationalShapeApproximationBase<K>::VariationalShapeApproximationBase;

    virtual ~VariationalShapeApproximation();

  protected:
    /// Computes the Proxy fitting for the L21 metric.
    inline void proxy_fitting() override final;

    /// Computes the energy function for the L21 metric.
    inline Scalar E( const TriangleIdx& T, const Proxy& P ) const override final;
};

//============================================================================

/**
 * @brief This is a specialized version of the VSA for the L21 metric.
 */
template <uint K>
class VariationalShapeApproximation<K, MetricType::LLOYD>
    : public VariationalShapeApproximationBase<K> {
  public:
    using Proxy = typename VariationalShapeApproximationBase<K>::Proxy;

    using VariationalShapeApproximationBase<K>::VariationalShapeApproximationBase;

    virtual ~VariationalShapeApproximation();

  protected:
    /// Computes the Proxy fitting for the L21 metric.
    inline void proxy_fitting() override final;

    /// Computes the energy function for the L21 metric.
    inline Scalar E( const TriangleIdx& T, const Proxy& P ) const override final;
};

//============================================================================

template <uint K, MetricType Type>
using VSA = VariationalShapeApproximation<K, Type>;

template <uint K>
using VSA_L2 = VariationalShapeApproximation<K, MetricType::L2>;

template <uint K>
using VSA_L21 = VariationalShapeApproximation<K, MetricType::L21>;

template <uint K>
using VSA_L21 = VariationalShapeApproximation<K, MetricType::LLOYD>;

} // namespace Geometry
} // namespace Core
} // namespace Ra

#include <Core/Geometry/Approximation/VariationalShapeApproximation.inl>
