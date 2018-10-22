#ifndef MESHPAINTUI_H
#define MESHPAINTUI_H

#include <QColor>
#include <QFrame>

namespace UI {
class PostSubdivUI;
}

namespace PostSubdivPlugin {
class PostSubdivPluginC;
}

namespace Ui {
class PostSubdivUI;
}

/// Widget for the PostSubdiv Plugin.
class PostSubdivUI : public QFrame {
    Q_OBJECT

    friend class PostSubdivPlugin::PostSubdivPluginC;

  public:
    explicit PostSubdivUI( QWidget* parent = 0 );
    ~PostSubdivUI();

  signals:
    /// Emitted when the user changes the subdivision method.
    void subdivMethodChanged( uint method );

    /// Emitted when the user changes the number of subdivision iterations.
    void subdivIterChanged( uint iter );

  private slots:
    /// Triggered when the user changes the subdivision method.
    void on_m_subdivMethod_currentIndexChanged( int index );

    /// Triggered when the user changes the number of subdivision iterations.
    void on_m_subdivIter_valueChanged( int arg1 );

  private:
    /// The Widget UI.
    Ui::PostSubdivUI* ui;
};

#endif // MESHPAINTUI_H
