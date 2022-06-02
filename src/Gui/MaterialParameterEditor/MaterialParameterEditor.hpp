#pragma once
#include <Gui/RaGui.hpp>

#include <QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <nlohmann/json.hpp>

namespace Ra {
namespace Engine {
namespace Data {
class Material;
class RenderParameters;
} // namespace Data
} // namespace Engine

namespace Gui {
namespace Widgets {
class ControlPanel;
}
/**
 * Simple Widget for Material edition
 * A material editor will show information about the selected material and expose a control panel
 * containing all of its editable parameters to allow the user to edit them.
 */
class RA_GUI_API MaterialParameterEditor : public QWidget
{
    Q_OBJECT
  public:
    /** Constructors and destructor
     */
    /** @{ */
    explicit MaterialParameterEditor( QWidget* parent = nullptr );
    MaterialParameterEditor( const MaterialParameterEditor& ) = delete;
    MaterialParameterEditor& operator=( const MaterialParameterEditor& ) = delete;
    MaterialParameterEditor( MaterialParameterEditor&& )                 = delete;
    MaterialParameterEditor&& operator=( MaterialParameterEditor&& ) = delete;
    ~MaterialParameterEditor() override                              = default;
    /** @} */

    /**
     * Update the different UI element with the given material
     * Display the properties of the material and populate the widget with widgets to edit its
     * editable parameters.
     * @param material The pointer to the material to edit
     */
    void setupFromMaterial( std::shared_ptr<Ra::Engine::Data::Material> material );

    /**
     * Wether to show parameters without associated metadata
     *
     * @param enable
     */
    void showUnspecified( bool enable );

  signals:
    /**
     * Signal emitted whenever a parameter is modified
     */
    void materialParametersModified();

  private:
    template <typename T>
    void addEnumParameterWidget( const std::string& name,
                                 T initial,
                                 Ra::Engine::Data::RenderParameters& params,
                                 const nlohmann::json& paramMetadata );
    template <typename T>
    void addNumberParameterWidget( const std::string& name,
                                   T initial,
                                   Ra::Engine::Data::RenderParameters& params,
                                   const nlohmann::json& metadata );
    template <typename T>
    void addVectorParameterWidget( const std::string& key,
                                   const std::vector<T>& initial,
                                   Ra::Engine::Data::RenderParameters& params,
                                   const nlohmann::json& metadata );
    template <typename T>
    void addMatrixParameterWidget( const std::string& key,
                                   const T& initial,
                                   Ra::Engine::Data::RenderParameters& params,
                                   const nlohmann::json& metadata );
    /// The label containing the instance name of the material
    QLabel* m_matInstanceNameLabel;
    /// The label containing the name of the material
    QLabel* m_matNameLabel;
    /// The text box containing the properties of the material
    QPlainTextEdit* m_matProperties;
    /// The layout containing the control panel
    QVBoxLayout* m_matParamsLayout;
    /// The control panel re-generated whenever setupFromMaterial is called
    Widgets::ControlPanel* m_parametersControlPanel;
    /// wether to show the unspecified materials
    bool m_showUnspecified = false;
};

} // namespace Gui
} // namespace Ra
