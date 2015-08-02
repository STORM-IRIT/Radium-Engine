#include <MainApplication/MainApplication.hpp>

#include <Core/Log/Log.hpp>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv)
{
    // Setup EasyLogger
    el::Configurations defaultConf;
    defaultConf.setToDefault();
     // Values are always std::string
    defaultConf.set(el::Level::Info,
                    el::ConfigurationType::Format,
                    "%datetime{%y-%M-%d %H:%m:%s} [%level] %msg");

    defaultConf.set(el::Level::Debug,
                    el::ConfigurationType::Format,
                    "%datetime{%y-%M-%d %H:%m:%s} [%level] %func :\n\t%msg");

     // default logger uses default configurations
     el::Loggers::reconfigureLogger("default", defaultConf);
     LOG(INFO) << "Log using default file";
     // To set GLOBAL configurations you may use
//    defaultConf.setGlobally(
//             el::ConfigurationType::Format, "%date %msg");
//    el::Loggers::reconfigureLogger("default", defaultConf);

    Ra::MainApplication app(argc, argv);
    return app.exec();
}
