#include <MainApplication/Gui/MaterialEditor.hpp>

namespace Ra
{
    namespace Gui
    {
        MaterialEditor::MaterialEditor( Core::Index roIdx, QWidget* parent )
            : QWidget( parent )
            , m_roIdx( roIdx )
        {
            changeRenderObject( roIdx );
        }

        void MaterialEditor::changeRenderObject( Core::Index roIdx )
        {
            m_roIdx = roIdx;
        }

    }
}
