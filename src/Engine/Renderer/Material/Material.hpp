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

    enum TextureType
    {
        TEX_DIFFUSE,
        TEX_SPECULAR,
        TEX_NORMAL,
        TEX_ALPHA
    };

public:
    explicit Material(const std::string& name);
    ~Material();

    void bind();

    inline const std::string& getName() const;

    inline void setShaderProgram(ShaderProgram* shader);
    inline ShaderProgram* getShaderProgram() const;

    inline void setKd(const Core::Color& kd);
    inline void setKs(const Core::Color& ks);
    inline void setMaterialType(const MaterialType& type);

    inline const Core::Color& getKd() const;
    inline const Core::Color& getKs() const;
    inline const MaterialType& getMaterialType() const;

    inline void addTexture(const TextureType& type, Texture* texture);
    inline Texture* getTexture(const TextureType& type) const;

private:
    std::string m_name;

    ShaderProgram* m_shader;

    Core::Color m_kd;
    Core::Color m_ks;

    MaterialType  m_materialType;

    std::map<TextureType, Texture*> m_textures;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Material/Material.inl>

#endif
