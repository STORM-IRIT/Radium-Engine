namespace Ra
{

inline void Entity::setTransform(const Core::Transform& transform)
{
    std::lock_guard<std::mutex> lock(m_transformMutex);
    m_transform = transform;
}

inline void Entity::setTransform(const Core::Matrix4& transform)
{
    std::lock_guard<std::mutex> lock(m_transformMutex);
    m_transform = Core::Transform(transform);
}

inline Core::Transform Entity::getTransform() const
{
    std::lock_guard<std::mutex> lock(m_transformMutex);
    return m_transform;
}

inline Core::Matrix4 Entity::getTransformAsMatrix() const
{
    std::lock_guard<std::mutex> lock(m_transformMutex);
    return m_transform.matrix();
}

} // namespace Ra
