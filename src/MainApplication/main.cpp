#include <MainApplication/MainApplication.hpp>

#include <Core/Log/Log.hpp>

int main(int argc, char** argv)
{
    Ra::MainApplication app(argc, argv);
    return app.exec();
}
