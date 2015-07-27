namespace Ra
{

inline const std::string& Engine::Material::getName() const
{
    return m_name;
}

inline void Engine::Material::setDefaultShaderProgram(
        const ShaderConfiguration& shader)
{
    m_defaultShaderConfiguration = shader;
    m_isDirty = true;
}

inline void Engine::Material::setContourShaderProgram(
        const ShaderConfiguration& shader)
{
    m_contourShaderConfiguration = shader;
    m_isDirty = true;
}

inline void Engine::Material::setWireframeShaderProgram(
        const ShaderConfiguration& shader)
{
    m_wireframeShaderConfiguration = shader;
    m_isDirty = true;
}

inline Engine::ShaderProgram* Engine::Material::getCurrentShaderProgram() const
{
    return m_currentShader;
}

inline void Engine::Material::changeMode(const Material::MaterialMode& mode)
{
    m_mode = mode;
    m_isDirty = true;
}

inline void Engine::Material::setKd(const Core::Color& kd)
{
    m_kd = kd;
}

inline void Engine::Material::setKs(const Core::Color& ks)
{
    m_ks = ks;
}

inline void Engine::Material::setNs(Scalar ns)
{
	m_ns = ns;
}

inline void Engine::Material::setMaterialType(const MaterialType& type)
{
    m_materialType = type;
}

inline void Engine::Material::addTexture(const TextureType& type, Texture* texture)
{
    // FIXME(Charly): Check if already present ?
    m_textures[type] = texture;
}

inline void Engine::Material::addTexture(const TextureType& type, const std::string& texture)
{
    m_pendingTextures[type] = texture;
    m_isDirty = true;
}

inline const Core::Color& Engine::Material::getKd() const
{
    return m_kd;
}

inline const Core::Color& Engine::Material::getKs() const
{
    return m_ks;
}

inline Scalar Engine::Material::getNs() const
{
	return m_ns;
}

inline const Engine::Material::MaterialType& Engine::Material::getMaterialType() const
{
    return m_materialType;
}

inline Engine::Texture* Engine::Material::getTexture(const TextureType &type) const
{
    Texture* tex = nullptr;

    auto it = m_textures.find(type);
    if (it != m_textures.end())
    {
        tex = it->second;
    }

    return tex;
}

} // namespace Ra
