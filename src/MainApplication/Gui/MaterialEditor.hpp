#ifndef RADIUMENGINE_MATERIALEDITOR_HPP
#define RADIUMENGINE_MATERIALEDITOR_HPP

#include <QWidget>

#include <memory>

#include <Core/Index/Index.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <ui_MaterialEditor.h>

class QCloseEvent;
class QShowEvent;

namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
        class RenderObjectManager;
        class Material;
    }
}

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

        private slots:
            void updateMaterialViz( const Engine::Material* material );

            void onKdColorChanged( int );
            void onKsColorChanged( int );

            void onExpChanged( double );

            void newKdColor( const QColor& color );
            void newKsColor( const QColor& color );

        protected:
            virtual void showEvent( QShowEvent* e ) override;
            virtual void closeEvent( QCloseEvent* e ) override;

        private:
            bool m_visible;

            Engine::RadiumEngine* m_engine;
            Engine::RenderObjectManager* m_roMgr;

            Core::Index m_roIdx;
            std::shared_ptr<Engine::RenderObject> m_renderObject;

        private:
            enum
            {
                OUTPUT_FINAL = 0,
                OUTPUT_DIFFUSE = 1,
                OUTPUT_SPECULAR = 2,
                OUTPUT_NORMAL = 3,
            };
        };
    }
}

#endif // RADIUMENGINE_MATERIALEDITOR_HPP
