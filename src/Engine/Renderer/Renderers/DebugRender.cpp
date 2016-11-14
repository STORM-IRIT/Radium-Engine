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
        {
        }

        DebugRender::~DebugRender()
        {
        }

        void DebugRender::initialize()
        {
            auto createProgram = [](const char* vertStr, const char* fragStr) -> uint
            {
                uint prog = glCreateProgram();

                uint vert = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vert, 1, &vertStr, 0);
                glCompileShader(vert);

                uint frag = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(frag, 1, &fragStr, 0);
                glCompileShader(frag);

                glAttachShader(prog, vert);
                glAttachShader(prog, frag);
                glLinkProgram(prog);

                glDetachShader(prog, vert);
                glDetachShader(prog, frag);

                glDeleteShader(vert);
                glDeleteShader(frag);

                return prog;
            };

            const char* lineVertStr =
                "#version 410\n"
                "layout(location = 0) in vec3 in_pos;\n"
                "layout(location = 5) in vec3 in_col;\n"
                "uniform mat4 model;\n"
                "uniform mat4 view;\n"
                "uniform mat4 proj;\n"
                "layout(location = 0) out vec3 out_color;\n"
                "void main() {\n"
                "    gl_Position = proj * view * model * vec4(in_pos, 1.0);\n"
                "    out_color = in_col;\n"
                "}\n";

            const char* lineFragStr =
                "#version 410\n"
                "layout(location = 0) in vec3 in_col;\n"
                "layout(location = 0) out vec4 out_col;\n"
                "void main() {\n"
                "    out_col = vec4(in_col, 1.0);\n"
                "}\n";

            m_lineProg = createProgram(lineVertStr, lineFragStr);

            m_modelLineLoc = glGetUniformLocation(m_lineProg, "model");
            m_viewLineLoc  = glGetUniformLocation(m_lineProg, "view");
            m_projLineLoc  = glGetUniformLocation(m_lineProg, "proj");

            static const char* pointVertStr =
                "#version 410\n"
                "layout (location = 0) in vec3 in_pos;\n"
                "layout (location = 1) in vec3 in_col;\n"
                "uniform mat4 view;\n"
                "uniform mat4 proj;\n"
                "layout(location = 0) out vec3 out_col;\n"
                "void main() {\n"
                "    gl_Position = proj * view * vec4(in_pos, 1.0);\n"
                "    out_col = in_col;\n"
                "    gl_PointSize = 40 / gl_Position.w;\n"
                "}\n";

            static const char* pointFragStr =
                "#version 410\n"
                "layout(location = 0) in vec3 in_col;\n"
                "layout(location = 0) out vec4 out_col;\n"
                "void main() {\n"
                "    out_col = vec4(in_col, 1.0);\n"
                "}\n";

            m_pointProg = createProgram(pointVertStr, pointFragStr);

            m_viewPointLoc = glGetUniformLocation(m_pointProg, "view");
            m_projPointLoc = glGetUniformLocation(m_pointProg, "proj");

            static const char* meshVertStr =
                "#version 410\n"
                "layout(location = 0) in vec3 in_pos;\n"
                "layout(location = 5) in vec3 in_col;\n"
                "uniform mat4 model;\n"
                "uniform mat4 view;\n"
                "uniform mat4 proj;\n"
                "layout(location = 0) out vec3 out_col;\n"
                "void main() {\n"
                "    gl_Position = proj * view * model * vec4(in_pos, 1.0);\n"
                "    out_col = in_col;\n"
                "}\n";

            static const char* meshFragStr =
                "#version 410\n"
                "layout(location = 0) in vec3 in_col;\n"
                "layout (location = 0) out vec4 out_col;\n"
                "void main() {\n"
                "    out_col = vec4(in_col, 1.0);\n"
                "}\n";

            m_meshProg = createProgram(meshVertStr, meshFragStr);

            m_modelMeshLoc = glGetUniformLocation(m_meshProg, "model");
            m_viewMeshLoc  = glGetUniformLocation(m_meshProg, "view");
            m_projMeshLoc  = glGetUniformLocation(m_meshProg, "proj");
        }

        void DebugRender::render(const Core::Matrix4& viewMatrix,
                                 const Core::Matrix4& projMatrix)
        {
            renderLines(viewMatrix.cast<float>(), projMatrix.cast<float>());
            renderPoints(viewMatrix.cast<float>(), projMatrix.cast<float>());
            renderMeshes(viewMatrix.cast<float>(), projMatrix.cast<float>());
        }

        void DebugRender::renderLines(const Core::Matrix4f& viewMatrix, const Core::Matrix4f& projMatrix)
        {
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
                const Core::Matrix4f id = Core::Matrix4f::Identity();

                glUseProgram(m_lineProg);
                glUniformMatrix4fv(m_modelLineLoc, 1, GL_FALSE, id.data());
                glUniformMatrix4fv(m_viewLineLoc, 1, GL_FALSE, viewMatrix.data());
                glUniformMatrix4fv(m_projLineLoc, 1, GL_FALSE, projMatrix.data());

                Mesh mesh("temp", GL_LINES);
                mesh.loadGeometry(vertices, indices);
                mesh.addData(Mesh::VERTEX_COLOR, colors);
                mesh.updateGL();
                mesh.render();
            }

            m_lines.clear();
        }

        void DebugRender::renderPoints(const Core::Matrix4f& viewMatrix, const Core::Matrix4f& projMatrix)
        {
            uint size = m_points.size();
            if (0 == size)
            {
                return;
            }

            GLuint vao, vbo;
            glGenVertexArrays(1, &vao);

            glBindVertexArray(vao);
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, size * 2 * sizeof(Core::Vector3), m_points.data(), GL_DYNAMIC_DRAW);

#ifdef CORE_USE_DOUBLE
            GLenum type = GL_DOUBLE;
#else
            GLenum type = GL_FLOAT;
#endif
            GLint64 ptr = 0;
            glVertexAttribPointer(0, 3, type, GL_FALSE, 6 * sizeof(Scalar), (GLvoid*)ptr);
            glEnableVertexAttribArray(0);
            ptr += 3 * sizeof(Scalar);
            glVertexAttribPointer(1, 3, type, GL_FALSE, 6 * sizeof(Scalar), (GLvoid*)ptr);
            glEnableVertexAttribArray(1);

            glEnable(GL_PROGRAM_POINT_SIZE);
            glUseProgram(m_pointProg);
            glUniformMatrix4fv(m_viewPointLoc, 1, GL_FALSE, viewMatrix.data());
            glUniformMatrix4fv(m_projPointLoc, 1, GL_FALSE, projMatrix.data());

            glDrawArrays(GL_POINTS, 0, size);
            glDisable(GL_PROGRAM_POINT_SIZE);

            glBindVertexArray(0);
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);

            m_points.clear();
        }

        void DebugRender::renderMeshes(const Core::Matrix4f &view, const Core::Matrix4f &proj)
        {
            if (m_meshes.empty())
            {
                return;
            }

            // Avoid too much states change
            uint idx = 0;
            std::sort(m_meshes.begin(), m_meshes.end(),
                      [](const DbgMesh& a, const DbgMesh& b) -> bool
                      {
                          return a.mesh->getRenderMode() < b.mesh->getRenderMode();
                      });

            for (; idx < m_meshes.size() && m_meshes[idx].mesh->getRenderMode() != GL_TRIANGLES; ++idx);

            glUseProgram(m_lineProg);
            glUniformMatrix4fv(m_viewLineLoc, 1, GL_FALSE, view.data());
            glUniformMatrix4fv(m_projLineLoc, 1, GL_FALSE, proj.data());

            for (uint i = 0; i < idx; ++i)
            {
                Core::Matrix4f model = m_meshes[i].transform.matrix().cast<float>();
                glUniformMatrix4fv(m_modelLineLoc, 1, GL_FALSE, model.data());
                m_meshes[i].mesh->updateGL();
                m_meshes[i].mesh->render();
            }

            glUseProgram(m_meshProg);
            glUniformMatrix4fv(m_viewMeshLoc, 1, GL_FALSE, view.data());
            glUniformMatrix4fv(m_projMeshLoc, 1, GL_FALSE, proj.data());

            for (uint i = idx; i < m_meshes.size(); ++i)
            {
                Core::Matrix4f model = m_meshes[i].transform.matrix().cast<float>();
                glUniformMatrix4fv(m_modelMeshLoc, 1, GL_FALSE, model.data());
                m_meshes[i].mesh->updateGL();
                m_meshes[i].mesh->render();
            }

            m_meshes.clear();
        }

        void DebugRender::addLine(const Core::Vector3& from,
                                   const Core::Vector3& to,
                                   const Core::Color& color)
        {
            Line l(from, to, color);
            m_lines.push_back(l);
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

        void DebugRender::addMesh(const std::shared_ptr<Mesh> &mesh, const Core::Transform& transform)
        {
            m_meshes.push_back({mesh, transform});
        }

        void DebugRender::addCross(const Core::Vector3& position,
                                   Scalar size,
                                   const Core::Color& color)
        {
            const Scalar hz = size / 2.0;
            for (int i = 0; i < 3; ++i)
            {
                Core::Vector3 offset = Core::Vector3::Zero();
                offset[i] = hz;

                const Core::Vector3 from = position - offset;
                const Core::Vector3 to   = position + offset;

                addLine(from, to, color);
            }
        }

        void DebugRender::addSphere(const Core::Vector3& center,
                                    Scalar radius,
                                    const Core::Color& color)
        {
            addMesh(DrawPrimitives::Sphere(center, radius, color));
        }

        void DebugRender::addCircle(const Core::Vector3& center,
                                    const Core::Vector3& normal,
                                    Scalar radius,
                                    const Core::Color& color)
        {
            addMesh(DrawPrimitives::Circle(center, normal, radius, 64, color));
        }

        void DebugRender::addFrame(const Core::Transform& transform,
                                   Scalar size)
        {
            addMesh(DrawPrimitives::Frame(transform, size));
        }

        void DebugRender::addTriangle(const Core::Vector3& p0,
                                      const Core::Vector3& p1,
                                      const Core::Vector3& p2,
                                      const Core::Color& color)
        {
            addMesh(DrawPrimitives::Triangle(p0, p1, p2, color));
        }

        void DebugRender::addAABB(const Core::Aabb& box, const Core::Color& color)
        {
            addMesh(DrawPrimitives::AABB(box, color));
        }

        void DebugRender::addOBB(const Core::Aabb& box, const Core::Transform& transform, const Core::Color& color)
        {
            addMesh(DrawPrimitives::AABB(box, color), transform);
        }

        RA_SINGLETON_IMPLEMENTATION(DebugRender);
    }
}
