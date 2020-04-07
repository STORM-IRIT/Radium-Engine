/**
 * Test if the given pass is dirty (openGL state not updated)
 * @param pass The index of the pass
 * @return Tru if pass must be updated.
 */
inline bool RenderTechnique::isDirty( Core::Utils::Index pass ) const {
    return m_dirtyBits & ( 1 << pass );
}

/**
 * Set the given pass dirty (openGL state updated)
 * @param pass The index of the pass
 */
inline void RenderTechnique::setDirty( Core::Utils::Index pass ) {
    m_dirtyBits |= ( 1 << pass );
}

inline void RenderTechnique::clearDirty( Core::Utils::Index pass ) {
    m_dirtyBits &= ~( 1 << pass );
}

inline bool RenderTechnique::hasConfiguration( Core::Utils::Index pass ) const {
    return m_setPasses & ( 1 << pass );
}

inline void RenderTechnique::setConfiguration( Core::Utils::Index pass ) {
    m_setPasses |= ( 1 << pass );
}

inline void RenderTechnique::clearConfiguration( Core::Utils::Index pass ) {
    m_setPasses &= ~( 1 << pass );
}

inline const ShaderConfiguration&
RenderTechnique::getConfiguration( Core::Utils::Index pass ) const {
    return m_activePasses[pass].first;
}
