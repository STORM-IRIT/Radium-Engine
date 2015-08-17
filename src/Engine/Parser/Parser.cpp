#include <Engine/Parser/Parser.hpp>

#include <string>
#include <cstdio>

#include <Core/Log/Log.hpp>
#include <Core/Utils/Any.hpp>
#include <Engine/Parser/json11.hpp>

namespace Ra
{
    namespace Engine
    {
        typedef std::map<std::string, json11::Json>::iterator JsonIter;
        void loadEntity(const JsonIter& entity, std::vector<LoadedEntity>& loadedData);

        void Parser::parse(const std::string& file, std::vector<LoadedEntity>& loadedData)
        {
            // HACK(Charly): Temporary workaround of a bug with Any
            Ra::Core::Any::getDeclTypeFor<int        >();
            Ra::Core::Any::getDeclTypeFor<Scalar     >();
            Ra::Core::Any::getDeclTypeFor<bool       >();
            Ra::Core::Any::getDeclTypeFor<std::string>();
            Ra::Core::Any::getDeclTypeFor<double     >();

            std::string err;
            auto data = json11::Json::parse_multi(file, err);
            if (!err.empty())
            {
                LOG(logERROR) << "Error occurred during parsing : " << err;
                return;
            }

            for (const auto& datum : data)
            {
                if (datum.is_object())
                {
                    auto items = datum.object_items();
                    auto entity = items.find("entity");
                    CORE_ASSERT(items.find("entity") != items.end(), "No entity description found");

                    if ((items.size() > 1) || (entity == items.end() && !items.empty()))
                    {
                        LOG(logWARNING) << "The file provided provides some data that will be ignored.";
                    }

                    if (entity == items.end())
                    {
                        LOG(logWARNING) << "The file provided misses some entity descriptions.";
                        continue;
                    }

                    loadEntity(items.find("entity"), loadedData);
                }
            }
        }

        void loadEntity(const JsonIter& entity, std::vector<LoadedEntity>& loadedData)
        {
            LoadedEntity loadedEntity;

            auto entityData = entity->second.object_items();

            auto entityNameIt = entityData.find("name");
            if (entityNameIt == entityData.end())
            {
                LOG(logERROR) << "No name provided for a loaded entity, ignored.";
                return;
            }

            auto entityName = entityNameIt->second;
            CORE_ASSERT(entityName.is_string(), "Entity name is not a string.");
            
            if (!entityName.is_string())
            {
                LOG(logERROR) << "Entity name provided is not in the correct type (should be a string), ignored.";
                return;
            }

            loadedEntity.name = entityName.string_value();

            auto entityPositionIt = entityData.find("position");

            Core::Vector3 vecPosition(0.0, 0.0, 0.0);
            if (entityPositionIt != entityData.end())
            {
                auto entityPosition = entityPositionIt->second;
                if (entityPosition.is_array() || entityPosition.array_items.size() < 3 || 
                    (!entityPosition.array_items()[0].is_number()) ||
                    (!entityPosition.array_items()[1].is_number()) ||
                    (!entityPosition.array_items()[2].is_number()))
                {
                    Scalar x = entityPosition.array_items()[0].number_value();
                    Scalar y = entityPosition.array_items()[1].number_value();
                    Scalar z = entityPosition.array_items()[2].number_value();
                    vecPosition = Core::Vector3(x, y, z);
                }
                else
                {
                    LOG(logWARNING) << "Position for entity provided in the wrong format (should be an array of 3 floats).";
                }
            }
            loadedEntity.position = vecPosition;

            auto entityOrientationIt = entityData.find("orientation");
            Core::Quaternion quatOrientation(1.0, 0.0, 0.0, 0.0);
            if (entityOrientationIt != entityData.end())
            {
                auto entityOrientation = entityOrientationIt->second;
                if (entityOrientation.is_array() || entityOrientation.array_items.size() < 4 ||
                    (!entityOrientation.array_items()[0].is_number()) ||
                    (!entityOrientation.array_items()[1].is_number()) ||
                    (!entityOrientation.array_items()[2].is_number()) ||
                    (!entityOrientation.array_items()[3].is_number()))
                {
                    Scalar w = entityOrientation.array_items()[0].number_value();
                    Scalar x = entityOrientation.array_items()[1].number_value();
                    Scalar y = entityOrientation.array_items()[2].number_value();
                    Scalar z = entityOrientation.array_items()[3].number_value();
                    quatOrientation = Core::Quaternion(w, x, y, z);
                }
                else
                {
                    LOG(logWARNING) << "Orientation for entity provided in the wrong format (should be an array of 4 floats).";
                }
            }
            loadedEntity.orientation = quatOrientation;
            
            auto entityScaleIt = entityData.find("scale");
            Core::Vector3 vecScale(1.0, 1.0, 1.0);
            if (entityScaleIt != entityData.end())
            {
                auto entityScale = entityScaleIt->second;
                if (entityScale.is_array() || entityScale.array_items.size() < 4 ||
                    (!entityScale.array_items()[0].is_number()) ||
                    (!entityScale.array_items()[1].is_number()) ||
                    (!entityScale.array_items()[2].is_number()) ||
                    (!entityScale.array_items()[3].is_number()))
                {
                    Scalar x = entityScale.array_items()[0].number_value();
                    Scalar y = entityScale.array_items()[1].number_value();
                    Scalar z = entityScale.array_items()[2].number_value();
                    vecScale = Core::Vector3(x, y, z);
                }
                else
                {
                    LOG(logWARNING) << "Scale for entity provided in the wrong format (should be an array of 3 floats).";
                }
            }
            loadedEntity.scale = vecScale;

            auto entityComponents = entityData.find("components")->second;
            CORE_ASSERT(entityComponents.is_array(), "Entity components is not an array.");

            for (const auto& c : entityComponents.array_items())
            {
                CORE_ASSERT(c.is_object(), "Entity component is not an object.");

                auto component = c.object_items();
                LOG(logDEBUG) << "Component system : " << component.find("system")->second.string_value();

                auto systemIt = component.find("system");
                if (systemIt == component.end())
                {
                    LOG(logWARNING) << "Component requested without telling the system it belongs to. Ignored.";
                    continue;
                }

                CORE_ASSERT(systemIt->second.is_string(), "System name is not a string.");
                std::string system = systemIt->second.string_value();

                LoadedComponent loadedComponent;
                loadedComponent.system = system;
                
                // We have saved system name, get the data
                component.erase("system");

                typedef std::pair<std::string, Core::Any> AnyByKey;
                std::map<std::string, Core::Any> values;
                for (const auto& cData : component)
                {
                    switch (cData.second.type())
                    {
                        case json11::Json::NUL:
                        {
                            LOG(logDEBUG) << "  " << cData.first << " is nothing.";
                        }
                        break;

                        case json11::Json::NUMBER:
                        {
                            values.insert(AnyByKey(cData.first, Scalar(cData.second.number_value())));
                        }
                        break;

                        case json11::Json::BOOL:
                        {
                            values.insert(AnyByKey(cData.first, bool(cData.second.bool_value())));
                        }
                        break;

                        case json11::Json::STRING:
                        {
                            values.insert(AnyByKey(cData.first, cData.second.string_value()));
                        }
                        break;

                        case json11::Json::ARRAY:
                        {
                            LOG(logWARNING) << "  " << cData.first << " : Arrays loading is not handled yet.";
                        }
                        break;

                        case json11::Json::OBJECT:
                        {
                            LOG(logWARNING) << "  " << cData.first << " : Objects loading is not handled yet.";
                        } 
                        break;
                    }
                
                    loadedComponent.data = values;
                }

                loadedEntity.data.push_back(loadedComponent);
            }

            loadedData.push_back(loadedEntity);

            /*
            switch (json.type())
            {
                case json11::Json::OBJECT:
                {
                    if (key != "")
                    {
                        LOG(logDEBUG) << spaces << key << " - object";
                        spaces = spaces + "  ";
                    }
                    for (const auto& item : json.object_items())
                    {
                        runThroughHierarchy(item.second, item.first);
                    }
                    if (key != "")
                    {
                        spaces.pop_back();
                        spaces.pop_back();
                    }
                } break;

                case json11::Json::BOOL:
                {
                    LOG(logDEBUG) << spaces << key << " - bool : " << json.bool_value();
                } break;

                case json11::Json::NUMBER:
                {
                    LOG(logDEBUG) << spaces << key << " - number : " << json.number_value();
                } break;

                case json11::Json::STRING:
                {
                    LOG(logDEBUG) << spaces << key << " - string : " << json.string_value();
                } break;

                case json11::Json::ARRAY:
                {
                    LOG(logDEBUG) << spaces << key << " - array : " << json.array_items().size() << " items";
                    spaces = spaces + "  ";
                    static int i = 0;
                    for (const auto& item : json.array_items())
                    {
                        LOG(logDEBUG) << spaces << "Item " << i++;
                        spaces = spaces + "  ";
                        runThroughHierarchy(item, "");
                        spaces.pop_back();
                        spaces.pop_back();
                    }
                    i=0;
                    spaces.pop_back();
                    spaces.pop_back();
                } break;
            }
            */
        }
    }
}