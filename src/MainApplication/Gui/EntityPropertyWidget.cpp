#include <MainApplication/Gui/EntityPropertyWidget.hpp>
#include <QVBoxLayout>
#include <MainApplication/Gui/VectorEditor.hpp>
#include <MainApplication/Gui/RotationEditor.hpp>

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
            switch(m_props[p].getType())
            {
                case Engine::EditableProperty::POSITION:
                {
                    CORE_ASSERT(m_widgets[p], "No widget for property");
                    static_cast<VectorEditor*>(m_widgets[p])->setValue(m_props[p].asPosition());
                    break;
                }
                case Engine::EditableProperty::ROTATION:
                {
                    CORE_ASSERT(m_widgets[p], "No widget for property");
                    static_cast<RotationEditor*>(m_widgets[p])->setValue(m_props[p].asRotation());
                    break;
                }
                default:
                    ;// do nothing.
            }
        }
    }

    EntityPropertyWidget::~EntityPropertyWidget()
    {
        // This will delete the widgets if there were any.
        setEditable(nullptr);
    }

    void EntityPropertyWidget::onChangedPosition(uint id, const Core::Vector3& v)
    {
        CORE_ASSERT(m_currentEdit, "Nothing to edit");
        CORE_ASSERT(sender() == m_widgets[id], "Sender error");
        m_props[id].asPosition() = v;
        m_currentEdit->setProperty(m_props[id]);
    }

    void EntityPropertyWidget::onChangedRotation(uint id, const Core::Quaternion& q)
    {
        CORE_ASSERT(m_currentEdit, "Nothing to edit");
        CORE_ASSERT(sender() == m_widgets[id], "Sender error");
        m_props[id].asRotation() = q;
        m_currentEdit->setProperty(m_props[id]);
    }

    void EntityPropertyWidget::setEditable(Engine::EditableInterface* edit)
    {
        if (m_currentEdit != edit)
        {
            m_currentEdit = edit;

            // Delete widgets from the previous editable.
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
                    switch (p.getType())
                    {
                        case Engine::EditableProperty::POSITION:
                        {
                            VectorEditor* widget = new VectorEditor(id, QString::fromStdString(p.getName()));
                            m_layout->addWidget(widget);
                            widget->setValue(p.asPosition());
                            connect(widget, SIGNAL (valueChanged(uint, const Core::Vector3&)),
                                    this, SLOT(onChangedPosition(uint, const Core::Vector3&)));
                            m_widgets.push_back(widget);
                            break;
                        }
                        case Engine::EditableProperty::ROTATION:
                        {
                            RotationEditor* widget = new RotationEditor(id, QString::fromStdString(p.getName()));
                            m_layout->addWidget(widget);
                            widget->setValue(p.asRotation());
                            connect(widget, SIGNAL (valueChanged(uint, const Core::Quaternion&)),
                                    this, SLOT(onChangedRotation(uint, const Core::Quaternion&)));
                            m_widgets.push_back(widget);
                            break;
                        }
                        default:
                            m_widgets.push_back(nullptr);
                    }
                    ++id;
                }
                CORE_ASSERT(m_widgets.size() == m_props.size(), "Widget/property inconsistency");
            }
        }

    }
}}