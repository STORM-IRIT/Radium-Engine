#ifndef RADIUMENGINE_MATERIALDATA_HPP
#define RADIUMENGINE_MATERIALDATA_HPP
#include <string>
#include <vector>

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>

#include <Core/File/AssetData.hpp>

namespace Ra {
  namespace Asset {

    class RA_CORE_API MaterialData : public AssetData
    {
    public:
        /// ENUM FOR SUPPORTED MATERIALS
        enum MaterialType {
            UNKNOWN     = 1 << 0,
            BLINN_PHONG = 1 << 1,
            DISNEY      = 1 << 2,
            MATTE       = 1 << 3,
            METAL       = 1 << 4,
            MIRROR      = 1 << 5,
            PLASTIC     = 1 << 6,
            SUBSTRATE   = 1 << 7,
            TRANSLUCENT = 1 << 8
        };
        // SUPPORTED MATERIALS
        struct BlinnPhongMaterial
        {
            BlinnPhongMaterial();
            BlinnPhongMaterial(const BlinnPhongMaterial& o ) = default;
            ~BlinnPhongMaterial() = default;

            /// QUERY
            inline bool hasDiffuse() const;
            inline bool hasSpecular() const;
            inline bool hasShininess() const;
            inline bool hasOpacity() const;
            inline bool hasDiffuseTexture() const;
            inline bool hasSpecularTexture() const;
            inline bool hasShininessTexture() const;
            inline bool hasNormalTexture() const;
            inline bool hasOpacityTexture() const;

            /// DEBUG
            inline void displayInfo() const;

            /// VARIABLE
            Core::Color m_diffuse;
            Core::Color m_specular;
            Scalar      m_shininess;
            Scalar      m_opacity;
            std::string m_texDiffuse;
            std::string m_texSpecular;
            std::string m_texShininess;
            std::string m_texNormal;
            std::string m_texOpacity;
            bool        m_hasDiffuse;
            bool        m_hasSpecular;
            bool        m_hasShininess;
            bool        m_hasOpacity;
            bool        m_hasTexDiffuse;
            bool        m_hasTexSpecular;
            bool        m_hasTexShininess;
            bool        m_hasTexNormal;
            bool        m_hasTexOpacity;
        };

        /// MATERIALDATA

        MaterialData(const std::string&  name = "",
                     const MaterialType& type = UNKNOWN);

        MaterialData( const MaterialData& material );
        MaterialData& operator=( const MaterialData& material );

        ~MaterialData();

        /// NAME
        inline void setName( const std::string& name );

        /// TYPE
        inline MaterialType getType() const;
        inline void setType( const MaterialType& type );

        /// QUERY
        inline const BlinnPhongMaterial &getBlinnPhong() const;
        inline void setBlinnPhong(const BlinnPhongMaterial &o);

        /// DEBUG
        inline void displayInfo() const;


    private:
        MaterialType m_type;
        union
        {
            BlinnPhongMaterial m_BlinnPhong;
            // TODO : add here others materials
        };


    };

  } // namespace Asset
} // namespace Ra

#include <Core/File/MaterialData.inl>

#endif //RADIUMENGINE_MATERIALDATA_HPP
