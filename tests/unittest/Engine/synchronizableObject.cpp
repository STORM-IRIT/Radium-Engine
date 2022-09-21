#include <catch2/catch.hpp>

#include <mutex>

#include <Engine/Data/SynchronizableObject.hpp>

using namespace Ra::Engine::Data;

TEST_CASE( "Engine/Data/SynchronizableObject", "[Engine][Engine/Data][SynchronizableObject]" ) {
    SECTION( "Create Synchronizable object" ) {

        Synchronizer::start();

        class SynchronizableObject : public Synchronizable
        {
          public:
            // model
            void updateData( int data ) {
                m_cpuData = data;
                needSync();
            }

          protected:
            // view
            void updateGL() override { m_gpuData = m_cpuData; }

          private:
            int m_cpuData;
            int m_gpuData;

          public: // getters
            int cpuData() const { return m_cpuData; }
            int gpuData() const { return m_gpuData; }
        };

        {
            SynchronizableObject syncObj;
            syncObj.updateData( 0 );
            REQUIRE( syncObj.cpuData() == 0 );

            Synchronizer::start();
            REQUIRE( syncObj.cpuData() == syncObj.gpuData() );

            syncObj.updateData( 1 );
            REQUIRE( syncObj.cpuData() == 1 );
            REQUIRE( 0 == syncObj.gpuData() );

            syncObj.updateData( 2 );
            REQUIRE( syncObj.cpuData() == 2 );
            REQUIRE( 0 == syncObj.gpuData() );

            Synchronizer::start();
            REQUIRE( syncObj.cpuData() == syncObj.gpuData() );

            syncObj.updateData( 3 ); // update and destroy before synchronize
        }
        // this line does nothing because the destroyed object cannot be synchronized
        Synchronizer::start();

        // concurrency test
        {
            SynchronizableObject syncObj2;
            syncObj2.updateData( 0 );

            constexpr int N = 1'000;
            // mutex is used here only to establish an equality test of the cpu and gpu data when
            // using concurrent threads
            std::mutex mtx;

            auto thread = std::thread( [&syncObj2, &mtx]() {
                for ( int i = 0; i < N; ++i ) {
                    mtx.lock();
                    syncObj2.updateData( i );
                    mtx.unlock();
                    std::this_thread::sleep_for( std::chrono::microseconds( 1 ) );
                }
            } );

            for ( int i = 0; i < N; ++i ) {
                mtx.lock();
                Synchronizer::start();
                REQUIRE( syncObj2.cpuData() == syncObj2.gpuData() );
                //                std::cout << syncObj2.cpuData() << std::endl;
                mtx.unlock();
                std::this_thread::sleep_for( std::chrono::microseconds( 3 ) );
            }

            thread.join();
            Synchronizer::start();
            REQUIRE( syncObj2.cpuData() == syncObj2.gpuData() );
        }
    }
}
