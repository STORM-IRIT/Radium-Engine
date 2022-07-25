#include <io/LasLoader/lasloader.hpp>

#include <Core/Asset/FileData.hpp>
#include <iostream>
#include <fstream>

using std::ifstream;
using std::ios;
using std::make_unique;
using std::to_string;
using std::runtime_error;

const string lasExt("las");

namespace Ra {
namespace IO {

LasLoader::LasLoader () {}

LasLoader::~LasLoader () {}

vector<string> LasLoader::getFileExtensions () const {
    return vector<string>({"*." + lasExt});
}

bool LasLoader::handleFileExtension(const string& extension) const {
    return extension.compare(lasExt) == 0;
}

Ra::Core::Asset::FileData* LasLoader::loadFile (const string& filename)  {
    using Ra::Core::Asset::GeometryData;
    using Ra::Core::Asset::FileData;
    using Ra::Core::VectorArray;
    using Ra::Core::Utils::logINFO;
    using Ra::Core::Utils::logWARNING;
    using Ra::Core::Utils::logERROR;

    FileData* fileData = new Ra::Core::Asset::FileData(filename);

    if (!fileData->isInitialized()) {
        delete fileData;
        LOG(logWARNING) << "Filedata could not be initialized";
        return nullptr;
    }

    auto startTime = clock();
    // a unique name is required by the component messaging system
    static int nameId = 0;
    auto geometry = make_unique<GeometryData>("LAS_PC_" + to_string(++nameId), GeometryData::POINT_CLOUD);
    if (geometry == nullptr) {
        LOG(logERROR) << "could not create geometry";
        return nullptr;
    }
    geometry->setFrame(Ra::Core::Transform::Identity());

    ifstream stream(filename, ios::out | ios::binary);

    if (!stream.is_open()) {
        throw runtime_error("Could not open binary ifstream to path " + filename );
        return nullptr;
    }
    else {
        LOG(logINFO) << "---Beginning file loading---";
    }

    /*header part*/
    char buffer[48];

    //reading minor version (needed to check data format)
    stream.seekg(0);
    stream.seekg(25);
    stream.read(buffer, 1);
    unsigned char minor = *(unsigned char*)buffer;

    if ((minor < 1) || (minor > 4)) {
        delete fileData;
        LOG(logERROR) << "LAS version 1." << (int)minor << " unsupported";
        return nullptr;
    }
    else {
        LOG(logINFO) << "LAS version 1." << (int)minor;
    }

    //reading distance from file start to first point record
    stream.seekg(0);
    stream.seekg(96);
    stream.read(buffer, 4);
    unsigned int offset = *(unsigned int*)buffer;

    //reading point data format
    stream.seekg(0);
    stream.seekg(104);
    stream.read(buffer, 1);
    unsigned char data_format = *(unsigned char*)buffer;

    if ((data_format < 0)
            || (data_format > 3 && minor < 3)
            || (data_format > 5 && minor == 3)
            || (data_format > 6 && minor == 4)) {
        delete fileData;
        throw runtime_error("Corrupted file. Unvalid data format");
        return nullptr;
    }
    else {
        LOG(logINFO) << "Data format " << (int)data_format;
        LOG(logINFO) << "Loading properties \"x\", \"y\", \"z\",";
    }

    //reading data record length (bytes)
    stream.read(buffer, 2);
    unsigned short data_len = *(unsigned short*)buffer;

    //reading total number of data records
    stream.read(buffer, 4);
    unsigned int nb_data = *(unsigned int*)buffer;

    //reading scales and offsets (used for computing coordinates)
    stream.seekg(0);
    stream.seekg(131);
    stream.read(buffer, 48);

    double scale_x = *(double *)buffer;
    double scale_y = *(double *)(buffer + 8);
    double scale_z = *(double *)(buffer + 16);
    double offset_x = *(double *)(buffer + 24);
    double offset_y = *(double *)(buffer + 32);
    double offset_z = *(double *)(buffer + 40);

    /*loading properties*/
    char* point = (char*)malloc(data_len * sizeof(char));

    VectorArray<Ra::Core::Vector3>& vertices = geometry->getGeometry().verticesWithLock();
    vertices.reserve(nb_data);

    //checking for colors
    bool colors = false;
    auto handle_color = geometry->getGeometry().vertexAttribs().addAttrib<Ra::Core::Vector4>(
                Ra::Core::Geometry::getAttribName(Ra::Core::Geometry::MeshAttrib::VERTEX_COLOR));
    VectorArray<Ra::Core::Vector4>& color = geometry->getGeometry().vertexAttribs().getDataWithLock(handle_color);
    if (data_format == 2 || data_format == 3 || (minor >= 3 && data_format == 5)) {
        colors = true;
        LOG(logINFO) << "\"red\", \"green\", \"blue\".";
    }

    //checking for GPS Time
    bool gps_time = false;
    auto handle_time = geometry->getGeometry().vertexAttribs().addAttrib<Scalar>("gps_time");
    VectorArray<Scalar>& time = geometry->getGeometry().vertexAttribs().getDataWithLock(handle_time);
    if ((data_format == 1 )
            || (data_format == 3)
            || ((minor > 2) && (data_format == 4))
            || ((minor > 2) && (data_format == 5))
            || ((minor == 4) && (data_format == 6))) {
        gps_time = true;
        LOG(logINFO) << "\"GPS time\".";
    }

    for (unsigned int i = 0;i < nb_data;++i) {
        //positioning at point data location in file
        int pos = offset + i * data_len;
        stream.seekg(0);
        stream.seekg(pos);

        //reading point data
        stream.read(point, data_len);

        //extracting initial x y z coordinates
        int ix;
        int iy;
        int iz;

        ix = *(int *)point;
        iy = *(int *)(point + 4);
        iz = *(int *)(point + 8);

        //computing actual coordinates
        double x = (double) ix * scale_x + offset_x;
        double y = (double) iy * scale_y + offset_y;
        double z = (double) iz * scale_z + offset_z;

        //storing coordinates in vertices object
        vertices.emplace_back(Scalar(x), Scalar(y), Scalar(z));

        //computing colors if found
        if (colors) {
            unsigned short red, green, blue;

            if (data_format == 5 || data_format == 3) {
                red   = *(unsigned short*)(point + 28);
                green = *(unsigned short*)(point + 30);
                blue  = *(unsigned short*)(point + 32);
            }
            else {
                red   = *(unsigned short*)(point + 20);
                green = *(unsigned short*)(point + 22);
                blue  = *(unsigned short*)(point + 24);
            }

            color.emplace_back(Scalar(red), Scalar(green), Scalar(blue), 1_ra);
        }

        if (gps_time) {
            //computing GPS time if found
            if (data_format == 6) {
                time.emplace_back()=(Scalar(*(double*)(point + 22)));
            }
            else {
                time.emplace_back()=(Scalar(*(double*)(point + 20)));
            }
        }
    }
    stream.close();

    geometry->getGeometry().verticesUnlock();
    geometry->getGeometry().vertexAttribs().unlock(handle_color);
    geometry->getGeometry().vertexAttribs().unlock(handle_time);

    //removing colors if not found
    if (!colors) {
        geometry->getGeometry().vertexAttribs().removeAttrib(handle_color);
    }

    //removing gps time if not found
    if (!gps_time) {
        geometry->getGeometry().vertexAttribs().removeAttrib(handle_time);
    }

    free(point);

    //finalizing
    fileData->m_geometryData.clear();
    fileData->m_geometryData.reserve(1);
    fileData->m_geometryData.push_back(move(geometry));

    fileData->m_loadingTime = (clock() - startTime) / Scalar(CLOCKS_PER_SEC);

    LOG(logINFO) << "---File loading ended---";

    fileData->displayInfo();

    fileData->m_processed = true;

    return fileData;
}

string LasLoader::name () const {
    return "LASer (.las)";
}

} //namespace IO
} //namespace Ra
