#pragma once
#include <Gui/RaGui.hpp>
#include <QDialog>

namespace Ui {
class RadiumHelpDialog;
}

namespace Ra {
namespace Gui {

class RA_GUI_API RadiumHelpDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit RadiumHelpDialog( QWidget* parent = nullptr );
    ~RadiumHelpDialog() override;

  private:
    std::unique_ptr<Ui::RadiumHelpDialog> ui;
};

} // namespace Gui
} // namespace Ra
