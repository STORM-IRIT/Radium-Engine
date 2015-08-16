#ifndef RADIUMENGINE_VECTOR_EDITOR_HPP_
#define RADIUMENGINE_VECTOR_EDITOR_HPP_

#include <QWidget>
#include <ui_vectoreditor.h>
namespace Ra {
namespace Gui{
class VectorEditor : public QWidget, private Ui::VectorEditor
{
public:
    VectorEditor(QString title, QWidget* parent = nullptr) : QWidget(parent)
    {
        setupUi(this);
        m_groupBox->setTitle(title);
    }

    void setValue(const Core::Vector3& vec)
    {
        m_x->setValue(vec.x());
        m_y->setValue(vec.y());
        m_z->setValue(vec.z());
    }
};
}
}
#endif// RADIUMENGINE_VECTOR_EDITOR_HPP_
