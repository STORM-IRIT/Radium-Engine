#ifndef RADIUMENGINE_ENTITY_PROPERTY_WIDGET_HPP_
#define RADIUMENGINE_ENTITY_PROPERTY_WIDGET_HPP_
#include <QWidget>
#include <QVBoxLayout>
#include <Engine/Entity/EditableProperty.hpp>
#include <MainApplication/Gui/VectorEditor.hpp>
namespace Ra {

namespace Gui {
class EntityPropertyWidget : public QWidget
{
    Q_OBJECT
public:
    /// The specialized tab to edit properties.
    EntityPropertyWidget(QWidget* parent = nullptr) : QWidget(parent), layout(new QVBoxLayout(this)), m_currentEdit(nullptr)
    {
    
      
    }

 public slots:
       void updateValues()
    {
        CORE_ASSERT(props.size() == widgets.size(), " ");
            for (uint p = 0; p < props.size(); ++p)
            {
                if (props[p].getType() == Engine::EditableProperty::POSITION)
                {
                    CORE_ASSERT(widgets[p], "No widget for property");
                    static_cast<VectorEditor*>(widgets[p])->setValue(props[p].asPosition());
                }
            }
    }


public:
    void setEditable(const Engine::EditableInterface* edit)
    {
        if (m_currentEdit != edit)
        {
            m_currentEdit = edit;

            for (auto w : widgets)
            {
                delete w;
            }
            widgets.clear();
            props.clear();
            if (m_currentEdit)
            {
                m_currentEdit->getProperties(props);

                for (auto p : props)
                {
                    if (p.getType() == Engine::EditableProperty::POSITION)
                    {
                        widgets.push_back(new VectorEditor(QString::fromStdString(p.getName())));
                        layout->addWidget(widgets.back());
                    }
                    else
                    {
                        widgets.push_back(nullptr);
                    };
                }
                updateValues();
            }
        }

    }

       const Engine::EditableInterface* m_currentEdit;
    QVBoxLayout* layout;

    std::vector<Engine::EditableProperty> props;
    std::vector<QWidget*> widgets;
};
}
}

#endif //RADIUMENGINE_ENTITY_PROPERTY_WIDGET_HPP_