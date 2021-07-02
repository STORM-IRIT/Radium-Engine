#pragma once
#include <Gui/RaGui.hpp>
#include <QDialog>

namespace Ui {
class AboutDialog;
}

namespace Ra {
namespace Gui {

class RA_GUI_API AboutDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit AboutDialog( QWidget* parent = nullptr );
    ~AboutDialog() override;

  signals:
    /// Emitted when the settings button is pressed
    void settings();

    /// Emitted when the settings help is pressed
    void help();

  private:
    Ui::AboutDialog* ui;
};

} // namespace Gui
} // namespace Ra
