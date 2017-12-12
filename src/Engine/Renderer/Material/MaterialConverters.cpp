#include <Engine/Renderer/Material/MaterialConverters.hpp>

#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>

#include <map>
///////////////////////////////////////////////
////        Material converter system       ///
///////////////////////////////////////////////


namespace Ra {
  namespace Engine {
    namespace MaterialConverterSystem {
      /// Map that stores each conversion function
      static std::map<std::string, std::function<RadiumMaterialPtr(AssetMaterialPtr)> > MaterialConverterRegistry;

      bool registerMaterialConverter(const std::string &name, ConverterFunction converter)
      {
          auto result = MaterialConverterRegistry.insert({name, converter});
          return result.second;
      }

      bool removeMaterialConverter(const std::string &name)
      {
          std::size_t removed = MaterialConverterRegistry.erase(name);
          return (removed == 1);
      }

      std::pair<bool, ConverterFunction> getMaterialConverter(const std::string &name)
      {
          auto search = MaterialConverterRegistry.find(name);
          if (search != MaterialConverterRegistry.end())
          {
              return { true, search->second };
          }
          auto result = std::make_pair(false,
                                       [name](AssetMaterialPtr) -> RadiumMaterialPtr
                                       {
                                           LOG(logERROR) << "Required material converter " << name << " not found!";
                                           return nullptr;
                                       }
          );
          return result;
      }

///////////////////////////////////////////////
////        Radium defined converters       ///
///////////////////////////////////////////////

      Material *MaterialConverter::operator()(const Ra::Asset::MaterialData *toconvert)
      {
          LOG(logERROR) << "Trying to convert a abstract material ... !";
          return nullptr;
      }

      Material *BlinnPhongMaterialConverter::operator()(const Ra::Asset::MaterialData *toconvert)
      {
          Ra::Engine::BlinnPhongMaterial *result = new Ra::Engine::BlinnPhongMaterial(toconvert->getName());

          auto source = static_cast<const Ra::Asset::BlinnPhongMaterialData *>(toconvert);

          if (source->hasDiffuse())
              result->m_kd = source->m_diffuse;
          if (source->hasSpecular())
              result->m_ks = source->m_specular;
          if (source->hasShininess())
              result->m_ns = source->m_shininess;
          if (source->hasOpacity())
              result->m_alpha = source->m_opacity;

#ifdef RADIUM_WITH_TEXTURES
          if (source->hasDiffuseTexture())
              result->addTexture(Ra::Engine::BlinnPhongMaterial::TextureType::TEX_DIFFUSE, source->m_texDiffuse);
          if (source->hasSpecularTexture())
              result->addTexture(Ra::Engine::BlinnPhongMaterial::TextureType::TEX_SPECULAR, source->m_texSpecular);
          if (source->hasShininessTexture())
              result->addTexture(Ra::Engine::BlinnPhongMaterial::TextureType::TEX_SHININESS, source->m_texShininess);
          if (source->hasOpacityTexture())
              result->addTexture(Ra::Engine::BlinnPhongMaterial::TextureType::TEX_ALPHA, source->m_texOpacity);
          if (source->hasNormalTexture())
              result->addTexture(Ra::Engine::BlinnPhongMaterial::TextureType::TEX_NORMAL, source->m_texNormal);
#endif
          return result;
      }
    }
  }
}
