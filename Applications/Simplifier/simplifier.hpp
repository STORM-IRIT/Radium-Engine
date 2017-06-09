#pragma once

#include <Engine/RadiumEngine.hpp>
#include <Engine/System/System.hpp>
#include <Engine/Entity/Entity.hpp>

#include <Engine/Managers/MeshContactManager/MeshContactManager.hpp>
#include <Engine/Managers/MeshContactManager/MeshContactElement.hpp>

class SimplifierComponent : public Ra::Engine::Component {

public:
    SimplifierComponent(const std::string& name);

    void initialize() override;
    void setupProgressiveMesh();
    void handleMeshLoading( const Ra::Asset::GeometryData* data );
    void exportMesh(std::string filename);
    Ra::Engine::MeshContactElement* getMeshContactElement();

private:
    std::string m_contentsName;
    Ra::Engine::MeshContactElement* m_meshContactElement;

};


class SimplifierSystem : public Ra::Engine::System {
public:
    SimplifierSystem();
    void handleAssetLoading(Ra::Engine::Entity *entity, const Ra::Asset::FileData *fileData);
    virtual void generateTasks(Ra::Core::TaskQueue *q, const Ra::Engine::FrameInfo &info) override;
    Ra::Engine::MeshContactManager* getMeshContactManager();
    std::vector<SimplifierComponent*> getSimplifierComponents();

private:

     Ra::Engine::MeshContactManager* m_meshContactManager;
     std::vector<SimplifierComponent*> m_simplifierComponents;
};
