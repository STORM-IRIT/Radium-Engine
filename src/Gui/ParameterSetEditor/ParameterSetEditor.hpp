#pragma once
#include <Gui/RaGui.hpp>

#include <QGroupBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <nlohmann/json.hpp>

#include <Core/Containers/VariableSetEnumManagement.hpp>
#include <Gui/Widgets/ControlPanel.hpp>

namespace Ra {

namespace Core {
class VariableSet;
class DynamicVisitor;
} // namespace Core
namespace Engine {
namespace Data {
class Material;
class RenderParameters;
} // namespace Data
} // namespace Engine

namespace Gui {

/**
 * \brief Simple Widget for RenderParameter editing
 * The editor will expose a control panel
 * containing all of the editable parameters from a RenderParameter set.
 */
class RA_GUI_API VariableSetEditor : public Widgets::ControlPanel
{
    Q_OBJECT
  public:
    explicit VariableSetEditor( const std::string& name, QWidget* parent = nullptr );
    VariableSetEditor( const VariableSetEditor& )            = delete;
    VariableSetEditor& operator=( const VariableSetEditor& ) = delete;
    VariableSetEditor( VariableSetEditor&& )                 = delete;
    VariableSetEditor&& operator=( VariableSetEditor&& )     = delete;
    ~VariableSetEditor()                                     = default;

    /** \brief Update the different UI element with the given renderParameter, using the given
     * constraints.
     *
     * \param params the VariableSet to edit
     * \param constraints the parameter constraints descriptor
     */
    void setupUi( Core::VariableSet& params, const nlohmann::json& constraints );

    /** \brief Wether to show parameters without associated metadata
     *
     * \param enable
     */
    void setShowUnspecified( bool enable ) { m_showUnspecified = enable; }
    bool showUnspecified() { return m_showUnspecified; }

    /** \brief Add a combobox allowing to chose the value of an enumerator.
     *
     * \note Only un-scoped enum (i.e. implicitly convertible from and to integral type), with
     * enumerators without initializer.
     * \tparam T
     * \param name
     * \param initial
     * \param params
     * \param paramMetadata
     */
    template <typename T>
    void addEnumWidget( const std::string& name,
                        T& initial,
                        Core::VariableSet& params,
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
    void addNumberWidget( const std::string& name,
                          T& initial,
                          Core::VariableSet& params,
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
    void addVectorWidget( const std::string& key,
                          std::vector<T>& initial,
                          Core::VariableSet& params,
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
    void addMatrixWidget( const std::string& key,
                          T& initial,
                          Core::VariableSet& params,
                          const nlohmann::json& metadata );

  signals:
    /**
     * Signal emitted whenever a parameter is modified
     */
    void parameterModified( const std::string& name );

  private:
    /// wether to show the unspecified materials
    bool m_showUnspecified = false;
};

template <typename T>
void VariableSetEditor::addEnumWidget( const std::string& key,
                                       T& initial,
                                       Core::VariableSet& params,
                                       const nlohmann::json& metadata ) {
    using namespace Ra::Core::VariableSetEnumManagement;
    auto m = metadata[key];

    std::string description = m.contains( "description" ) ? m["description"] : "";
    std::string nm          = m.contains( "name" ) ? std::string { m["name"] } : key;

    if ( auto ec = getEnumConverter<T>( params, key ) ) {
        auto items                        = ( *ec )->getEnumerators();
        auto onEnumParameterStringChanged = [this, &params, &key]( const QString& value ) {
            //            params.setVariable( key, value.toStdString() );
            setEnumVariable( params, key, value.toStdString() );
            emit parameterModified( key );
        };
        addComboBox( nm,
                     onEnumParameterStringChanged,
                     getEnumString( params, key, initial ),
                     items,
                     description );
    }
    else {
        LOG( Core::Utils::logWARNING )
            << "ParameterSet don't have converter for enum " << key << " use index<>int instead.";

        auto items = std::vector<std::string>();
        items.reserve( m["values"].size() );
        for ( const auto& value : m["values"] ) {
            items.push_back( value );
        }

        auto onEnumParameterIntChanged = [this, &params, &key]( T value ) {
            params.setVariable( key, value );
            emit parameterModified( key );
        };
        addComboBox( nm, onEnumParameterIntChanged, initial, items, description );
    }
}

template <typename T>
void VariableSetEditor::addNumberWidget( const std::string& key,
                                         T& initial,
                                         Core::VariableSet& /*params*/,
                                         const nlohmann::json& metadata ) {
    auto onNumberParameterChanged = [this, &initial, &key]( T value ) {
        // no need to edit params since initial is a ref
        initial = value;
        emit parameterModified( key );
    };

    if ( metadata.contains( key ) ) {
        auto m   = metadata[key];
        auto min = std::numeric_limits<T>::lowest();
        auto max = std::numeric_limits<T>::max();

        std::string description = m.contains( "description" ) ? m["description"] : "";
        std::string nm          = m.contains( "name" ) ? std::string { m["name"] } : key;

        if ( m.contains( "oneOf" ) ) {
            // the variable has multiple valid bounds
            std::vector<std::pair<T, T>> bounds;
            bounds.reserve( m["oneOf"].size() );
            for ( const auto& bound : m["oneOf"] ) {
                auto mini = bound.contains( "minimum" ) ? T( bound["minimum"] ) : min;
                auto maxi = bound.contains( "maximum" ) ? T( bound["maximum"] ) : max;
                bounds.emplace_back( mini, maxi );
            }
            auto predicate = [bounds]( T value ) {
                bool valid = false;
                auto it    = bounds.begin();
                while ( !valid && it != bounds.end() ) {
                    valid = value >= ( *it ).first && value <= ( *it ).second;
                    ++it;
                }
                return valid;
            };

            addConstrainedNumberInput<T>(
                nm, onNumberParameterChanged, initial, predicate, description );
        }
        else if ( m.contains( "minimum" ) && m.contains( "maximum" ) ) {
            min = T( m["minimum"] );
            max = T( m["maximum"] );
            if constexpr ( std::is_floating_point_v<T> ) {
                addPowerSliderInput( nm, onNumberParameterChanged, initial, min, max, description );
            }
            else { addSliderInput( nm, onNumberParameterChanged, initial, min, max, description ); }
        }
        else {
            min = m.contains( "minimum" ) ? T( m["minimum"] ) : min;
            max = m.contains( "maximum" ) ? T( m["maximum"] ) : max;
            addNumberInput<T>( nm, onNumberParameterChanged, initial, min, max, description );
        }
    }
    else if ( m_showUnspecified ) { addNumberInput<T>( key, onNumberParameterChanged, initial ); }
}

template <typename T>
void VariableSetEditor::addVectorWidget( const std::string& key,
                                         std::vector<T>& initial,
                                         Core::VariableSet& /*params*/,
                                         const nlohmann::json& metadata ) {
    auto onVectorParameterChanged = [this, &initial, &key]( const std::vector<T>& value ) {
        initial = value;
        emit parameterModified( key );
    };

    if ( metadata.contains( key ) ) {
        auto m                  = metadata[key];
        std::string description = m.contains( "description" ) ? m["description"] : "";
        addVectorInput<T>( m["name"], onVectorParameterChanged, initial, description );
    }
    else if ( m_showUnspecified ) { addVectorInput<T>( key, onVectorParameterChanged, initial ); }
}

template <typename T>
void VariableSetEditor::addMatrixWidget( const std::string& key,
                                         T& initial,
                                         Core::VariableSet& /*params*/,
                                         const nlohmann::json& metadata ) {
    auto onMatrixParameterChanged = [this, &initial, &key]( const Ra::Core::MatrixN& value ) {
        initial = T( value );
        emit parameterModified( key );
    };

    if ( metadata.contains( key ) ) {
        auto m                  = metadata[key];
        std::string description = m.contains( "description" ) ? m["description"] : "";
        addMatrixInput( m["name"], onMatrixParameterChanged, initial, 3, description );
    }
    else if ( m_showUnspecified ) { addMatrixInput( key, onMatrixParameterChanged, initial ); }
}

} // namespace Gui
} // namespace Ra
