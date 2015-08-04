#ifndef RADIUMENGINE_MATERIAL_HPP
#define RADIUMENGINE_MATERIAL_HPP

#include <map>
#include <string>

#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>

namespace Ra { namespace Engine { class Texture; } }
namespace Ra { namespace Engine { class ShaderProgram; } }

namespace Ra { namespace Engine {

class RA_API Material
{
public:
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

    inline const std::string& getName() const;

    inline void setKd(const Core::Color& kd);
    inline void setKs(const Core::Color& ks);
	inline void setNs(Scalar ns);

    inline const Core::Color& getKd() const;
    inline const Core::Color& getKs() const;
	inline Scalar getNs() const;

    inline void addTexture(const TextureType& type, Texture* texture);
    inline void addTexture(const TextureType& type, const std::string& texture);
    inline Texture* getTexture(const TextureType& type) const;

private:
    Core::Color m_kd;
    Core::Color m_ks;
	Scalar m_ns;

    std::string m_name;

    bool m_isDirty;

    std::map<TextureType, Texture*> m_textures;
    std::map<TextureType, std::string> m_pendingTextures;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/RenderTechnique/Material.inl>

#endif
