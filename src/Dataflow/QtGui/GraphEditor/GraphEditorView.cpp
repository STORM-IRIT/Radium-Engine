#include <Dataflow/QtGui/GraphEditor/GraphEditorView.hpp>

#include <QVBoxLayout>

#include <QtNodes/ConnectionStyle>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModelRegistry>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/QtGui/GraphEditor/WidgetFactory.hpp>
#include <utility>

namespace Ra {
namespace Dataflow {
namespace QtGui {
namespace GraphEditor {
GraphEditorView::GraphEditorView( QWidget* parent ) : QWidget( parent, Qt::Window ) {
    QVBoxLayout* l = new QVBoxLayout( this );

    m_graph = std::make_shared<SimpleGraphModel>();
    scene   = new QtNodes::BasicGraphicsScene( *m_graph );
    view    = new QtNodes::GraphicsView( scene );

    l->addWidget( view );
    l->setContentsMargins( 0, 0, 0, 0 );
    l->setSpacing( 0 );
}

GraphEditorView::~GraphEditorView() {}

// Find the way to see all the scene in the editor (or, at leas 75% of the scene)
void GraphEditorView::resizeEvent( QResizeEvent* ) {
    // view->resetTransform();
    view->ensureVisible( view->sceneRect() );
    view->centerOn( view->sceneRect().center() );
}

} // namespace GraphEditor
} // namespace QtGui
} // namespace Dataflow
} // namespace Ra
