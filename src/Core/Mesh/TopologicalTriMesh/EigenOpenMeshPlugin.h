#ifdef RELEASE_EIGEN_PLUGIN_PROTECTION_AGAINST_DIRECT_INCLUSION

EIGEN_STRONG_INLINE Scalar length() const
{
    return this->norm();
}

EIGEN_STRONG_INLINE Matrix& vectorize( Scalar v )
{
    fill( v );
    return *this;
}

EIGEN_STRONG_INLINE Matrix( Scalar value )
{
    fill( value );
}

#endif
