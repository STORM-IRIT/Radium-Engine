#ifndef RADIUMENGINE_VOLUME_HPP
#define RADIUMENGINE_VOLUME_HPP

#include <Core/Geometry/AbstractGeometry.hpp>
#include <Core/RaCore.hpp>
#include <Core/Types.hpp>

#define RA_REQUIRE_OPTIONAL
#include <Core/Utils/StdOptional.hpp> // trigger an error if optional is not found
#undef RA_REQUIRE_OPTIONAL

#include <algorithm> // find_if

namespace Ra {
namespace Core {
namespace Geometry {

/// AbstractVolume is an abstract class for Volumetric data (can be parametric or discrete, with
/// dense or sparse storage)
///
/// \note Right now the encoded function \f$f\f$ is dimension 1, ie. \f$f:\Re^3->\Re\f$. If needed,
/// the class can be templated so the built-in typedef #ValueType is replaced by a parameter.
///
/// \note This class could also be modified to encode n-dimensionnal textures.
/// See this for an implementation of n-dimensionnal grids with neighbhorood queries (compile-time
/// loop unrolling): http://github.com/STORM-IRIT/OpenGR/blob/master/src/gr/accelerators/normalset.h
class RA_CORE_API AbstractVolume : public AbstractGeometry
{

  public:
    /// The type of geometry.
    enum VolumeStorageType {
        UNKNOWN         = 1 << 0,
        PARAMETRIC      = 1 << 1, /// <\brief volume defined as a function f : R^2 -> R
        DISCRETE_DENSE  = 1 << 2, /// <\brief volume defined as a function sampled in a regular grid
        DISCRETE_SPARSE = 1 << 3  /// <\brief volume defined as a function sampled in a regular grid
                                  /// with sparse storage
    };

    /// Type of value encoded in the volume
    using ValueType = Scalar;

  protected:
    AbstractVolume( const VolumeStorageType& type );

  public:
    AbstractVolume( const AbstractVolume& data ) = default;
    AbstractVolume& operator=( const AbstractVolume& ) = default;
    ~AbstractVolume()                                  = default;

    /// Return the type of geometry.
    inline VolumeStorageType getType() const;

  protected:
    /// Set the type of geometry.
    inline void setType( const VolumeStorageType& type );

  public:
    /// Get the function value a given position p
    ///
    /// Set to invalid when the query position p is out of bound
    virtual Utils::optional<ValueType> getValue( Eigen::Ref<const Vector3> p ) const = 0;
    ///@}

    /// \name Status queries
    ///@{
    /// Return true if the volume is parametric
    bool isParametric() const;
    /// Return true if the volume is discrete. Can be cast as AbstractDiscreteVolume
    bool isDiscrete() const;
    /// Return true if the volume is dense (implies #isDiscrete to be true)
    bool isDense() const;
    /// Return true if the volume is sparse (implies #isDiscrete to be true)
    bool isSparse() const;
    ///@}

    /// Print info to the Debug output. Need to be extended by child classes
    void displayInfo() const;

  protected:
    /// The type of geometry for the object.
    VolumeStorageType m_type;
};

class RA_CORE_API AbstractDiscreteVolume : public AbstractVolume
{

  public:
    using ValueType = AbstractVolume::ValueType;
    using IndexType = Vector3i;

  protected:
    inline AbstractDiscreteVolume( const VolumeStorageType& type ) :
        AbstractVolume( type ), _size( IndexType::Zero() ), _binSize( Vector3::Zero() ) {}

  public:
    AbstractDiscreteVolume( const AbstractDiscreteVolume& data ) = default;
    AbstractDiscreteVolume& operator=( const AbstractDiscreteVolume& ) = default;
    ~AbstractDiscreteVolume() override                                 = default;

    /// Erases all data, making the geometry empty.
    void clear() override;

    Aabb computeAabb() const override;

    const Vector3i& size() const { return _size; }
    /// \warning Clears existing data
    void setSize( Eigen::Ref<const Vector3i> size ) {
        _size = size;
        updateStorage();
    }
    const Vector3& binSize() const { return _binSize; }
    void setBinSize( Eigen::Ref<const Vector3> binSize ) { _binSize = binSize; }

    inline Utils::optional<ValueType> getBinValue( Eigen::Ref<const IndexType> p ) const {
        if ( auto res = linearIndex( p ) ) return getBinValue( *res );
        return {};
    }

    /// Get the function value a given position p (discrete implementation).
    ///
    /// \see #getBinValue
    inline Utils::optional<ValueType> getValue( Eigen::Ref<const Vector3> p ) const override final {
        return getBinValue( ( p.cwiseQuotient( _binSize ) ).cast<typename IndexType::Scalar>() );
    }

    /// Increment bin p by value
    inline bool addToBin( const ValueType& value, Eigen::Ref<const IndexType> p ) {
        if ( auto res = linearIndex( p ) )
        {
            addToBin( value, *res );
            return true;
        }
        return false;
    }

  protected:
    inline Utils::optional<typename IndexType::Scalar>
    linearIndex( Eigen::Ref<const IndexType> p ) const {
        using Integer = typename IndexType::Scalar;
        if ( ( p.array() >= _size.array() ).any() ) return {};
        return p.dot( IndexType( Integer( 1 ), _size( 0 ), _size( 0 ) * _size( 1 ) ) );
    }
    virtual Utils::optional<ValueType> getBinValue( typename IndexType::Scalar idx ) const = 0;
    virtual void addToBin( const ValueType& value, typename IndexType::Scalar idx )        = 0;

    /// Method called when size or binsize as been updated
    virtual void updateStorage() = 0;

  private:
    IndexType _size;  /// <\brief number of bins per dimension
    Vector3 _binSize; /// <\brief size of a bin per dimension
};

/// Discrete volume data storing values in a regular grid
class RA_CORE_API VolumeGrid : public AbstractDiscreteVolume
{
  public:
    using ValueType = AbstractDiscreteVolume::ValueType;
    using IndexType = AbstractDiscreteVolume::IndexType;

    using Container = std::vector<ValueType>;

  public:
    inline VolumeGrid( const ValueType& defaultValue = ValueType( 0. ) ) :
        AbstractDiscreteVolume( DISCRETE_DENSE ), _defaultValue( defaultValue ) {}
    VolumeGrid( const VolumeGrid& data ) = default;
    VolumeGrid& operator=( const VolumeGrid& ) = default;
    ~VolumeGrid() override                     = default;

    using AbstractDiscreteVolume::addToBin;
    using AbstractDiscreteVolume::getBinValue;

    inline const Container& data() const { return _data; }
    inline Container& data() { return _data; }

    inline void addToAllBin( const ValueType& value ) {
        for ( auto& v : _data )
            v += value;
    }

  protected:
    /// Get the function value a given position p
    inline Utils::optional<ValueType> getBinValue( typename IndexType::Scalar idx ) const override {
        return _data[size_t( idx )];
    }

    inline void addToBin( const ValueType& value, typename IndexType::Scalar idx ) override {
        _data[size_t( idx )] += value;
    }

    /// \warning This method needs to be updated in case we switch to multidimensionnal functions
    inline void updateStorage() override { _data.resize( size_t( size().prod() ), _defaultValue ); }

  private:
    const ValueType _defaultValue;
    Container _data;
}; // class AbstractDiscreteVolume

/// Discrete volume data with sparse storage
///
/// Samples are stored as SparseVolumeData::sample, which stores the bin linear
/// index, and the function value
class RA_CORE_API VolumeSparse : public AbstractDiscreteVolume
{
  public:
    using ValueType = AbstractDiscreteVolume::ValueType;
    using IndexType = AbstractDiscreteVolume::IndexType;
    struct SampleType {
        int index;
        ValueType value;

        inline SampleType( int idx, const ValueType& v ) : index( idx ), value( v ) {}
    };
    using Container = std::vector<SampleType>;

  public:
    inline VolumeSparse() : AbstractDiscreteVolume( DISCRETE_SPARSE ) {}
    VolumeSparse( const VolumeSparse& data ) = default;
    VolumeSparse& operator=( const VolumeSparse& ) = default;
    ~VolumeSparse() override                       = default;

    using AbstractDiscreteVolume::addToBin;
    using AbstractDiscreteVolume::getBinValue;

  protected:
    /// Get the function value a given position p (if the bin exists)
    ///
    /// Returns an invalid value when no sample is registered in the targeted bin.
    ///
    /// Complexity: At most last - first applications of the predicate
    /// (http://en.cppreference.com/w/cpp/algorithm/find)
    inline Utils::optional<ValueType> getBinValue( typename IndexType::Scalar idx ) const override {
        auto res = findBin( idx );
        if ( res != std::end( _data ) ) return res->value;
        return {};
    }

    /// Increment bin p by value
    ///
    /// Create the bin if not already existing
    ///
    /// Complexity: At most last - first applications of the predicate
    /// (http://en.cppreference.com/w/cpp/algorithm/find)
    inline void addToBin( const ValueType& value, typename IndexType::Scalar idx ) override {
        auto res = findBin( idx );
        if ( res != std::end( _data ) )
            res->value += value;
        else
            _data.emplace_back( idx, value );
    }

    inline void updateStorage() override { _data.clear(); }

  private:
    inline Container::iterator findBin( typename IndexType::Scalar idx ) {
        return std::find_if( std::begin( _data ), std::end( _data ), [&idx]( const SampleType& s ) {
            return s.index == idx;
        } );
    }
    inline Container::const_iterator findBin( typename IndexType::Scalar idx ) const {
        return std::find_if( std::begin( _data ), std::end( _data ), [&idx]( const SampleType& s ) {
            return s.index == idx;
        } );
    }

  private:
    Container _data;

}; // class DiscreteVolumeData

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_VOLUME_HPP
