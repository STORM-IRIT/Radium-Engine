#include "ui_AboutDialog.h"
#include <Gui/AboutDialog/AboutDialog.hpp>
#include <QPushButton>

namespace Ra {
namespace Gui {
AboutDialog::AboutDialog( QWidget* parent ) : QDialog( parent ), ui( new Ui::AboutDialog ) {
    ui->setupUi( this );
    ui->radiumText->setFocusPolicy( Qt::NoFocus );
    ui->radiumText->setSource( QUrl( "qrc:/about.html" ) );
    auto btn = ui->aboutbuttons->button( QDialogButtonBox::Close );
    btn->setFocus();
    btn->setAutoDefault( true );
    btn->setDefault( true );
    auto settingsBtn = new QPushButton( "Settings", ui->aboutbuttons );
    ui->aboutbuttons->addButton( settingsBtn, QDialogButtonBox::RejectRole );
    settingsBtn->setAutoDefault( false );
    settingsBtn->setDefault( false );
    connect( settingsBtn, &QPushButton::pressed, this, &AboutDialog::settings );
    btn = ui->aboutbuttons->button( QDialogButtonBox::Help );
    btn->setAutoDefault( false );
    btn->setDefault( false );
    connect( btn, &QPushButton::pressed, this, &AboutDialog::help );
}

AboutDialog::~AboutDialog() {
    delete ui;
}

} // namespace Gui
} // namespace Ra
