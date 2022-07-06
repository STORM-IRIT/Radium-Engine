#include <Core/Geometry/Curve2D.hpp>
#include <Core/Math/Interpolation.hpp>
#include <Engine/Data/Material.hpp>
#include <Engine/Data/Mesh.hpp>
#include <Engine/Data/ViewingParameters.hpp>
#include <Gui/Viewer/CameraManipulator.hpp>

#include "CurveEditor.hpp"
#include "Painty/CubicBezierApproximation.hpp"

CurveEditor::CurveEditor(
    const Ra::Core::VectorArray<Ra::Core::Geometry::Curve2D::Vector>& polyline,
    MyViewer* viewer ) :
    Ra::Engine::Scene::Entity( "Curve Editor" ), m_viewer( viewer ) {
    m_entityMgr = Ra::Engine::RadiumEngine::getInstance()->getEntityManager();
    m_roMgr     = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager();

    // Approximate polyline with bezier
    CubicBezierApproximation approximator;
    approximator.init( polyline );
    approximator.compute();
    auto solution    = approximator.getSolution();
    auto solutionPts = solution.getCtrlPoints();

    // Create and render solution entities
    CurveFactory factory;
    std::vector<Ra::Core::Vector3Array> allCtrlPts;
    m_viewer->makeCurrent();
    for ( unsigned int i = 0; i < solutionPts.size() - 3; i += 3 ) {
        Ra::Core::Vector3Array ctrlPts;
        ctrlPts.push_back( Ra::Core::Vector3( solutionPts[i].x(), 0, solutionPts[i].y() ) );
        ctrlPts.push_back( Ra::Core::Vector3( solutionPts[i + 1].x(), 0, solutionPts[i + 1].y() ) );
        ctrlPts.push_back( Ra::Core::Vector3( solutionPts[i + 2].x(), 0, solutionPts[i + 2].y() ) );
        ctrlPts.push_back( Ra::Core::Vector3( solutionPts[i + 3].x(), 0, solutionPts[i + 3].y() ) );
        std::string name = "Curve_" + std::to_string( i / 3 );
        auto e           = factory.createCurveComponent( this, ctrlPts, name );
        m_curveEntities.push_back( e );
        allCtrlPts.push_back( ctrlPts );
    }

    std::string namePt = "CtrlPt_" + std::to_string( 0 );
    auto e             = PointFactory::createPointComponent(
        this, allCtrlPts[0][0], { 0 }, namePt, Ra::Core::Utils::Color::Blue() );
    m_pointEntities.push_back( e );
    int nameIndex = 1;
    for ( unsigned int i = 0; i < allCtrlPts.size(); i++ ) {
        for ( unsigned int j = 1; j < allCtrlPts[i].size() - 1; j++ ) {
            namePt = "CtrlPt_" + std::to_string( nameIndex );
            e      = PointFactory::createPointComponent( this, allCtrlPts[i][j], { i }, namePt );
            m_pointEntities.push_back( e );
            nameIndex++;
        }
        namePt = "CtrlPt_" + std::to_string( nameIndex );
        if ( i == allCtrlPts.size() - 1 )
            e = PointFactory::createPointComponent(
                this, allCtrlPts[i][3], { i }, namePt, Ra::Core::Utils::Color::Blue() );
        else
            e = PointFactory::createPointComponent(
                this, allCtrlPts[i][3], { i, i + 1 }, namePt, Ra::Core::Utils::Color::Blue() );
        m_pointEntities.push_back( e );
        nameIndex++;
    }
    m_viewer->doneCurrent();
    m_viewer->getRenderer()->buildAllRenderTechniques();
    m_viewer->needUpdate();
}

CurveEditor::~CurveEditor() {}

unsigned int CurveEditor::getPointIndex( unsigned int curveIdSize,
                                         unsigned int currentPtIndex,
                                         const Ra::Core::Vector3& firstCtrlPt,
                                         const Ra::Core::Vector3& currentPt ) {
    unsigned int pointIndex;
    if ( curveIdSize > 1 || currentPtIndex == m_pointEntities.size() - 1 ) {
        pointIndex = ( firstCtrlPt == currentPt ) ? 0 : 3;
    }
    else { pointIndex = ( currentPtIndex % 3 ); }
    return pointIndex;
}

void CurveEditor::updateCurve( unsigned int curveId,
                               unsigned int curveIdSize,
                               PointComponent* pointComponent,
                               const Ra::Core::Vector3& oldPoint,
                               unsigned int currentPoint ) {
    auto component = m_curveEntities[curveId];
    auto ctrlPts   = component->m_ctrlPts;
    int pointIndex = getPointIndex( curveIdSize, currentPoint, ctrlPts[0], oldPoint );

    auto ro = m_roMgr->getRenderObject( m_pointEntities[currentPoint]->getRenderObjects()[0] );
    auto transform          = ro->getTransform();
    ctrlPts[pointIndex]     = transform * pointComponent->m_defaultPoint;
    pointComponent->m_point = ctrlPts[pointIndex];

    std::string name = m_curveEntities[curveId]->getName();
    removeComponent( name );
    auto e                   = CurveFactory::createCurveComponent( this, ctrlPts, name );
    m_curveEntities[curveId] = e;
}

void CurveEditor::refreshPoint( unsigned int pointIndex, const Ra::Core::Vector3& newPoint ) {
    auto pointName      = m_pointEntities[pointIndex]->getName();
    auto pointComponent = m_pointEntities[pointIndex];
    auto pointColor     = pointComponent->m_color;
    auto pointCurve     = pointComponent->m_curveId;
    Ra::Core::Vector3 point;
    if ( newPoint == Ra::Core::Vector3::Zero() )
        point = pointComponent->m_point;
    else
        point = newPoint;
    removeComponent( m_pointEntities[pointIndex]->getName() );
    auto pointEntity =
        PointFactory::createPointComponent( this, point, pointCurve, pointName, pointColor );
    m_pointEntities[pointIndex] = pointEntity;
}

void CurveEditor::updateCurves( bool onRelease ) {

    if ( m_currentPoint < 0 ) return;

    auto pointComponent      = m_pointEntities[m_currentPoint];
    auto oldPoint            = pointComponent->m_point;
    unsigned int curveIdSize = pointComponent->m_curveId.size();

    CurveFactory factory;
    m_viewer->makeCurrent();

    for ( unsigned int i = 0; i < curveIdSize; i++ ) {
        auto curveId = pointComponent->m_curveId[i];
        updateCurve( curveId, curveIdSize, pointComponent, oldPoint, m_currentPoint );
    }

    for ( unsigned int i = 0; i < m_tangentPoints.size(); i++ ) {
        auto tangentPtComponent     = m_pointEntities[m_tangentPoints[i]];
        Ra::Core::Vector3 tangentPt = tangentPtComponent->m_point;
        auto symCurveId             = tangentPtComponent->m_curveId[0];
        updateCurve( symCurveId, 1, tangentPtComponent, tangentPt, m_tangentPoints[i] );
    }

    // Necessary for transform re-initialization
    if ( onRelease ) {
        refreshPoint( m_currentPoint );
        for ( unsigned int i = 0; i < m_tangentPoints.size(); i++ ) {
            refreshPoint( m_tangentPoints[i] );
        }
        m_tangentPoints.clear();
    }

    m_viewer->doneCurrent();
    m_viewer->getRenderer()->buildAllRenderTechniques();
    m_viewer->needUpdate();
}

Ra::Core::Transform CurveEditor::computePointTransform( PointComponent* pointCmp,
                                                        const Ra::Core::Vector3& midPoint,
                                                        const Ra::Core::Vector3& worldPos ) {
    auto transform         = Ra::Core::Transform::Identity();
    Ra::Core::Vector3 diff = ( midPoint - worldPos );
    if ( m_symetry ) { transform.translate( ( midPoint + diff ) - pointCmp->m_defaultPoint ); }
    else {
        diff.normalize();
        auto actualdiff = diff * ( midPoint - pointCmp->m_point ).norm();
        transform.translate( ( midPoint + actualdiff ) - pointCmp->m_defaultPoint );
    }
    return transform;
}

inline float CurveEditor::distanceSquared( const Ra::Core::Vector3f& pointA,
                                           const Ra::Core::Vector3f& pointB ) {
    float dx = pointA.x() - pointB.x();
    float dy = pointA.y() - pointB.y();
    float dz = pointA.z() - pointB.z();
    return dx * dx + dy * dy + dz * dz;
}

// De Casteljau algorithm
void CurveEditor::subdivisionBezier( int vertexIndex,
                                     unsigned int curveIndex,
                                     const Ra::Core::Vector3Array& ctrlPts ) {
    auto bezier =
        Ra::Core::Geometry::CubicBezier( Ra::Core::Vector2( ctrlPts[0].x(), ctrlPts[0].z() ),
                                         Ra::Core::Vector2( ctrlPts[1].x(), ctrlPts[1].z() ),
                                         Ra::Core::Vector2( ctrlPts[2].x(), ctrlPts[2].z() ),
                                         Ra::Core::Vector2( ctrlPts[3].x(), ctrlPts[3].z() ) );
    float u = float( vertexIndex ) / 100.f;

    Ra::Core::Vector2 fu          = bezier.f( u );
    auto clickedPoint             = Ra::Core::Vector3( fu.x(), 0, fu.y() );
    Ra::Core::Vector3 firstPoint  = Ra::Core::Math::linearInterpolate( ctrlPts[0], ctrlPts[1], u );
    Ra::Core::Vector3 sndPoint    = Ra::Core::Math::linearInterpolate( ctrlPts[1], ctrlPts[2], u );
    Ra::Core::Vector3 thirdPoint  = Ra::Core::Math::linearInterpolate( ctrlPts[2], ctrlPts[3], u );
    Ra::Core::Vector3 fourthPoint = Ra::Core::Math::linearInterpolate( firstPoint, sndPoint, u );
    Ra::Core::Vector3 fifthPoint  = Ra::Core::Math::linearInterpolate( sndPoint, thirdPoint, u );

    auto newCtrlPts = ctrlPts;
    newCtrlPts[1]   = firstPoint;
    newCtrlPts[2]   = fourthPoint;
    newCtrlPts[3]   = clickedPoint;
    auto nameCurve  = m_curveEntities[curveIndex]->getName();
    removeComponent( nameCurve );
    m_curveEntities[curveIndex] = CurveFactory::createCurveComponent( this, newCtrlPts, nameCurve );

    refreshPoint( curveIndex * 3 + 1, firstPoint );
    refreshPoint( curveIndex * 3 + 2, thirdPoint );

    auto firstInsertionIdx = curveIndex * 3 + 2;
    std::string namePt     = "CtrlPt_" + std::to_string( m_pointEntities.size() );
    auto ptE               = PointFactory::createPointComponent( this,
                                                   clickedPoint,
                                                                 { curveIndex, curveIndex + 1 },
                                                   namePt,
                                                   Ra::Core::Utils::Color::Blue() );
    m_pointEntities.insert( m_pointEntities.begin() + firstInsertionIdx, ptE );
    namePt = "CtrlPt_" + std::to_string( m_pointEntities.size() );
    ptE    = PointFactory::createPointComponent( this, fourthPoint, { curveIndex }, namePt );
    m_pointEntities.insert( m_pointEntities.begin() + firstInsertionIdx, ptE );

    namePt = "CtrlPt_" + std::to_string( m_pointEntities.size() );
    ptE    = PointFactory::createPointComponent( this, fifthPoint, { curveIndex + 1 }, namePt );
    m_pointEntities.insert( m_pointEntities.begin() + ( ( curveIndex + 1 ) * 3 + 1 ), ptE );

    Ra::Core::Vector3Array newCtrlPts1;
    newCtrlPts1.push_back( clickedPoint );
    newCtrlPts1.push_back( fifthPoint );
    newCtrlPts1.push_back( thirdPoint );
    newCtrlPts1.push_back( ctrlPts[3] );

    nameCurve   = "Curve_" + std::to_string( m_curveEntities.size() );
    auto curveE = CurveFactory::createCurveComponent( this, newCtrlPts1, nameCurve );

    m_curveEntities.insert( m_curveEntities.begin() + curveIndex + 1, curveE );

    for ( unsigned int i = ( ( curveIndex + 1 ) * 3 + 2 ); i < m_pointEntities.size(); i++ ) {
        auto ptCmp = m_pointEntities[i];
        for ( unsigned int j = 0; j < ptCmp->m_curveId.size(); j++ ) {
            ptCmp->m_curveId[j] += 1;
        }
    }
}

void CurveEditor::addPointAtEnd( const Ra::Core::Vector3& worldPos ) {
    Ra::Core::Vector3Array ctrlPts;
    auto lastIndex         = m_pointEntities.size() - 1;
    auto beforeLast        = m_pointEntities[lastIndex - 1];
    auto last              = m_pointEntities[lastIndex];
    Ra::Core::Vector3 diff = ( last->m_point - beforeLast->m_point );
    diff.normalize();
    Ra::Core::Vector3 secondPt  = ( last->m_point + diff );
    Ra::Core::Vector3 thirdDiff = ( last->m_point - worldPos );
    thirdDiff.normalize();
    Ra::Core::Vector3 thirdPt = ( worldPos - diff );
    ctrlPts.push_back( last->m_point );
    ctrlPts.push_back( secondPt );
    ctrlPts.push_back( thirdPt );
    ctrlPts.push_back( worldPos );
    std::string name = "Curve_" + std::to_string( m_curveEntities.size() );
    auto e           = CurveFactory::createCurveComponent( this, ctrlPts, name );
    m_curveEntities.push_back( e );

    unsigned int pointIndex = lastIndex + 1;
    last->m_curveId.push_back( ( pointIndex / 3 ) );
    std::string namePt = "CtrlPt_" + std::to_string( pointIndex );
    auto ptC =
        PointFactory::createPointComponent( this, ctrlPts[1], { ( pointIndex / 3 ) }, namePt );
    m_pointEntities.push_back( ptC );
    namePt = "CtrlPt_" + std::to_string( pointIndex + 1 );
    auto eb =
        PointFactory::createPointComponent( this, ctrlPts[2], { ( pointIndex / 3 ) }, namePt );
    m_pointEntities.push_back( eb );
    namePt = "CtrlPt_" + std::to_string( pointIndex + 2 );
    ptC    = PointFactory::createPointComponent(
        this, ctrlPts[3], { ( pointIndex / 3 ) }, namePt, Ra::Core::Utils::Color::Blue() );
    m_pointEntities.push_back( ptC );
}

void CurveEditor::addPointInCurve( const Ra::Core::Vector3& worldPos, int mouseX, int mouseY ) {
    auto camera = m_viewer->getCameraManipulator()->getCamera();
    auto radius = int( m_viewer->getRenderer()->getBrushRadius() );
    bool found  = false;
    Ra::Engine::Rendering::Renderer::PickingResult pres;
    for ( int i = mouseX - radius; i < mouseX + radius && !found; i++ ) {
        for ( int j = mouseY - radius; j < mouseY + radius; j++ ) {
            // m_viewer->cursor().setPos(m_viewer->mapToGlobal(QPoint(i, j)));
            Ra::Engine::Rendering::Renderer::PickingQuery query {
                Ra::Core::Vector2( i, m_viewer->height() - j ),
                Ra::Engine::Rendering::Renderer::SELECTION,
                Ra::Engine::Rendering::Renderer::RO };
            Ra::Engine::Data::ViewingParameters renderData {
                camera->getViewMatrix(), camera->getProjMatrix(), 0 };

            pres = m_viewer->getRenderer()->doPickingNow( query, renderData );
            if ( pres.getRoIdx().isValid() && m_roMgr->exists( pres.getRoIdx() ) ) {
                auto ro = m_roMgr->getRenderObject( pres.getRoIdx() );
                if ( ro->getMesh()->getNumVertices() == 2 ) continue;
                pres.removeDuplicatedIndices();
                auto e = ro->getComponent();

                int curveIndex = -1;
                for ( int i = 0; i < m_curveEntities.size(); i++ ) {
                    if ( m_curveEntities[i] == e ) {
                        curveIndex = i;
                        break;
                    }
                }
                if ( curveIndex < 0 ) continue;

                auto meshPtr  = ro->getMesh().get();
                auto mesh     = dynamic_cast<Ra::Engine::Data::Mesh*>( meshPtr );
                auto curveCmp = static_cast<CurveComponent*>( ro->getComponent() );
                auto ctrlPts  = curveCmp->m_ctrlPts;

                if ( mesh != nullptr ) {
                    float bestV = std::numeric_limits<float>::max();
                    int vIndex  = -1;
                    for ( unsigned int w = 0; w < mesh->getNumVertices(); w++ ) {
                        auto dist =
                            distanceSquared( worldPos, mesh->getCoreGeometry().vertices()[w] );
                        if ( dist < bestV ) {
                            bestV  = dist;
                            vIndex = w;
                        }
                    }

                    subdivisionBezier( vIndex, curveIndex, ctrlPts );
                }
                found = true;
                break;
            }
        }
    }
}

bool CurveEditor::processHover( std::shared_ptr<Ra::Engine::Rendering::RenderObject> ro ) {
    auto e = ro->getComponent();

    // if not a control point -> do nothing
    for ( int i = 0; i < m_pointEntities.size(); i++ ) {
        if ( e == m_pointEntities[i] ) {
            m_currentPoint = i;
            break;
        }
    }
    if ( m_currentPoint < 0 ) return false;

    ro->getMaterial()->getParameters().addParameter( "material.color",
                                                     Ra::Core::Utils::Color::Red() );
    m_selectedRo = ro;
    return true;
}

void CurveEditor::processUnhovering() {
    auto pointCmp = static_cast<PointComponent*>( m_selectedRo->getComponent() );
    m_selectedRo->getMaterial()->getParameters().addParameter( "material.color",
                                                               pointCmp->m_color );
    m_selectedRo   = nullptr;
    m_currentPoint = -1;
}

void CurveEditor::processPicking( const Ra::Core::Vector3& worldPos ) {
    if ( m_currentPoint < 0 ) return;
    auto pointComponent = static_cast<PointComponent*>( m_selectedRo->getComponent() );
    auto point          = pointComponent->m_point;

    auto transformTranslate = Ra::Core::Transform::Identity();
    transformTranslate.translate( worldPos - point );

    auto transform = Ra::Core::Transform::Identity();
    transform      = m_selectedRo->getLocalTransform() * transformTranslate;
    m_selectedRo->setLocalTransform( transform );

    if ( m_smooth && !( m_currentPoint == 0 || m_currentPoint == 1 ||
                        m_currentPoint == m_pointEntities.size() - 2 ||
                        m_currentPoint == m_pointEntities.size() - 1 ) ) {

        if ( m_currentPoint % 3 == 2 ) {
            auto pointMid  = m_pointEntities[m_currentPoint + 1];
            pointComponent = m_pointEntities[m_currentPoint + 2];
            auto symTransform =
                computePointTransform( pointComponent, pointMid->m_point, worldPos );
            auto ro = m_roMgr->getRenderObject( pointComponent->getRenderObjects()[0] );
            ro->setLocalTransform( symTransform );
            if ( m_tangentPoints.empty() ) { m_tangentPoints.push_back( m_currentPoint + 2 ); }
        }
        else if ( m_currentPoint % 3 == 1 ) {
            auto pointMid  = m_pointEntities[m_currentPoint - 1];
            pointComponent = m_pointEntities[m_currentPoint - 2];
            auto symTransform =
                computePointTransform( pointComponent, pointMid->m_point, worldPos );
            auto ro = m_roMgr->getRenderObject( pointComponent->getRenderObjects()[0] );
            ro->setLocalTransform( symTransform );
            if ( m_tangentPoints.empty() ) { m_tangentPoints.push_back( m_currentPoint - 2 ); }
        }
        else if ( m_currentPoint % 3 == 0 ) {
            auto leftRo = m_roMgr->getRenderObject(
                m_pointEntities[m_currentPoint - 1]->getRenderObjects()[0] );
            auto leftTransform = leftRo->getTransform() * transformTranslate;
            leftRo->setLocalTransform( leftTransform );
            auto rightRo = m_roMgr->getRenderObject(
                m_pointEntities[m_currentPoint + 1]->getRenderObjects()[0] );
            auto rightTransform = rightRo->getTransform() * transformTranslate;
            rightRo->setLocalTransform( rightTransform );
            if ( m_tangentPoints.empty() ) {
                m_tangentPoints.push_back( m_currentPoint - 1 );
                m_tangentPoints.push_back( m_currentPoint + 1 );
            }
        }
    }
    updateCurves();
}
