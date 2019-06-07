#ifndef RADIUMENGINE_MATERIALEDITOR_HPP
#define RADIUMENGINE_MATERIALEDITOR_HPP

#include <QWidget>

#include <memory>

#include <Core/Utils/Index.hpp>
//#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <ui_MaterialEditor.h>

class QCloseEvent;
class QShowEvent;

namespace Ra {
namespace Engine {
class RadiumEngine;
class RenderObjectManager;
class RenderObject;
class Material;
class BlinnPhongMaterial;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Gui {
class MaterialEditor : public QWidget, private Ui::MaterialEditor
{
    Q_OBJECT

  public:
    MaterialEditor( QWidget* parent = nullptr );

    void changeRenderObject( Ra::Core::Utils::Index roIdx );

  private slots:
    void updateBlinnPhongViz();

    void onKdColorChanged( int );
    void onKsColorChanged( int );

    void onExpChanged( double );

    void newKdColor( const QColor& color );
    void newKsColor( const QColor& color );

    void on_m_closeButton_clicked();
    void on_kUsePerVertex_clicked( bool checked );

  protected:
    virtual void showEvent( QShowEvent* e ) override;
    virtual void closeEvent( QCloseEvent* e ) override;

  private:
    bool m_visible;

    Core::Utils::Index m_roIdx;
    std::shared_ptr<Engine::RenderObject> m_renderObject;

    /// TODO generalize material editor to others materials
    bool m_usable;
    Ra::Engine::BlinnPhongMaterial* m_blinnphongmaterial;

  private:
    enum {
        OUTPUT_FINAL    = 0,
        OUTPUT_DIFFUSE  = 1,
        OUTPUT_SPECULAR = 2,
        OUTPUT_NORMAL   = 3,
    };
};
} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_MATERIALEDITOR_HPP
