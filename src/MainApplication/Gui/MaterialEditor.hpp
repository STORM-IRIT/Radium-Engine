#ifndef RADIUMENGINE_MATERIALEDITOR_HPP
#define RADIUMENGINE_MATERIALEDITOR_HPP

#include <QWidget>

#include <Core/Index/Index.hpp>

#include <ui_MaterialEditor.h>

class QCloseEvent;

namespace Ra
{
    namespace Gui
    {
        class MaterialEditor : public QWidget, private Ui::MaterialEditor
        {
            Q_OBJECT

        public:
            MaterialEditor( QWidget* parent = nullptr );

            void changeRenderObject( Ra::Core::Index roIdx );

        protected:
            void closeEvent( QCloseEvent* e ) override;

        private:
            Core::Index m_roIdx;
        };
    }
}

#endif // RADIUMENGINE_MATERIALEDITOR_HPP
