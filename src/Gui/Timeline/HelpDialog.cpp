#include "ui_HelpDialog.h"
#include <Gui/Timeline/HelpDialog.hpp>

namespace Ra::Gui {

HelpDialog::HelpDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::HelpDialog ) {
    ui->setupUi( this );
}

HelpDialog::~HelpDialog() {
    delete ui;
}

void HelpDialog::reject() {
    emit closed();
    QDialog::reject();
}

} // namespace Ra::Gui
