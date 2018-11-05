#ifndef SKINNINGLUGIN_HPP_
#define SKINNINGLUGIN_HPP_

#include <SkinningPluginMacros.hpp>

#include <PluginBase/RadiumPluginInterface.hpp>
#include <QAction>
#include <QComboBox>
#include <QFrame>
#include <QObject>
#include <QtPlugin>

#include <QDoubleSpinBox>
#include <QLabel>
#include <QStackedWidget>
#include <QTextEdit>

namespace Ra {
namespace Engine {
class RadiumEngine;
struct ItemEntry;
} // namespace Engine
namespace Guibase {
class SelectionManager;
}
} // namespace Ra

namespace SkinningPlugin {

class SkinningComponent;
class SkinningSystem;

class SkinningWidget : public QFrame {
    Q_OBJECT

    friend class SkinningPluginC;

  public:
    explicit SkinningWidget( QWidget* parent = nullptr );

  public slots:
    void setCurrent( const Ra::Engine::ItemEntry& entry, SkinningComponent* comp );

  private slots:
    void onSkinningChanged( int newType );

    void onLSBActionTriggered();
    void onDQActionTriggered();
    void onCoRActionTriggered();
    void onPBSActionTriggered();

    void onStretchStiffnessSelectValueChanged( double value );
    void onCompressionSelectValueChanged( double value );
    void onNegVolumeSelectValueChanged( double value );
    void onPosVolumeStiffnessSelectValueChanged( double value );

    void chooseVolumetricMeshPath();

  private:
    SkinningComponent* m_current;

    QStackedWidget* m_stackedWidget;
    QTextEdit* m_pathVolumetricMesh;
    QComboBox* m_skinningSelect;
    QLabel* m_pathVolumetricMeshValidIndication;

    QDoubleSpinBox* m_stretchStiffnessSelect;
    QDoubleSpinBox* m_compressionStiffnessSelect;
    QDoubleSpinBox* m_negVolumeStiffnessSelect;
    QDoubleSpinBox* m_posVolumeStiffnessSelect;

    QAction* m_actionLBS;
    QAction* m_actionDQ;
    QAction* m_actionCoR;
    QAction* m_actionPBS;

    // Functions
    void setSkinningWidgetLayout();
    QWidget* createPBSInterface();
    void createPBStiffnesses();
};

// Du to an ambiguous name while compiling with Clang, must differentiate plugin claas from plugin
// namespace
class SkinningPluginC : public QObject, Ra::Plugins::RadiumPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

  public:
    virtual ~SkinningPluginC();

    virtual void registerPlugin( const Ra::PluginContext& context ) override;

    virtual bool doAddWidget( QString& name ) override;
    virtual QWidget* getWidget() override;

    virtual bool doAddMenu() override;
    virtual QMenu* getMenu() override;

    virtual bool doAddAction( int& nb ) override;
    virtual QAction* getAction( int id ) override;

  private slots:
    void onCurrentChanged( const QModelIndex& current, const QModelIndex& prev );

  private:
    SkinningSystem* m_system;
    Ra::GuiBase::SelectionManager* m_selectionManager;
    SkinningWidget* m_widget;
};

} // namespace SkinningPlugin

#endif // SKINNINGPLUGIN_HPP_
