// Include Radium base application and its simple Gui
#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindowFactory.hpp>
#include <Gui/Viewer/Viewer.hpp>

// include the Engine/entity/component interface
#include <Core/Asset/BlinnPhongMaterialData.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Engine/Data/TextureManager.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/GeometryComponent.hpp>
#include <Engine/Scene/GeometrySystem.hpp>

#include <QTimer>

int main(int argc, char* argv[])
{
    //! [Creating the application]
    Ra::Gui::BaseApplication app(argc, argv);
    app.initialize(Ra::Gui::SimpleWindowFactory {});
    //! [Creating the application]

    //! [Creating a quad geometry with texture coordinates]
    auto quad = Ra::Core::Geometry::makeZNormalQuad({ 1_ra, 1_ra }, {}, true);
    //! [Creating a quad geometry with texture coordinates]

    //! [Creating a texture]
    constexpr int width = 192;
    constexpr int height = 512;
    constexpr int size = width * height;
    unsigned char data[size];
    // fill with some function
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; j++) {
            data[(i * height + j)] = (unsigned char)(255.0 * std::abs(std::sin(j * i * M_PI / 64.0) * std::cos(j * i * M_PI / 96.0)));
        }
    }
    auto& textureParameters = app.m_engine->getTextureManager()->addTexture("myTexture", width, height, data);
    // these values will be used when engine initialize texture GL representation.
    textureParameters.format = gl::GLenum::GL_RED;
    textureParameters.internalFormat = gl::GLenum::GL_R8;
    //! [Creating a texture]

    //! [Create an entity and component to draw or data]
    auto e = app.m_engine->getEntityManager()->createEntity("Textured quad");

    Ra::Core::Asset::BlinnPhongMaterialData matData("myMaterialData");
    // remove glossy highlight
    matData.m_specular = Ra::Core::Utils::Color::Black();
    matData.m_hasSpecular = true;

    matData.m_hasTexDiffuse = true;
    // this name has to be the same as texManager added texture name
    matData.m_texDiffuse = "myTexture";

    // the entity get's this new component ownership. a bit wired since hidden in ctor.
    new Ra::Engine::Scene::TriangleMeshComponent("Quad Mesh", e, std::move(quad), &matData);
    //! [Create an entity and component to draw or data]

    //! [Tell the window that something is to be displayed]
    app.m_mainWindow->prepareDisplay();
    //! [Tell the window that something is to be displayed]

    auto viewer = app.m_mainWindow->getViewer();
    viewer->makeCurrent();
    auto texture = app.m_engine->getTextureManager()->getOrLoadTexture(textureParameters);
    viewer->doneCurrent();

    // terminate the app after 4 second (approximatively). Camera can be moved using mouse moves.
    auto thread = std::thread([&app, &texture]() { // not worked
        for (int dec = 0; dec < 40; ++dec) {
            unsigned char newData[size];
            for (int i = 0; i < width; ++i) {
                for (int j = 0; j < height; j++) {
                    newData[(i * height + j)] = (unsigned char)(200.0 * std::abs(std::sin(float(dec) / 4_ra)) * std::abs(std::sin(j * i * M_PI / 64.0) * std::cos(j * i * M_PI / 96.0)));
                }
            }

            texture->updateData(newData);

            std::cout << "update image data with dec = " << dec << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        app.appNeedsToQuit();
    });
    thread.detach();

    return app.exec();
}
