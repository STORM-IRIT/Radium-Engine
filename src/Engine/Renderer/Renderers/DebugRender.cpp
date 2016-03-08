#include <Engine/Renderer/Renderers/DebugRender.hpp>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>

namespace Ra
{
    namespace Engine
    {
        DebugRender::DebugRender()
            : m_shaderProgram(nullptr)
        {
        }

        DebugRender::~DebugRender()
        {
        }

        void DebugRender::initialize()
        {
            m_shaderProgram = ShaderProgramManager::getInstance()->
                addShaderProgram("Debug");
        }

        void DebugRender::render(const Core::Matrix4& viewMatrix, 
                                 const Core::Matrix4& projMatrix)
        {
            if (nullptr == m_shaderProgram)
            {
                m_lines.clear();
                return;
            }

            Core::Vector3Array vertices;
            Core::Vector4Array colors;
            std::vector<GLuint>  indices;
            unsigned int indexI = 0;
            for (const auto& l : m_lines)
            {
                vertices.push_back(l.a);
                vertices.push_back(l.b);

                colors.push_back(l.col);
                colors.push_back(l.col);

                indices.push_back(indexI++);
                indices.push_back(indexI++);
            }

            m_shaderProgram->bind();
            m_shaderProgram->setUniform("view", viewMatrix);
            m_shaderProgram->setUniform("proj", projMatrix);

            Mesh mesh("temp", GL_LINES);
            mesh.loadGeometry(vertices, indices);
            mesh.addData(Mesh::VERTEX_COLOR, colors);
            mesh.updateGL();
            mesh.render();

            m_lines.clear();
        }

        void DebugRender::drawLine(const Core::Vector3& from, 
                                   const Core::Vector3& to, 
                                   const Core::Color& color)
        {
            m_lines.push_back(Line(from, to, color));
        }

        RA_SINGLETON_IMPLEMENTATION(DebugRender);
    }
}
