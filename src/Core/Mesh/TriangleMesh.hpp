#ifndef RADIUMENGINE_TRIANGLEMESH_HPP
#define RADIUMENGINE_TRIANGLEMESH_HPP

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/MeshTypes.hpp>

namespace Ra
{
    namespace Core
    {


class VertexAttribBase{
public :
    std::string getName() const {return m_name;}
    virtual void resize(size_t s) = 0;
    std::string m_name;
};

template <typename T>
class VertexAttrib : public VertexAttribBase{
public:
    typedef VectorArray<T> data_type;
    using value_type = T;
//    using Container = VectorArray<T>;
    void resize(size_t s) override {
        m_data.resize(s);
    }
    inline data_type &data(){return m_data;}
    inline const data_type &data() const {return m_data;}
    data_type m_data;
};

template <typename T>
class VertexAttribHandle{
public:
    typedef T value_type;
    typedef typename VertexAttrib<T>::data_type data_type;
    int m_idx;
};

class VertexAttribManager{
public:
    std::vector<VertexAttribBase *> m_attribs;

    template <typename T>
    inline VertexAttrib<T> &
    getAttrib(VertexAttribHandle<T> h){
        return *static_cast<VertexAttrib<T>*>(m_attribs[h.m_idx]);
    }

    template <typename T>
    inline const VertexAttrib<T> &
    getAttrib(VertexAttribHandle<T> h) const {
        return *static_cast<VertexAttrib<T>*>(m_attribs[h.m_idx]);
    }


    template <typename T>
    VertexAttribHandle<T>
    addAttrib(const T&, std::string name){
        VertexAttrib<T> * attrib = new VertexAttrib<T>;
        attrib->m_name = name;
        VertexAttribHandle<T> h;
        m_attribs.push_back(attrib);
        h.m_idx = m_attribs.size()-1;
        return h;
    }

    template <typename T>
    void addAttrib(VertexAttribHandle<T> &h, std::string name){
        VertexAttrib<T> * attrib = new VertexAttrib<T>;
        attrib->m_name = name;
        m_attribs.push_back(attrib);
        h.m_idx = m_attribs.size()-1;
    }
};



        /// A very basic structure representing a triangle mesh which stores the bare minimum :
        /// vertices, faces and normals. See MeshUtils for geometric functions
        /// operating on a mesh.


        struct TriangleMesh
        {
            /// Create an empty mesh.
            inline TriangleMesh() {

                m_vertexAttribs.addAttrib(m_verticesHandle, "position");
                m_vertexAttribs.addAttrib(m_normalsHandle, "normal");

            }
            /// Copy constructor and assignment operator
            TriangleMesh( const TriangleMesh& ) = default;
            TriangleMesh& operator= ( const TriangleMesh& ) = default;

            /// Erases all data, making the mesh empty.
            inline void clear();

            /// Appends another mesh to this one.
            inline void append( const TriangleMesh& other );

            using Face     = VectorNui;
            VectorArray<Triangle> m_triangles;
            VectorArray<Face>  m_faces;

            VertexAttribHandle<Vector3>::data_type &vertices(){                return m_vertexAttribs.getAttrib(m_verticesHandle).data();            }
            VertexAttribHandle<Vector3>::data_type &normals(){                return m_vertexAttribs.getAttrib(m_normalsHandle).data();            }

            const VertexAttribHandle<Vector3>::data_type &vertices() const {                return m_vertexAttribs.getAttrib(m_verticesHandle).data();            }
            const VertexAttribHandle<Vector3>::data_type &normals() const {                return m_vertexAttribs.getAttrib(m_normalsHandle).data();            }


            VertexAttribManager m_vertexAttribs;
            using vertex_attrib_data_type = VertexAttribHandle<Vector3>;
            using normal_attrib_data_type = VertexAttribHandle<Vector3>;
            vertex_attrib_data_type m_verticesHandle;
            normal_attrib_data_type m_normalsHandle;


        public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        };


    }
}

#include <Core/Mesh/TriangleMesh.inl>

#endif //RADIUMENGINE_TRIANGLEMESH_HPP
