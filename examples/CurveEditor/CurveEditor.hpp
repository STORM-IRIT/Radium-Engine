#include "CurveFactory.hpp"
#include "Gui/MyViewer.hpp"
#include <Core/Geometry/Curve2D.hpp>
#include <Engine/Rendering/RenderObject.hpp>
#include <Engine/Rendering/RenderObjectManager.hpp>
#include <Engine/Rendering/Renderer.hpp>

class CurveEditor : public Ra::Engine::Scene::Entity
{

  public:
    CurveEditor( std::vector<Ra::Core::Vector2f> polyline, MyViewer* viewer );
    ~CurveEditor();

    void addPointAtEnd( const Ra::Core::Vector3& worldPos );
    void addPointInCurve( const Ra::Core::Vector3& worldPos, int mouseX, int mouseY );

    void updateCurves( bool onRelease = false );

    bool processHover( std::shared_ptr<Ra::Engine::Rendering::RenderObject> ro );
    void processUnhovering();
    void processPicking( const Ra::Core::Vector3& worldPos );

    std::shared_ptr<Ra::Engine::Rendering::RenderObject> getSelected() { return m_selectedRo; }

    void resetSelected() {
        m_selectedRo   = nullptr;
        m_currentPoint = -1;
    }

    void setSmooth( bool smooth ) { m_smooth = smooth; }
    void setSymetry( bool symetry ) { m_symetry = symetry; }

  private:
    inline float distanceSquared( const Ra::Core::Vector3f& pointA,
                                  const Ra::Core::Vector3f& pointB );
    unsigned int getPointIndex( unsigned int curveIdSize,
                                unsigned int currentPtIndex,
                                const Ra::Core::Vector3& firstCtrlPt,
                                const Ra::Core::Vector3& currentPt );
    void updateCurve( unsigned int curveId,
                      unsigned int curveIdSize,
                      PointComponent* pointComponent,
                      const Ra::Core::Vector3& currentPt,
                      unsigned int currentPoint );
    Ra::Core::Transform computePointTransform( PointComponent* pointCmp,
                                               const Ra::Core::Vector3& midPoint,
                                               const Ra::Core::Vector3& worldPos );
    void subdivisionBezier( int vertexIndex,
                            unsigned int curveIndex,
                            const Ra::Core::Vector3Array& ctrlPts );
    void refreshPoint( unsigned int pointIndex,
                       const Ra::Core::Vector3& newPoint = Ra::Core::Vector3::Zero() );

  private:
    Ra::Engine::Scene::EntityManager* m_entityMgr;
    int m_currentPoint { -1 };
    std::shared_ptr<Ra::Engine::Rendering::RenderObject> m_selectedRo { nullptr };
    std::vector<PointComponent*> m_pointEntities;
    std::vector<CurveComponent*> m_curveEntities;
    std::vector<unsigned int> m_tangentPoints;
    Ra::Engine::Rendering::RenderObjectManager* m_roMgr;

    bool m_smooth { false };
    bool m_symetry { false };

    class CurveEditor : public Ra::Engine::Scene::Entity
    {

      public:
        CurveEditor( const Ra::Core::VectorArray<Ra::Core::Geometry::Curve2D::Vector>& polyline,
                     MyViewer* viewer );
        ~CurveEditor();

        void addPointAtEnd( const Ra::Core::Vector3& worldPos );
        void addPointInCurve( const Ra::Core::Vector3& worldPos, int mouseX, int mouseY );

        void updateCurves( bool onRelease = false );

        bool processHover( std::shared_ptr<Ra::Engine::Rendering::RenderObject> ro );
        void processUnhovering();
        void processPicking( const Ra::Core::Vector3& worldPos );

        std::shared_ptr<Ra::Engine::Rendering::RenderObject> getSelected() { return m_selectedRo; }

        void resetSelected() {
            m_selectedRo   = nullptr;
            m_currentPoint = -1;
        }

        void setSmooth( bool smooth ) { m_smooth = smooth; }
        void setSymetry( bool symetry ) { m_symetry = symetry; }

      private:
        inline float distanceSquared( const Ra::Core::Vector3f& pointA,
                                      const Ra::Core::Vector3f& pointB );
        unsigned int getPointIndex( unsigned int curveIdSize,
                                    unsigned int currentPtIndex,
                                    const Ra::Core::Vector3& firstCtrlPt,
                                    const Ra::Core::Vector3& currentPt );
        void updateCurve( unsigned int curveId,
                          unsigned int curveIdSize,
                          PointComponent* pointComponent,
                          const Ra::Core::Vector3& currentPt,
                          unsigned int currentPoint );
        Ra::Core::Transform computePointTransform( PointComponent* pointCmp,
                                                   const Ra::Core::Vector3& midPoint,
                                                   const Ra::Core::Vector3& worldPos );
        void subdivisionBezier( int vertexIndex,
                                unsigned int curveIndex,
                                const Ra::Core::Vector3Array& ctrlPts );
        void refreshPoint( unsigned int pointIndex,
                           const Ra::Core::Vector3& newPoint = Ra::Core::Vector3::Zero() );

      private:
        Ra::Engine::Scene::EntityManager* m_entityMgr;
        int m_currentPoint { -1 };
        std::shared_ptr<Ra::Engine::Rendering::RenderObject> m_selectedRo { nullptr };
        std::vector<PointComponent*> m_pointEntities;
        std::vector<CurveComponent*> m_curveEntities;
        std::vector<unsigned int> m_tangentPoints;
        Ra::Engine::Rendering::RenderObjectManager* m_roMgr;

        bool m_smooth { false };
        bool m_symetry { false };

        MyViewer* m_viewer { nullptr };
    };
