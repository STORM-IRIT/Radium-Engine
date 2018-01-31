#ifndef RADIUMENGINE_VERTEXPICKING_HPP
#define RADIUMENGINE_VERTEXPICKING_HPP

#include <GuiBase/RaGuiBase.hpp>

#include <Engine/RaEngine.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Core/Mesh/MeshPrimitives.hpp>
#include <Engine/Component/Component.hpp>

#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

#include <QObject>

namespace Ra
{
    namespace Gui
    {
        /// Data representing a feature on a Render Object.
        /// If m_featureType==RO, this means that no feature is selected.
        /// Besides the Render Object index, the data are:
        ///  - for vertices: the index.
        ///  - for edges: the vertices indices.
        ///  - for triangles and faces: the triangle/face index and its vertices indices.
        struct RA_GUIBASE_API FeatureData
        {
            FeatureData() : m_featureType(Engine::Renderer::RO), m_roIdx(-1)
            {}
            
            Engine::Renderer::PickingMode m_featureType;
            std::vector< int > m_data;
            uint m_roIdx;
        };
        
        class RA_GUIBASE_API FeaturePickingManager
        {
        public:
            FeaturePickingManager();
            ~FeaturePickingManager();
            
            /// Registers the index of the first RenderObject from which feature picking is enabled.
            inline void setMinRenderObjectIndex(uint id)
            {
                m_firstRO = id;
            }
            
            /// Performs the picking of the feature on the given Render Object, according to the picking query and the cast ray.
            void doPicking( int roIndex, const Engine::Renderer::PickingQuery& query, const Core::Ray& ray );
            
            /// Const accessor to the selected feature data.
            inline const FeatureData& getFeatureData() const
            {
                return m_FeatureData;
            }
            
            /// Non-const accessor to the selected feature data.
            inline FeatureData& getFeatureData()
            {
                return m_FeatureData;
            }
            
            /// Clears the selection.
            void clearFeature();
            
            /// Sets the selection to the idx-th vertex.
            /// Note: Only if the selected feature is in VERTEX mode and id is a valid vertex id.
            void setVertexIndex(int id);
            
            /// Sets the selection to the idx-th triangle.
            /// Note: Only if the selected feature is in TRIANGLE mode and id is a valid triangle id.
            void setTriangleIndex(int id);
            
        private:
            /// The selected feature data.
            FeatureData m_FeatureData;
            /// The first Render Object on which Feature Picking is allowed.
            uint m_firstRO;
        };
    } // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_VERTEXPICKING_HPP
