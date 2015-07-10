namespace Ra
{
    template<typename T>
    Tex3D<T>::Tex3D(const Vector3i& resolution,
                 const Vector3& start,
                 const Vector3& end)
            : Grid<T, 3>(resolution), m_aabb(start, end)
    {
        const Vector3 quotient = (resolution - Vector3i(1,1,1)).cast<Scalar>();
        m_cellSize = m_aabb.sizes().cwiseQuotient(quotient);
    }

    template<typename T>
    Tex3D<T>::Tex3D(const Vector3i& resolution, const Aabb& aabb)
            : Grid<T, 3>(resolution), m_aabb(aabb)
    {
        const Vector3 quotient = (resolution - Vector3i(1, 1, 1)).cast<Scalar>();
        m_cellSize = m_aabb.sizes().cwiseQuotient(quotient);
    }

    template<typename T>
    inline const Aabb& Tex3D<T>::getAabb() const
    {
        return m_aabb;
    }

    template<typename T>
    inline T fetch(const Vector3& v)
    {

        //TODO: optimized version of trilinear interpolation.
    }
}