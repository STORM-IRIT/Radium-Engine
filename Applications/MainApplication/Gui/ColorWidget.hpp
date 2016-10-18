#ifndef RADIUMENGINE_COLORWIDGET_HPP
#define RADIUMENGINE_COLORWIDGET_HPP

#include <QWidget>

namespace Ra
{
    namespace Gui
    {

        class ColorWidget : public QWidget
        {
            Q_OBJECT

        public:
            ColorWidget( QWidget* parent = nullptr );

        public slots:
            void colorChanged( int r, int g, int b );

        signals:
            void newColorPicked( const QColor& color );

        private:
            void colorChanged();
            virtual void mousePressEvent( QMouseEvent* e ) override;

        private:
            QColor m_currentColor;
        };

    }
}

#endif
