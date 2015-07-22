#ifndef RADIUMENGINE_MATERIAL_HPP
#define RADIUMENGINE_MATERIAL_HPP

#include <map>
#include <string>

#include <Core/Math/Vector.hpp>

namespace Ra { namespace Engine { class Texture; } }
namespace Ra { namespace Engine { class ShaderProgram; } }

namespace Ra { namespace Engine {

class Material
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
        TEX_ALPHA
    };

public:
    RA_CORE_ALIGNED_NEW

    explicit Material(const std::string& name);
    ~Material();

    void bind();

    inline void changeMode(const MaterialMode& mode);

    inline const std::string& getName() const;

    inline void setDefaultShaderProgram(ShaderProgram* shader);
    inline void setContourShaderProgram(ShaderProgram* shader);
    inline void setWireframeShaderProgram(ShaderProgram* shader);
    inline ShaderProgram* getCurrentShaderProgram() const;

    inline void setKd(const Core::Color& kd);
    inline void setKs(const Core::Color& ks);
    inline void setMaterialType(const MaterialType& type);

    inline const Core::Color& getKd() const;
    inline const Core::Color& getKs() const;
    inline const MaterialType& getMaterialType() const;

    inline void addTexture(const TextureType& type, Texture* texture);
    inline Texture* getTexture(const TextureType& type) const;

private:
    Core::Color m_kd;
    Core::Color m_ks;

    std::string m_name;

    ShaderProgram* m_currentShader;
    ShaderProgram* m_defaultShader;
    ShaderProgram* m_contourShader;
    ShaderProgram* m_wireframeShader;

    MaterialType  m_materialType;

    std::map<TextureType, Texture*> m_textures;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Material/Material.inl>

#endif
