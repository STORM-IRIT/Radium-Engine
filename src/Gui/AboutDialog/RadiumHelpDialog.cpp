#include <Gui/AboutDialog/RadiumHelpDialog.hpp>

#include <Gui/Utils/KeyMappingManager.hpp>

#include "ui_RadiumHelpDialog.h"

namespace Ra {
namespace Gui {
RadiumHelpDialog::RadiumHelpDialog( QWidget* parent ) :
    QDialog( parent ), ui( new Ui::RadiumHelpDialog ) {
    ui->setupUi( this );
    auto kmappingMngr = Gui::KeyMappingManager::getInstance();
    std::string keyMappingHelp;
    ui->keymapping->document()->setDefaultStyleSheet(
        "table { border-collapse: collapse; }"
        "table tr th {padding-top: .4em; padding-bottom: .2em; text-align: left; border-bottom: "
        "1px solid gray;}"
        "table tr td {padding-top: .4em; padding-bottom: .2em; padding-left:1em; border-bottom: "
        "1px solid gray;}" );
    keyMappingHelp += "<h1> UI action mapping </h1>\n";
    keyMappingHelp += kmappingMngr->getHelpText();
    keyMappingHelp += "<br/>\n";
    ui->keymapping->setText( keyMappingHelp.c_str() );
}

// default detor define here, after inclusion of ui_HelpDialog, to have unique_ptr of forward decl
// type, and prevent "invalid application of 'sizeof' to an incomplete type"
RadiumHelpDialog::~RadiumHelpDialog() = default;

} // namespace Gui
} // namespace Ra
