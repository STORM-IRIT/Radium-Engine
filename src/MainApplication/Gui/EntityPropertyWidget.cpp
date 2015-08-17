#include <MainApplication/Gui/EntityPropertyWidget.hpp>
#include <QVBoxLayout>
#include <MainApplication/Gui/VectorEditor.hpp>

namespace Ra {
namespace Gui{

    EntityPropertyWidget::EntityPropertyWidget(QWidget* parent)
      : QWidget(parent), m_currentEdit(nullptr), m_layout(new QVBoxLayout(this))
    { }

    void EntityPropertyWidget::updateValues()
    {
        CORE_ASSERT(m_props.size() == m_widgets.size(), " ");
        for (uint p = 0; p < m_props.size(); ++p)
        {
            if (m_props[p].getType() == Engine::EditableProperty::POSITION)
            {
                CORE_ASSERT(m_widgets[p], "No widget for property");
                static_cast<VectorEditor*>(m_widgets[p])->setValue(m_props[p].asPosition());
            }
        }
    }

    void EntityPropertyWidget::onValueChanged(uint id, const Core::Vector3& v)
    {
        CORE_ASSERT(m_currentEdit, "Nothing to edit");
        CORE_ASSERT(sender() == m_widgets[id], "Sender error");
        m_props[id].asPosition() = v;
        m_currentEdit->setProperty(m_props[id]);
    }

    void EntityPropertyWidget::setEditable(Engine::EditableInterface* edit)
    {
        if (m_currentEdit != edit)
        {
            m_currentEdit = edit;

            for (auto w : m_widgets)
            {
                delete w;
            }
            m_widgets.clear();
            m_props.clear();
            if (m_currentEdit)
            {
                m_currentEdit->getProperties(m_props);
                uint id = 0;
                for (auto p : m_props)
                {
                    if (p.getType() == Engine::EditableProperty::POSITION)
                    {
                        VectorEditor* widget = new VectorEditor(id,QString::fromStdString(p.getName()));
                        m_layout->addWidget(widget);
                        widget->setValue(p.asPosition());
                        connect(widget, SIGNAL (valueChanged(uint, const Core::Vector3&)),
                                 this, SLOT(onValueChanged(uint, const Core::Vector3&)));
                        m_widgets.push_back(widget);
                    }
                    else
                    {
                        m_widgets.push_back(nullptr);
                    };
                    ++id;
                }
            }
        }

    }
}}