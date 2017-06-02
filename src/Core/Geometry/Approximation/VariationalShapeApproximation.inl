#include <Core/Geometry/Approximation/VariationalShapeApproximation.hpp>

#include <set>
#include <random>
#include <Eigen/Eigenvalues>
#include <Core/Log/Log.hpp>
#include <Core/Geometry/Triangle/TriangleOperation.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
inline VariationalShapeApproximationBase< K_Region >::VariationalShapeApproximationBase( const Mesh& mesh ) :
    m_mesh( mesh ),
    m_queue(),
    m_region(),
    m_proxy(),
    m_init( false ) {}



//////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
inline VariationalShapeApproximationBase< K_Region >::~VariationalShapeApproximationBase() {}



//////////////////////////////////////////////////////////////////////////////
// INIT
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
inline void VariationalShapeApproximationBase< K_Region >::init() {
    this->compute_data();
    this->compute_seed();
    this->m_init = true;
}



template < uint K_Region >
inline bool VariationalShapeApproximationBase< K_Region >::initialized() const {
    return m_init;
}



//////////////////////////////////////////////////////////////////////////////
// EXECUTION
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
inline void VariationalShapeApproximationBase< K_Region >::exec( const uint iteration ) {
    if( !this->initialized() ) {
        CORE_WARN_IF( false, "V.S.A. NOT INITIALIZED" );
        return;
    }
    LOG( logDEBUG ) << "Computing V.S.A. ...";
    for( uint i = 0; i < iteration; ++i )  {
        this->proxy_fitting();
        this->geometry_partitioning();
    }
    LOG( logDEBUG ) << "V.S.A. completed.";
}



template < uint K_Region >
template < uint Iteration >
inline void VariationalShapeApproximationBase< K_Region >::exec() {
    if( !this->initialized() ) {
        CORE_WARN_IF( false, "V.S.A. NOT INITIALIZED" );
        return;
    }
    LOG( logDEBUG ) << "Computing V.S.A. ...";
    for( uint i = 0; i < Iteration; ++i )  {
        this->proxy_fitting();
        this->geometry_partitioning();
    }
    LOG( logDEBUG ) << "V.S.A. completed.";
}



//////////////////////////////////////////////////////////////////////////////
// REGION
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
inline const typename VariationalShapeApproximationBase< K_Region >::Region& VariationalShapeApproximationBase< K_Region >::region( const uint i ) const {
    return m_region.at( i );
}



//////////////////////////////////////////////////////////////////////////////
// PROXY
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
inline const typename VariationalShapeApproximationBase< K_Region >::Proxy& VariationalShapeApproximationBase< K_Region >::proxy( const uint i ) const {
    return m_proxy.at( i );
}



//////////////////////////////////////////////////////////////////////////////
// COLOR
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
inline void VariationalShapeApproximationBase< K_Region >::create_region_color() {
    for( uint i = 0; i < K; ++i ) {
        const Scalar q = Scalar( i ) / static_cast< Scalar >( K - 1 );
        for( const auto& T : this->m_region[i] ) {
            this->m_fvalue[T]  = q;
        }
    }
}



template < uint K_Region >
inline void VariationalShapeApproximationBase< K_Region >::create_energy_color() {
    for( uint i = 0; i < K; ++i ) {
        for( const auto& T : this->m_region[i] ) {
            this->m_fvalue[T]  = this->E( T, this->m_proxy[i] );
        }
    }
}



template < uint K_Region >
inline void VariationalShapeApproximationBase< K_Region >::shuffle_regions() {
    //This function was made in order to have a chance for neighboring regions
    //to have distant IDs, hence having a different color.
    std::array< uint, K > id;
    RegionList tmp_region;
    ProxyList  tmp_proxy;
    for( uint i = 0; i < K; ++i ) {
        id[i] = i;
    }
    std::random_shuffle( id.begin(), id.end() );
    for( uint i = 0; i < K; ++i ) {
        tmp_region[i] = this->m_region[id[i]];
        tmp_proxy[i]  = this->m_proxy[id[i]];
    }
    std::swap( tmp_region, this->m_region );
    std::swap( tmp_proxy,  this->m_proxy  );
    for( uint i = 0; i < K; ++i ) {
        for( const auto& T : this->m_region[i] ) {
            this->m_fregion[T] = i;
        }
    }
}



//////////////////////////////////////////////////////////////////////////////
// DATA
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
inline void VariationalShapeApproximationBase< K_Region >::compute_data() {
    const uint size = this->m_mesh.m_triangles.size();
    this->m_fbary.resize( size );
    this->m_fnormal.resize( size );
    this->m_farea.resize( size );
    this->m_fregion.resize( size, -1 );
    this->m_fvisited.resize( size, false );
    this->m_fvalue.resize( 0 );
    this->m_fadj.resize( size );

    std::vector< std::vector< TriangleIdx > > vadj( this->m_mesh.m_vertices.size() );
    for( uint t = 0; t < size; ++t ) {
        const Triangle& T    = this->m_mesh.m_triangles[t];
        const uint      i    = T[0];
        const uint      j    = T[1];
        const uint      k    = T[2];
        const Vector3   v[3] = { this->m_mesh.m_vertices[i], this->m_mesh.m_vertices[j], this->m_mesh.m_vertices[k] };
        this->m_fbary[t] = triangleBarycenter( v[0], v[1], v[2] );
        this->m_fnormal[t] = triangleNormal( v[0], v[1], v[2] );
        this->m_farea[t] = triangleArea( v[0], v[1], v[2] );
        vadj[i].push_back( t );
        vadj[j].push_back( t );
        vadj[k].push_back( t );
    }

    for( const auto& list : vadj ) {
        for( uint i = 0; i < list.size(); ++i ) {
            for( uint j = i+1; j < list.size(); ++j ) {
                this->m_fadj[list[i]].insert( list[j] );
                this->m_fadj[list[j]].insert( list[i] );
            }
        }
    }

}



//////////////////////////////////////////////////////////////////////////////
// SEED
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
inline void VariationalShapeApproximationBase< K_Region >::compute_seed() {
    std::set< TriangleIdx > t;
    std::default_random_engine g(time(0));
    std::uniform_int_distribution< TriangleIdx > rnd( 0, this->m_mesh.m_triangles.size()-1 );
    while( t.size() != K ) {
        t.insert( rnd(g) );
    }
    for( uint i = 0; i < K; ++i ) {
        m_region[i].push_back( *t.begin() );
        t.erase( t.begin() );
    }
}



//////////////////////////////////////////////////////////////////////////////
// GEOMETRY PARTITIONING
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
inline void VariationalShapeApproximationBase< K_Region >::geometry_partitioning() {
    //Reset the visited flag
    for( TriangleIdx T = 0; T < this->m_mesh.m_triangles.size(); ++T ) {
        this->m_fvisited[T] = false;
    }

    //Force the queue to be empty
    this->m_queue = PriorityQueue();

    //Choose best triangle in each region
    for( uint i = 0; i < K; ++i ) {
        assert( !this->m_region[i].empty() );
        Energy min_e = std::numeric_limits< Energy >::max();
        TriangleIdx T;
        for( const auto& t : this->m_region[i] ) {
            const Energy e = this->E( t, this->m_proxy[i] );
            if( e < min_e ) {
                min_e = e;
                T = t;
            }
        }
        this->add_neighbors_to_queue( T, i );

        // Clear the region
        this->m_region[i].clear();
        this->m_region[i].push_back( T );
        this->m_fregion[T]  = i;
        this->m_fvisited[T] = true;
    }

    //While the queue is not empty
     while( !this->m_queue.empty() ) {
         const auto q = this->m_queue.top();
         this->m_queue.pop();
         const TriangleIdx t = q.second.first;
         if( this->m_fvisited[t] ) {
             continue;
         }
         const uint R = q.second.second;
         this->m_region[R].push_back( t );
         this->m_fregion[t]  = R;
         this->m_fvisited[t] = true;

         this->add_neighbors_to_queue( t, R );
     }
}



template < uint K_Region >
inline void VariationalShapeApproximationBase< K_Region >::add_neighbors_to_queue( const TriangleIdx& T, const uint proxy_id ) {
    for( const auto& r : this->m_fadj[T] ) {
        this->m_queue.push( QueueEntry( this->E( r, this->m_proxy[proxy_id] ), Pair( r, proxy_id ) ) );
    }
}






//============================================================================
//============================================================================
//============================================================================






//////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region, MetricType Type >
VariationalShapeApproximation< K_Region, Type >::~VariationalShapeApproximation() {}



//////////////////////////////////////////////////////////////////////////////
// PROXY FITTING
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region, MetricType Type >
inline void VariationalShapeApproximation< K_Region, Type >::proxy_fitting() {
    static const Scalar c = 2.0 / 72.0;
    Matrix3 m;
    m << 10.0, 7.0, 0.0, 7.0, 10.0, 0.0, 0.0, 0.0, 0.0;
    for( uint i = 0; i < this->K; ++i ) {

        Scalar area_sum         = 0;
        this->m_proxy[i].first  = Vector3::Zero();
        Matrix3 Q               = Matrix3::Zero();

        for( const auto& T : this->m_region[i] ) {
            const Vector3  v[3] = { this->m_mesh.m_vertices[this->m_mesh.m_triangles[T][0]],
                                    this->m_mesh.m_vertices[this->m_mesh.m_triangles[T][1]],
                                    this->m_mesh.m_vertices[this->m_mesh.m_triangles[T][2]] };
            const Vector3& g    = this->m_fbary[T];

            Matrix3 M;
            M.row(0) = v[1] - v[0];
            M.row(1) = v[2] - v[0];
            M.row(2) = Vector3::Zero();

            Q += ( c * this->m_farea[T] * M * m * M.transpose() ) + ( this->m_farea[T] * g * g.transpose() );

            this->m_proxy[i].first += this->m_farea[T] * g;
            area_sum               += this->m_farea[T];
        }
        this->m_proxy[i].first /= area_sum;
        Q -= area_sum * this->m_proxy[i].first * this->m_proxy[i].first.transpose();
        this->m_proxy[i].second = Eigen::SelfAdjointEigenSolver<Matrix3>( Q ).eigenvectors().col( 0 ).normalized();
    }
}



//////////////////////////////////////////////////////////////////////////////
// ENERGY
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region, MetricType Type >
inline Scalar VariationalShapeApproximation< K_Region, Type >::E( const TriangleIdx& T, const Proxy& P ) const {
    static const Scalar c = 1.0 / 6.0;
    const Vector3 v[3] = { this->m_mesh.m_vertices[this->m_mesh.m_triangles[T][0]],
                           this->m_mesh.m_vertices[this->m_mesh.m_triangles[T][1]],
                           this->m_mesh.m_vertices[this->m_mesh.m_triangles[T][2]] };
    const Vector3 p    = P.first;
    const Vector3 n    = P.second;
    Scalar d[3];
    d[0] = std::abs( n.dot( p - v[0] ) );
    d[1] = std::abs( n.dot( p - v[1] ) );
    d[2] = std::abs( n.dot( p - v[2] ) );
    return ( c * ( ( d[0] * d[0] ) +
                   ( d[1] * d[1] ) +
                   ( d[2] * d[2] ) +
                   ( d[0] * d[1] ) +
                   ( d[0] * d[2] ) +
                   ( d[1] * d[2] ) ) * this->m_farea[T] );
}






//============================================================================
//============================================================================
//============================================================================






//////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
VariationalShapeApproximation< K_Region, MetricType::L21 >::~VariationalShapeApproximation() {}



//////////////////////////////////////////////////////////////////////////////
// PROXY FITTING
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
inline void VariationalShapeApproximation< K_Region, MetricType::L21 >::proxy_fitting() {
    for( uint i = 0; i < this->K; ++i ) {
        Scalar area_sum = 0;
        this->m_proxy[i].first  = Vector3::Zero();
        this->m_proxy[i].second = Vector3::Zero();
        for( const auto& T : this->m_region[i] ) {
            this->m_proxy[i].first  += this->m_farea[T] * this->m_fbary[T];
            this->m_proxy[i].second += this->m_farea[T] * this->m_fnormal[T];
            area_sum                += this->m_farea[T];
        }
        this->m_proxy[i].first  /= area_sum;
        this->m_proxy[i].second = this->m_proxy[i].second.normalized();
    }
}



//////////////////////////////////////////////////////////////////////////////
// ENERGY
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
inline Scalar VariationalShapeApproximation< K_Region, MetricType::L21 >::E( const TriangleIdx& T, const Proxy& P ) const {
    const Scalar  a      = this->m_farea[T];
    const Vector3 n      = this->m_fnormal[T];
    const Vector3 d      = n - P.second;
    const Scalar  result = a * d.squaredNorm();
    return result;
}






//============================================================================
//============================================================================
//============================================================================






//////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
VariationalShapeApproximation< K_Region, MetricType::LLOYD >::~VariationalShapeApproximation() {}



//////////////////////////////////////////////////////////////////////////////
// PROXY FITTING
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
inline void VariationalShapeApproximation< K_Region, MetricType::LLOYD >::proxy_fitting() {
    for( uint i = 0; i < this->K; ++i ) {
        this->m_proxy[i].first  = Vector3::Zero();
        this->m_proxy[i].second = Vector3::Zero();
        for( const auto& T : this->m_region[i] ) {
            this->m_proxy[i].first  += this->m_fbary[T];
            this->m_proxy[i].second += this->m_fnormal[T];
        }
        this->m_proxy[i].first  /= this->m_region[i].size();
        this->m_proxy[i].second  = this->m_proxy[i].second.normalized();
    }
}



//////////////////////////////////////////////////////////////////////////////
// ENERGY
//////////////////////////////////////////////////////////////////////////////
template < uint K_Region >
inline Scalar VariationalShapeApproximation< K_Region, MetricType::LLOYD >::E( const TriangleIdx& T, const Proxy& P ) const {
    const Vector3& b     = this->m_fbary[T];
    const Scalar  result = (b - P.first).norm();
    return result;
}



} // namespace Geometry
} // namespace Core
} // namespace Ra


