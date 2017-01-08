#include <Gui/TransformEditorWidget.hpp>
#include <QVBoxLayout>
#include <Engine/RadiumEngine.hpp>

namespace Ra
{
    namespace Gui
    {

        TransformEditorWidget::TransformEditorWidget(QWidget* parent)
                : QWidget(parent), m_layout(new QVBoxLayout(this)), m_translationEditor(nullptr) { }

        void TransformEditorWidget::updateValues()
        {
            if( canEdit() )
            {
                getTransform();
                CORE_ASSERT( m_translationEditor, "No edtitor widget !");
                m_translationEditor->blockSignals( true );
                m_translationEditor->setValue(m_transform.translation());
                m_translationEditor->blockSignals( false );
            }
        }

        void TransformEditorWidget::onChangedPosition(const Core::Vector3& v, uint id)
        {
            CORE_ASSERT(m_currentEdit.isValid(), "Nothing to edit");
            m_transform.translation() = v;
            setTransform(m_transform);
        }

        void TransformEditorWidget::setEditable( const Engine::ItemEntry& ent )
        {
            delete m_translationEditor;
            TransformEditor::setEditable( ent );
            if ( canEdit() )
            {
                m_translationEditor = new VectorEditor(0, QString::fromStdString(getEntryName( Engine::RadiumEngine::getInstance(), m_currentEdit)),  true );
                connect(m_translationEditor, &VectorEditor::valueChanged, this, &TransformEditorWidget::onChangedPosition);
            }
        }
    }
}
