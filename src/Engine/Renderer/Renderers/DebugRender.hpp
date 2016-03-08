#pragma once

#include <vector>

#include <Core/Utils/Singleton.hpp>

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

            virtual void drawLine(const Core::Vector3& from, 
                                  const Core::Vector3& to, 
                                  const Core::Color& color);

        private:
            struct Line
            {
                Line(const Core::Vector3& la, const Core::Vector3& lb, 
                     const Core::Color& lcol)
                    : a(la), b(lb), col(lcol) {}

                Core::Vector3 a, b;
                Core::Color col;
            };

        private:
            ShaderProgram* m_shaderProgram;

            std::vector<Line> m_lines;
        };
    }
}
