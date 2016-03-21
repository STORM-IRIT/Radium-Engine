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
            : m_lineShader(nullptr)
        {
        }

        DebugRender::~DebugRender()
        {
        }

        void DebugRender::initialize()
        {
            m_lineShader = ShaderProgramManager::getInstance()->addShaderProgram("DebugLine");
            m_pointShader = ShaderProgramManager::getInstance()->addShaderProgram("DebugPoint");
            m_plainShader = ShaderProgramManager::getInstance()->getShaderProgram(ShaderConfiguration("Plain", "../Shaders"));
        }

        void DebugRender::render(const Core::Matrix4& viewMatrix, 
                                 const Core::Matrix4& projMatrix)
        {
            renderLines(viewMatrix, projMatrix);
            renderPoints(viewMatrix, projMatrix);
            renderMeshes(viewMatrix, projMatrix);
        }

        void DebugRender::renderLines(const Core::Matrix4& viewMatrix, const Core::Matrix4& projMatrix)
        {
            if (nullptr == m_lineShader)
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

            if (vertices.size() > 0)
            {
                m_lineShader->bind();
                m_lineShader->setUniform("view", viewMatrix);
                m_lineShader->setUniform("proj", projMatrix);

                Mesh mesh("temp", GL_LINES);
                mesh.loadGeometry(vertices, indices);
                mesh.addData(Mesh::VERTEX_COLOR, colors);
                mesh.updateGL();
                mesh.render();
            }

            m_lines.clear();
        }

        void DebugRender::renderPoints(const Core::Matrix4& viewMatrix, const Core::Matrix4& projMatrix)
        {
            if (nullptr == m_pointShader)
            {
                m_points.clear();

                return;
            }

            uint size = m_points.size();
            if (0 == size)
            {
                return;
            }

            if (0 == m_pointVao)
            {
                glGenVertexArrays(1, &m_pointVao);
                glBindVertexArray(m_pointVao);
                glGenBuffers(1, &m_pointVbo);
                glBindVertexArray(0);
            }

            glBindVertexArray(m_pointVao);

            glBindBuffer(GL_ARRAY_BUFFER, m_pointVbo);
            glBufferData(GL_ARRAY_BUFFER, size * 2 * sizeof(Core::Vector3), m_points.data(), GL_DYNAMIC_DRAW);

#ifdef CORE_USE_DOUBLE
            GLenum type = GL_DOUBLE;
#else
            GLenum type = GL_FLOAT;
#endif
            GLvoid* ptr = nullptr;
            glVertexAttribPointer(0, 3, type, GL_FALSE, 6 * sizeof(Scalar), ptr);
            glEnableVertexAttribArray(0);
            ptr += 3 * sizeof(Scalar);
            glVertexAttribPointer(1, 3, type, GL_FALSE, 6 * sizeof(Scalar), ptr);
            glEnableVertexAttribArray(1);

            glEnable(GL_PROGRAM_POINT_SIZE);
            m_pointShader->bind();
            m_pointShader->setUniform("view", viewMatrix);
            m_pointShader->setUniform("proj", projMatrix);

            glDrawArrays(GL_POINTS, 0, size);
            glDisable(GL_PROGRAM_POINT_SIZE);

            glBindVertexArray(0);

            m_points.clear();
        }

        void DebugRender::renderMeshes(const Core::Matrix4 &view, const Core::Matrix4 &proj)
        {
            if (m_meshes.size() == 0)
            {
                return;
            }

            // Avoid too much states change
            uint idx = 0;
            std::sort(m_meshes.begin(), m_meshes.end(), [](const std::shared_ptr<Mesh> a, const std::shared_ptr<Mesh>& b) -> bool { return a->getRenderMode() < b->getRenderMode(); });
            for (; m_meshes[idx]->getRenderMode() != GL_TRIANGLES; ++idx);

            m_lineShader->bind();
            m_lineShader->setUniform("view", view);
            m_lineShader->setUniform("proj", proj);
            for (uint i = 0; i < idx; ++i)
            {
                m_meshes[i]->updateGL();
                m_meshes[i]->render();
            }

            Core::Matrix4 id = Core::Matrix4::Identity();
            m_plainShader->bind();
            m_plainShader->setUniform("transform.view", view);
            m_plainShader->setUniform("transform.proj", proj);
            m_plainShader->setUniform("transform.model", id);
            for (uint i = idx; i < m_meshes.size(); ++i)
            {
                m_meshes[i]->updateGL();
                m_meshes[i]->render();
            }


            m_meshes.clear();
        }

        void DebugRender::addLine(const Core::Vector3& from,
                                   const Core::Vector3& to, 
                                   const Core::Color& color)
        {
            m_lines.push_back(Line(from, to, color));
        }

        void DebugRender::addPoint(const Core::Vector3 &p, const Core::Color &c)
        {
            m_points.push_back({p,c.head<3>()});
        }

        void DebugRender::addPoints(const Core::Vector3Array& p, const Core::Color& c)
        {
            for (uint i = 0; i < p.size(); ++i)
            {
                m_points.push_back({p[i], c.head<3>()});
            }
        }

        void DebugRender::addPoints(const Core::Vector3Array &p, const Core::Vector4Array &c)
        {
            CORE_ASSERT(p.size() == c.size(), "Data sizes mismatch.");
            for (uint i = 0; i < p.size(); ++i)
            {
                m_points.push_back({p[i], c[i].head<3>()});
            }
        }

        void DebugRender::addMesh(const std::shared_ptr<Mesh> &mesh)
        {
            m_meshes.push_back(mesh);
        }

        RA_SINGLETON_IMPLEMENTATION(DebugRender);
    }
}
