#ifndef RADIUMENGINE_MATERIAL_HPP
#define RADIUMENGINE_MATERIAL_HPP

#include <map>
#include <string>

#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Renderer/Shader/ShaderConfiguration.hpp>

namespace Ra { namespace Engine { class Texture; } }
namespace Ra { namespace Engine { class ShaderProgram; } }

namespace Ra { namespace Engine {

class RA_API Material
{
public:
    enum MaterialType
    {
        MAT_OPAQUE,
        MAT_TRANSPARENT
    };

    enum MaterialMode
    {
        MODE_DEFAULT,
        MODE_CONTOUR,
        MODE_WIREFRAME
    };

    enum TextureType
    {
        TEX_DIFFUSE,
        TEX_SPECULAR,
        TEX_NORMAL,
		TEX_SHININESS,
        TEX_ALPHA
    };

public:
    RA_CORE_ALIGNED_NEW

    explicit Material(const std::string& name);
    ~Material();

    void updateGL();

    void bind();

    /**
     * @brief Bind the material given a shader. This can be useful for a
     * deferred renderer, where only one shader is used in the geometry pass
     * (Which will also register colors, etc)
     *
     * @param shader
     */
    void bind(ShaderProgram* shader);

    inline void changeMode(const MaterialMode& mode);

    inline const std::string& getName() const;

    inline void setDefaultShaderProgram(const ShaderConfiguration& shader);
    inline void setContourShaderProgram(const ShaderConfiguration& shader);
    inline void setWireframeShaderProgram(const ShaderConfiguration& shader);
    inline ShaderProgram* getCurrentShaderProgram() const;

    inline void setKd(const Core::Color& kd);
    inline void setKs(const Core::Color& ks);
	inline void setNs(Scalar ns);
    inline void setMaterialType(const MaterialType& type);

    inline const Core::Color& getKd() const;
    inline const Core::Color& getKs() const;
	inline Scalar getNs() const;
    inline const MaterialType& getMaterialType() const;

    inline void addTexture(const TextureType& type, Texture* texture);
    inline void addTexture(const TextureType& type, const std::string& texture);
    inline Texture* getTexture(const TextureType& type) const;

private:
    Core::Color m_kd;
    Core::Color m_ks;
	Scalar m_ns;

    std::string m_name;

    bool m_isDirty;
    MaterialMode m_mode;

    ShaderConfiguration m_defaultShaderConfiguration;
    ShaderConfiguration m_contourShaderConfiguration;
    ShaderConfiguration m_wireframeShaderConfiguration;

    ShaderProgram* m_currentShader;
    ShaderProgram* m_defaultShader;
    ShaderProgram* m_contourShader;
    ShaderProgram* m_wireframeShader;

    MaterialType  m_materialType;

    std::map<TextureType, Texture*> m_textures;
    std::map<TextureType, std::string> m_pendingTextures;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Material/Material.inl>

#endif
