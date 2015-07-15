namespace Ra
{

inline void Entity::setTransform(const Transform& transform)
{
    std::lock_guard<std::mutex> lock(m_transformMutex);
    m_transform = transform;
}

inline void Entity::setTransform(const Matrix4& transform)
{
    std::lock_guard<std::mutex> lock(m_transformMutex);
    m_transform = Transform(transform);
}

inline Transform Entity::getTransform() const
{
    std::lock_guard<std::mutex> lock(m_transformMutex);
    return m_transform;
}

inline Matrix4 Entity::getTransformAsMatrix() const
{
    std::lock_guard<std::mutex> lock(m_transformMutex);
    return m_transform.matrix();
}

} // namespace Ra
