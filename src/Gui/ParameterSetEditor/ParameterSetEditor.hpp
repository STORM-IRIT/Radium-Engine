#pragma once
#include <Gui/RaGui.hpp>

#include <QGroupBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <nlohmann/json.hpp>

#include <Gui/Widgets/ControlPanel.hpp>

namespace Ra {
namespace Engine {
namespace Data {
class Material;
class RenderParameters;
} // namespace Data
} // namespace Engine

namespace Gui {
/**
 * \brief Simple Widget for RenderParameter edition
 * The editor will expose a control panel
 * containing all of the editable parameters from a RenderParameter set.
 */
class RA_GUI_API ParameterSetEditor : public Widgets::ControlPanel
{
    Q_OBJECT
  public:
    /** Constructors and destructor
     */
    /** @{ */
    explicit ParameterSetEditor( const std::string& name, QWidget* parent = nullptr );
    ParameterSetEditor( const ParameterSetEditor& ) = delete;
    ParameterSetEditor& operator=( const ParameterSetEditor& ) = delete;
    ParameterSetEditor( ParameterSetEditor&& )                 = delete;
    ParameterSetEditor&& operator=( ParameterSetEditor&& ) = delete;
    ~ParameterSetEditor() override                         = default;
    /** @} */

    /**
     * \brief Update the different UI element with the given renderParameter, using the given
     * constraints.
     * \param params the parameter set to edit
     * \param constraints the parameter
     * constraints descriptor
     * \param name (optional) the name to display in top of the editor
     */
    void setupFromParameters( Engine::Data::RenderParameters& params,
                              const nlohmann::json& constraints );

    /**
     * Wether to show parameters without associated metadata
     *
     * \param enable
     */
    void showUnspecified( bool enable );

  signals:
    /**
     * Signal emitted whenever a parameter is modified
     */
    void parameterModified( const std::string& name );

  private:
    /**
     * \brief Add a combobox allowing to chose the value of an enumerator.
     * \note Only un-scoped enum (i.e. implicitly convertible from and to integral type), with
     * enumerators without initializer.
     * \tparam T
     * \param name
     * \param initial
     * \param params
     * \param paramMetadata
     */
    template <typename T>
    void addEnumParameterWidget( const std::string& name,
                                 T initial,
                                 Ra::Engine::Data::RenderParameters& params,
                                 const nlohmann::json& paramMetadata );
    /**
     * \brief
     * \tparam T
     * \param name
     * \param initial
     * \param params
     * \param metadata
     */
    template <typename T>
    void addNumberParameterWidget( const std::string& name,
                                   T initial,
                                   Ra::Engine::Data::RenderParameters& params,
                                   const nlohmann::json& metadata );

    /**
     * \brief
     * \tparam T
     * \param key
     * \param initial
     * \param params
     * \param metadata
     */
    template <typename T>
    void addVectorParameterWidget( const std::string& key,
                                   const std::vector<T>& initial,
                                   Ra::Engine::Data::RenderParameters& params,
                                   const nlohmann::json& metadata );

    /**
     * \brief
     * \tparam T
     * \param key
     * \param initial
     * \param params
     * \param metadata
     */
    template <typename T>
    void addMatrixParameterWidget( const std::string& key,
                                   const T& initial,
                                   Ra::Engine::Data::RenderParameters& params,
                                   const nlohmann::json& metadata );
    /// wether to show the unspecified materials
    bool m_showUnspecified = false;
};

} // namespace Gui
} // namespace Ra
