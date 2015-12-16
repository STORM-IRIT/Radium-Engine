#ifndef RADIUMENGINE_MATERIALEDITOR_HPP
#define RADIUMENGINE_MATERIALEDITOR_HPP

#include <QWidget>

#include <Core/Index/Index.hpp>

#include <ui_MaterialEditor.h>

namespace Ra
{
    namespace Gui
    {
        class MaterialEditor : public QWidget, private Ui::MaterialEditor
        {
            Q_OBJECT

        public:
            MaterialEditor( Ra::Core::Index roIdx, QWidget* parent = nullptr );

        public slots:
            void changeRenderObject( Ra::Core::Index roIdx );

        private:
            Core::Index m_roIdx;
        };
    }
}

#endif // RADIUMENGINE_MATERIALEDITOR_HPP
