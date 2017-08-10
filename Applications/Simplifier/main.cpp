#include <QCoreApplication>
#include <QTimer>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QPluginLoader>
#include <QCommandLineParser>
#include <QOpenGLContext>

#include <GuiBase/Viewer/Viewer.hpp>
#include <GuiBase/TimerData/FrameTimerData.hpp>

#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <PluginBase/RadiumPluginInterface.hpp>

#include <simplifier.hpp>

#ifdef IO_USE_ASSIMP
    #include <IO/AssimpLoader/AssimpFileLoader.hpp>
#endif
#ifdef IO_USE_PBRT
    #include <IO/PbrtLoader/PbrtFileLoader.hpp>
#endif


namespace Ra
{
    enum SimplificationOption { NFACES, NPERCENT, NLOD };

    //Class inheriting from QCoreApplication just for parameters management. No loop use in this programme.
    class SimplifierApp : public QCoreApplication
    {
    public:
        SimplifierApp(int& argc, char** argv)
                : QCoreApplication(argc, argv)
                , _engine(Ra::Engine::RadiumEngine::createInstance())
                ,_task_queue(new Ra::Core::TaskQueue(std::thread::hardware_concurrency() - 1))
                , m_isAboutToQuit( false )
                , m_facePercentage( -1 )
                , m_faceNumber( -1 )
                , m_lodNumber( 1 )
        {

            QCommandLineParser parser;
            parser.setApplicationDescription("Radium Engine - Simplifier");
            parser.addHelpOption();
            parser.addVersionOption();

            std::string pluginsPath = "Plugins";

            QCommandLineOption fileOpt(QStringList{"f", "file", "scene"}, "Open a scene file at startup.", "file name", "foo.bar");
            QCommandLineOption pluginOpt(QStringList{"plugins", "pluginsPath"}, "Set the path to the plugin dlls.", "folder", "Plugins");
            QCommandLineOption pluginLoadOpt(QStringList{"load", "loadPlugin"}, "Only load plugin with the given name (filename without the extension). If this option is not used, all plugins in the plugins folder will be loaded. ", "name");
            QCommandLineOption pluginIgnoreOpt(QStringList{"ignore", "ignorePlugin"}, "Ignore plugins with the given name. If the name appears within both load and ignore options, it will be ignored.", "name");
            //Simplificateur cmd
            QCommandLineOption lodOpt(QStringList{"l", "lod"}, "Set the number of level of details meshs that will be generated. Value 1 will genereted two meshs (100% and 50% faces keep) ", "Level Of Detail", "0");
            QCommandLineOption percentOpt(QStringList{"pr", "percentage"}, "Set face percentage keep after simplification", "Face percentage", "0");
            QCommandLineOption facesOpt(QStringList{"fa", "faces"}, "Set face number keep after simplification", "Face number", "0");
            QCommandLineOption outFileOpt(QStringList{"o", "out"}, "Output file name after simplification", "file name", "outfoo.bar");

            parser.addOptions({pluginOpt, pluginLoadOpt, pluginIgnoreOpt, fileOpt, lodOpt, percentOpt, facesOpt, outFileOpt});
            parser.process(*this);


            #ifdef IO_USE_ASSIMP
                    _engine->registerFileLoader( new IO::AssimpFileLoader() );
            #endif
            #ifdef IO_USE_PBRT
                    _engine->registerFileLoader( new IO::PbrtFileLoader() );
            #endif

            // Load plugins
            if ( !loadPlugins( pluginsPath, parser.values(pluginLoadOpt), parser.values(pluginIgnoreOpt) ) )
            {
                LOG( logERROR ) << "An error occurred while trying to load plugins.";
            }

            m_simplifierSystem = new SimplifierSystem();
            _engine->registerSystem("Simplifier system", m_simplifierSystem);

            //Parameters checking
            if(!parser.isSet(percentOpt) && !parser.isSet(facesOpt) && !parser.isSet(lodOpt))
            {
                LOG( logERROR ) << "At least one option between Percentage, Face number and Lod must be use.";
                LOG( logINFO ) << parser.helpText().toStdString();
                appNeedsToQuit();
            }
            else if (!((parser.isSet(percentOpt) && !parser.isSet(facesOpt) && !parser.isSet(lodOpt)) ||
                (!parser.isSet(percentOpt) && parser.isSet(facesOpt) && !parser.isSet(lodOpt)) ||
                (!parser.isSet(percentOpt) && !parser.isSet(facesOpt) && parser.isSet(lodOpt))))
            {
                LOG( logERROR ) << "Percentage option, faces options and lod option are exclusive. They can't be use in same time.";
                LOG( logINFO ) << parser.helpText().toStdString();
                appNeedsToQuit();
            }
            if(!parser.isSet(outFileOpt))
            {
                LOG( logERROR ) << "No output file set.";
                LOG( logINFO ) << parser.helpText().toStdString();
                appNeedsToQuit();
            }
            if (!parser.isSet(fileOpt))
            {
                LOG( logERROR ) << "No input file set.";
                LOG( logINFO ) << parser.helpText().toStdString();
                appNeedsToQuit();
            }

            if (parser.isSet(pluginOpt))    pluginsPath = parser.value(pluginOpt).toStdString();
            if (parser.isSet(fileOpt))      m_inputFile = parser.value(fileOpt);
            if (parser.isSet(lodOpt))
            {
                m_lodNumber = parser.value(lodOpt).toUInt() + 1;
                optionSelected = NLOD;
            }
            if (parser.isSet(percentOpt))
            {
                m_facePercentage = parser.value(percentOpt).toUInt();
                optionSelected = NPERCENT;
            }
            if (parser.isSet(facesOpt))
            {
                m_faceNumber = parser.value(facesOpt).toUInt();
                optionSelected = NFACES;
            }
            if (parser.isSet(outFileOpt))     m_outputFile = parser.value(outFileOpt).toStdString();

        }

        ~SimplifierApp()
        {
            _engine->cleanup();
        }

        void appStart()
        {
            //Load file
            QFileInfo fileInfo(m_inputFile);
            if(fileInfo.isDir())
            {
                LOG(logERROR) << "Directory path detected. Please use only file path";
            }
            else
            {
                 if(!loadFile(m_inputFile))
                 {
                     LOG(logERROR) << "Model can not be loaded";
                     appNeedsToQuit();
                     return;
                 }
            }

            //Compute new mesh
            int initialFacesNumber = m_simplifierSystem->getMeshContactManager()->getNbFacesMax();
            if(optionSelected == NPERCENT)
            {
                m_facePercentage = std::min(100,m_facePercentage);
                m_facePercentage = std::max(m_facePercentage,0);
                startSimplification(initialFacesNumber*(m_facePercentage/100.0));
                //Export result
                exportMesh(m_outputFile);
            }
            else if(optionSelected == NFACES)
            {
                startSimplification(m_faceNumber);
                //Export result
                exportMesh(m_outputFile);
            }
            else if(optionSelected == NLOD)
            {
                int percentageIncrement = 100/m_lodNumber;
                int currentLod = 1;
                startSimplification(initialFacesNumber*(percentageIncrement/100.0));
                exportMesh(m_outputFile+"_"+std::to_string(percentageIncrement*currentLod));
                while(currentLod < m_lodNumber)
                {
                    ++currentLod;
                    changeLodValue(initialFacesNumber*((percentageIncrement*currentLod)/100.0));
                    exportMesh(m_outputFile+"_"+std::to_string(percentageIncrement*currentLod));
                }
            }
            else
            {
                LOG( logERROR ) << "Bad value set for decimation";
            }
        }

        void appNeedsToQuit()
        {
            LOG( logDEBUG ) << "About to quit.";
            m_isAboutToQuit = true;
        }

        bool loadFile( QString path )
        {
            std::string pathStr = path.toLocal8Bit().data();
            LOG(logINFO) << "Loading file " << pathStr << "...";
            return _engine->loadFile( pathStr );
        }
        void startSimplificationPercentage(int facePercentage)
        {

            m_simplifierSystem->getMeshContactManager()->setNbFacesChanged(m_simplifierSystem->getMeshContactManager()->getNbFacesMax()*(facePercentage/100.0));
            m_simplifierSystem->getMeshContactManager()->setConstructM0();
        }

        void changeLodValue(int faceNumber)
        {
            m_simplifierSystem->getMeshContactManager()->setLodValueChanged(faceNumber);
        }

        void startSimplification(int faceNumber)
        {
            if(faceNumber > m_simplifierSystem->getMeshContactManager()->getNbFacesMax())
            {
               LOG(logWARNING) << "Number of faces requested is bigger than initial number of faces";
            }
            m_simplifierSystem->getMeshContactManager()->setNbFacesChanged(faceNumber);
            m_simplifierSystem->getMeshContactManager()->setConstructM0();
        }

        void exportMesh(std::string filename="")
        {
            for (SimplifierComponent* comp : m_simplifierSystem->getSimplifierComponents())
            {
                if(filename.empty())
                {
                    filename = comp->getName();
                }
                comp->exportMesh(filename);
            }
        }

        bool loadPlugins( const std::string& pluginsPath, const QStringList& loadList, const QStringList& ignoreList )
        {
            QDir pluginsDir( qApp->applicationDirPath() );
            LOG( logINFO )<<" *** Loading Plugins ***";
            bool result = pluginsDir.cd( pluginsPath.c_str() );

            if (!result)
            {
                LOG(logERROR) << "Cannot open specified plugins directory "<<pluginsPath;
                return false;
            }

            LOG( logDEBUG )<<"Plugin directory :"<<pluginsDir.absolutePath().toStdString();
            bool res = true;
            uint pluginCpt = 0;

            Ra::PluginContext context;
            context.m_engine = _engine.get();

            for (const auto& filename : pluginsDir.entryList(QDir::Files))
            {

                std::string ext = Ra::Core::StringUtils::getFileExt( filename.toStdString() );
    #if defined( OS_WINDOWS )
                std::string sysDllExt = "dll";
    #elif defined( OS_LINUX )
                std::string sysDllExt = "so";
    #elif defined( OS_MACOS )
                std::string sysDllExt = "dylib";
    #else
                static_assert( false, "System configuration not handled" );
    #endif
                if ( ext == sysDllExt )
                {
                    std::string basename = Ra::Core::StringUtils::getBaseName(filename.toStdString(),false);

                    auto stringCmp = [basename](const QString& str) { return str.toStdString() == basename;};

                    if (!loadList.empty() && std::find_if(loadList.begin(), loadList.end(),stringCmp ) == loadList.end() )
                    {
                        LOG(logDEBUG)<<"Ignoring "<<filename.toStdString()<<" (not on load list)";
                        continue;
                    }
                    if ( std::find_if (ignoreList.begin(), ignoreList.end(), stringCmp) != ignoreList.end())
                    {
                        LOG(logDEBUG)<<"Ignoring "<<filename.toStdString()<<" (on ignore list)";
                        continue;
                    }

                    QPluginLoader pluginLoader( pluginsDir.absoluteFilePath( filename ) );
                    // Force symbol resolution at load time.
                    pluginLoader.setLoadHints( QLibrary::ResolveAllSymbolsHint );

                    LOG( logINFO ) << "Found plugin " << filename.toStdString();

                    QObject* plugin = pluginLoader.instance();
                    Ra::Plugins::RadiumPluginInterface* loadedPlugin;

                    if ( plugin )
                    {
                        loadedPlugin = qobject_cast<Ra::Plugins::RadiumPluginInterface*>( plugin );
                        if ( loadedPlugin )
                        {
                            ++pluginCpt;
                            loadedPlugin->registerPlugin( context );
                        }
                        else
                        {
                            LOG( logERROR ) << "Something went wrong while trying to cast plugin"
                                            << filename.toStdString();
                            res = false;
                        }
                    }
                    else
                    {
                        LOG( logERROR ) << "Something went wrong while trying to load plugin "
                                        << filename.toStdString() << " : "
                                        << pluginLoader.errorString().toStdString();
                        res = false;
                    }
                }
            }

            if (pluginCpt == 0)
            {
                LOG(logINFO) << "No plugin found or loaded.";
            }
            else
            {
                LOG(logINFO) << "Loaded " << pluginCpt << " plugins.";
            }

            return res;
        }

        bool isAppReady()
        {
            return !m_isAboutToQuit;
        }

    public:
        // Our instance of the engine
        std::unique_ptr<Ra::Engine::RadiumEngine> _engine;

        // Task queue
        std::unique_ptr<Ra::Core::TaskQueue> _task_queue;

        SimplifierSystem* m_simplifierSystem;

        bool m_isAboutToQuit;

        int m_facePercentage;
        int m_faceNumber;
        int m_lodNumber;
        SimplificationOption optionSelected;
        std::string m_outputFile;
        QString m_inputFile;

    }; // end class
}

int main(int argc, char* argv[])
{
    // Create app
    Ra::SimplifierApp app(argc, argv);
    app._engine->initialize();

    // Load Blinn-Phong shader.Required by fancyMesh plugin.
    Ra::Engine::ShaderConfiguration bpConfig("BlinnPhong");
    bpConfig.addShader(Ra::Engine::ShaderType_VERTEX, "Shaders/BlinnPhong.vert.glsl");
    bpConfig.addShader(Ra::Engine::ShaderType_FRAGMENT, "Shaders/BlinnPhong.frag.glsl");
    Ra::Engine::ShaderConfigurationFactory::addConfiguration(bpConfig);

    if(app.isAppReady())
    {
        // Start the app.
        app.appStart();
    }
    return 0;
}

