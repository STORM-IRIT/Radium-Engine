#include <MainApplication/Gui/MaterialEditor.hpp>

#include <QCloseEvent>

namespace Ra
{
    namespace Gui
    {
        MaterialEditor::MaterialEditor( QWidget* parent )
            : QWidget( parent )
            , m_roIdx( -1 )
        {
        }

        void MaterialEditor::changeRenderObject( Core::Index roIdx )
        {
            m_roIdx = roIdx;
        }

        void MaterialEditor::closeEvent( QCloseEvent* e )
        {
            hide();
        }
    }
}
