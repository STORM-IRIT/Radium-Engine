#include "ui_HelpDialog.h"
#include <GuiBase/Timeline/HelpDialog.hpp>

namespace Ra::GuiBase {

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

} // namespace Ra::GuiBase
