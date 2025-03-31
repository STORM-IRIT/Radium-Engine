
#include <Gui/Timeline/HelpDialog.hpp>
#include <QDialog>
#include <QObject>

#include "ui_HelpDialog.h"

class QWidget;

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
