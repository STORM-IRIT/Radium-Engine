# this file set variables pointing to local installation of dependencies.
# if a dependency is not set here, it will be fetched and compiled during the configure step of Radium
# use this file in your cmake command by using
#   mkdir build && cd build && cmake -C ../localDependencies.cmake -DYourVariable=AValue ..
# for now, only Eigen3 is supported (NOTE : using the eigen3 from macport does not allow to compile Radium, the eigen3 version we use is newer than the one packaged by macport)
set(Eigen3_DIR /opt/local/share/eigen3/cmake CACHE PATH "Local Eigen3 package")
