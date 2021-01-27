#include "ui_KeyFrameEditor.h"
#include <GuiBase/KeyFrameEditor/KeyFrameEditor.hpp>
#include <GuiBase/KeyFrameEditor/KeyFrameEditorFrame.hpp>

#include <Core/Animation/KeyFramedValue.hpp>

#include <fstream>
#include <iostream>

#include <QFileDialog>
#include <QResizeEvent>

namespace Ra::GuiBase {

KeyFrameEditor::KeyFrameEditor( Scalar maxTime, QWidget* parent ) :
    QDialog( parent ),
    ui( new Ui::KeyFrameEditor ) {
    ui->setupUi( this );

    // set sizePolicy to allow zoom in scrollArea
    ui->scrollAreaWidgetContents->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
    // first draw of ruler with current width (default in Timeline.ui) of dialog
    ui->scrollArea->onDrawRuler( width() - 2,
                                 height() - 2 ); // left/right border width = 2 *1 pixel

    // --- SET INTERNAL REFERENCES ---
    ui->m_editorFrame->setEditorUi( ui );
    ui->m_timeScale->setScrollArea( ui->scrollArea );
    ui->m_valueScale->setScrollArea( ui->scrollArea );
    ui->m_frameScale->setScrollArea( ui->scrollArea );
    ui->m_frameScale->setEditorFrame( ui->m_editorFrame );

    // set duration
    ui->m_editorFrame->setDuration( maxTime );

    // --- CREATE INTERNAL CONNECTIONS ---
    connect( ui->m_editorFrame, &KeyFrameEditorFrame::updated, [=]() { update(); } );

    // --- CONNECT INTERNAL SIGNALS TO EXTERNAL SIGNALS ---
    connect( ui->m_editorFrame,
             &KeyFrameEditorFrame::cursorChanged,
             this,
             &KeyFrameEditor::cursorChanged );
    connect( ui->m_editorFrame,
             &KeyFrameEditorFrame::keyFrameChanged,
             this,
             &KeyFrameEditor::keyFrameChanged );
    connect( ui->m_editorFrame,
             &KeyFrameEditorFrame::keyFrameAdded,
             this,
             &KeyFrameEditor::keyFrameAdded );
    connect( ui->m_editorFrame,
             &KeyFrameEditorFrame::keyFrameDeleted,
             this,
             &KeyFrameEditor::keyFrameDeleted );
    connect( ui->m_editorFrame,
             &KeyFrameEditorFrame::keyFrameMoved,
             this,
             &KeyFrameEditor::keyFrameMoved );
    connect( ui->m_editorFrame,
             &KeyFrameEditorFrame::keyFramesMoved,
             this,
             &KeyFrameEditor::keyFramesMoved );
}

KeyFrameEditor::~KeyFrameEditor() {
    delete ui;
}

void KeyFrameEditor::onChangeCursor( Scalar time ) {
    ui->m_editorFrame->onChangeCursor( time, false );
    update();
}

void KeyFrameEditor::onChangeDuration( Scalar duration ) {
    ui->m_editorFrame->setDuration( duration );
    update();
}

void KeyFrameEditor::onUpdateKeyFrames( Scalar currentTime ) {
    ui->m_editorFrame->onUpdateKeyFrames( currentTime );
}

void KeyFrameEditor::resizeEvent( QResizeEvent* event ) {

    ui->scrollArea->onDrawRuler( event->size().width() - 2, event->size().height() - 2 );
}

void KeyFrameEditor::setKeyFramedValue( const std::string& name, KeyFrame* frame ) {
    ui->m_valueName->setText( QString::fromStdString( name ) );
    ui->m_editorFrame->setKeyFramedValue( frame );
}

} // namespace Ra::GuiBase
