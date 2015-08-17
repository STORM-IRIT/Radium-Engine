#ifndef RADIUMENGINE_VECTOR_EDITOR_HPP_
#define RADIUMENGINE_VECTOR_EDITOR_HPP_

#include <QWidget>
#include <ui_vectoreditor.h>
namespace Ra {
namespace Gui{
class VectorEditor : public QWidget, private Ui::VectorEditor
{
    Q_OBJECT
public:
    VectorEditor(uint id, QString title, QWidget* parent = nullptr) : QWidget(parent), m_id(id)
    {
        setupUi(this);
        m_groupBox->setTitle(title);
        connect(m_x, SIGNAL(valueChanged(double)), this, SLOT(onValueChangedInternal(double)));
        connect(m_y, SIGNAL(valueChanged(double)), this, SLOT(onValueChangedInternal(double)));
        connect(m_z, SIGNAL(valueChanged(double)), this, SLOT(onValueChangedInternal(double)));
    }

    void setValue(const Core::Vector3& vec)
    {
        m_x->setValue(vec.x());
        m_y->setValue(vec.y());
        m_z->setValue(vec.z());
    }

signals:
        void valueChanged(uint id, const Core::Vector3& newValue);

private slots:
        void onValueChangedInternal( double d )
        {
            Core::Vector3 v (m_x->value(), m_y->value(), m_z->value());
            emit(valueChanged(m_id,v));
        };
private:
    uint m_id;
};
}
}
#endif// RADIUMENGINE_VECTOR_EDITOR_HPP_
