#pragma once

#include <vector>
#include <memory>

#include <Core/Utils/Singleton.hpp>
#include <Core/Containers/VectorArray.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

namespace Ra
{
    namespace Engine
    {
        class RA_ENGINE_API DebugRender
        {
            RA_SINGLETON_INTERFACE(DebugRender);

        public:
            DebugRender();
            virtual ~DebugRender();

            void initialize();
            void render(const Core::Matrix4& view, const Core::Matrix4& proj);

            virtual void addLine(const Core::Vector3& from, const Core::Vector3& to, const Core::Color& color);
            virtual void addPoint(const Core::Vector3& p, const Core::Color& color);
            virtual void addPoints(const Core::Vector3Array& p, const Core::Color& color);
            virtual void addPoints(const Core::Vector3Array& p, const Core::Vector4Array& colors);
            virtual void addMesh(const std::shared_ptr<Mesh>& mesh, const Core::Transform& transform = Core::Transform::Identity());

        private:
            struct Line
            {
                Line(const Core::Vector3& la, const Core::Vector3& lb, const Core::Color& lcol) : a(la), b(lb), col(lcol) {}

                Core::Vector3 a, b;
                Core::Color col;
            };

            struct Point
            {
                Core::Vector3 p;
                Core::Vector3 c;
            };

            struct DbgMesh
            {
                std::shared_ptr<Mesh> mesh;
                Core::Transform transform;
            };

            void renderLines(const Core::Matrix4f& view, const Core::Matrix4f& proj);
            void renderPoints(const Core::Matrix4f& view, const Core::Matrix4f& proj);
            void renderMeshes(const Core::Matrix4f& view, const Core::Matrix4f& proj);

        private:
            uint m_lineProg;
            uint m_pointProg;
            uint m_meshProg;
            
            uint m_modelLineLoc;
            uint m_viewLineLoc;
            uint m_projLineLoc;
            
            uint m_modelMeshLoc;
            uint m_viewMeshLoc;
            uint m_projMeshLoc;
            
            uint m_viewPointLoc;
            uint m_projPointLoc;

            std::vector<Line> m_lines;
            std::vector<DbgMesh> m_meshes;

            std::vector<Point> m_points;
        };
    }
}
