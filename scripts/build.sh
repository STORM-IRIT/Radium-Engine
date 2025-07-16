#!/bin/bash

set -uoe pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
COMMAND_COLOR='\033[0;90m'
NC='\033[0m'



if [ "$(uname)" == "Darwin" ]; then
    JOBS=$(sysctl -n hw.logicalcpu)
else
    JOBS=$(nproc)
fi

BUILD_TYPE=Release
BUILD_EXT=true
BUILD_RADIUM=true
USE_DOUBLE=OFF
UPDATE_VERSION=OFF
ENABLE_PCH=ON
INSTALL_DOC=OFF
ENABLE_EXAMPLE=ON
ENABLE_COVERAGE=OFF
ENABLE_TESTING=ON

VERBOSE=true
function eval_verbose {
    if [ "$VERBOSE" = true ]; then
        echo -e "${COMMAND_COLOR}$*${NC}"
    fi
    "$@"
}

# Help function
function usage {
    echo "Usage: $0 [OPTIONS]"
    echo
    echo "Options:"
    echo "  -h, --help              Show this help message and exit",
    echo "  -j, --jobs N            Use N jobs for parallel build"
    echo "                          (default: number of CPU cores, ${JOBS})"
    echo "  -c, --config TYPE       Build CONFIG using cmake convention"
    echo "                          (default: ${BUILD_TYPE})"
    echo "  --cc                    Set cc, default to system."
    echo "  --cxx                   Set cxx, default to system."
    echo "  -e, --build-ext BOOL    Build external (default: ${BUILD_EXT})"
    echo "  -r, --build-radium BOOL Build radium (default: ${BUILD_RADIUM})"
    echo "  -G, --generator GEN     Specify CMake generator (e.g., 'Ninja')"
    echo "  -o, --options OPTS      More cmake configure options."
    echo "            double quote if multiple OPTS, single quote if spaces"
    echo "            as in -o \"-Da -Ddir='a b'\""
    echo "  -B, --build-prefix DIR  Set BUILD_PREFIX (Radium-Engine/build),"
    echo "            then uses BUILD_PREFIX/CONFIG/{external|Radium-Engine}"
    echo "  -i, --install DIR       Install path PREFIX"
    echo "            (default: ../radium-install)"
    echo "            install in PREFIX/CONFIG/Radium-Engine|externals"
    echo "  --install-radium DIR     Install radium path (superseed prefix)"
    echo "  --install-external DIR   Install path prefix (superseed prefix)"
    echo "  --use-double ON/OFF      default: ${USE_DOUBLE}"
    echo "  --update-version ON/OFF  default: ${UPDATE_VERSION}"
    echo "  --enable-pch ON/OFF      default: ${ENABLE_PCH}"
    echo "  --install-doc ON/OFF     default: ${INSTALL_DOC}"
    echo "  --enable-example ON/OFF  default: ${ENABLE_EXAMPLE}"
    echo "  --enable-testing ON/OFF  default: ${ENABLE_TESTING}"
    echo "  --enable-coverage ON/OFF default: ${ENABLE_COVERAGE}"
}

if [[ $? -gt 0 ]]; then
    usage
    exit
fi

# Parse options, todo fix missing parameters
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            usage
            exit 0
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        -c|--config)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -e|--build-ext)
            BUILD_EXT=$2
            shift 2
            ;;
        -r|--build-radium)
            BUILD_RADIUM=$2
            shift 2
            ;;
        -G|--generator)
            CMAKE_GENERATOR="$2"
            shift 2
            ;;
        -o|--options)
            eval "CMAKE_EXTRA_ARGS=($2)"
            shift 2
            ;;
        -i|--install)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --install-external)
            EXTERNAL_INSTALL_DIR="$2"
            shift 2
            ;;
        --install-radium)
            RADIUM_INSTALL_DIR="$2"
            shift 2
            ;;
         -B|--build-prefix)
            BUILD_PREFIX="$2"
            shift 2
            ;;
         --cc)
            CC="$2"
            shift 2
            ;;
         --cxx)
             CXX="$2"
             shift 2
             ;;
         --use-double)
             USE_DOUBLE="$2"
             shift 2
             ;;
         --update-version)
             UPDATE_VERSION="$2"
             shift 2
             ;;
         --enable-pch)
             ENABLE_PCH="$2"
             shift 2
             ;;
         --install-doc)
             INSTALL_DOC="$2"
             shift 2
             ;;
         --enable-example)
             ENABLE_EXAMPLE="$2"
             shift 2
             ;;
         --enable-testing)
             ENABLE_TESTING="$2"
             shift 2
             ;;
         --enable-coverage)
             ENABLE_COVERAGE="$2"
             shift 2
             ;;
         --) shift; break ;;
         *) # Invalid option
             echo -e "${RED}Error: Invalid option [$1]${NC}\n"
             usage
             exit
             ;;
    esac
done


# Get SDK directory (directory where this script is located)
THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}/" )/../" && pwd )"

if [ -z ${BUILD_PREFIX+x} ]; then
    BUILD_ROOT="${THIS_DIR}/build"
else
    BUILD_ROOT="${BUILD_PREFIX}"
fi

# Define build and install directories
EXTERNAL_BUILD_DIR="${BUILD_ROOT}/${BUILD_TYPE}/external"
RADIUM_BUILD_DIR="${BUILD_ROOT}/${BUILD_TYPE}/Radium-Engine"

if [ -z ${INSTALL_PREFIX+x} ]; then
    INSTALL_PREFIX="${THIS_DIR}/../install/${BUILD_TYPE}"
fi

if [ -z ${RADIUM_INSTALL_DIR+x} ]; then
    RADIUM_INSTALL_DIR="${INSTALL_PREFIX}/Radium-Engine"
fi

if [ -z ${EXTERNAL_INSTALL_DIR+x} ]; then
    EXTERNAL_INSTALL_DIR="${INSTALL_PREFIX}/external"
fi

GENERATOR_ARG=""
if [ ! -z ${CMAKE_GENERATOR+x} ]; then
    GENERATOR_ARG="-G ${CMAKE_GENERATOR}"
fi

COMPILER_ARG=""
if [ ! -z ${CC+x} ]; then
    COMPILER_ARG="-DCMAKE_C_COMPILER=${CC}"
fi
if [ ! -z ${CCX+x} ]; then
    COMPILER_ARG="${COMPILER_ARG} -DCMAKE_C_COMPILER=${CXX}"
fi

if [ "${BUILD_EXT}" = true ]; then
    echo -e "\n${GREEN}Configure externals...${NC}"

    eval_verbose cmake -S "${THIS_DIR}/external" -B "${EXTERNAL_BUILD_DIR}" \
                 ${GENERATOR_ARG:+"${GENERATOR_ARG}"} \
                 ${COMPILER_ARG:+"${COMPILER_ARG}"} \
                 -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
                 -DCMAKE_EXECUTE_PROCESS_COMMAND_ECHO=NONE \
                 -DCMAKE_INSTALL_MESSAGE=LAZY \
                 -DCMAKE_INSTALL_PREFIX="${EXTERNAL_INSTALL_DIR}"

    echo -e "\n${GREEN}Configure externals done.${NC}"
    echo -e "\n${GREEN}Build externals...${NC}"

    eval_verbose cmake --build "${EXTERNAL_BUILD_DIR}" --parallel "${JOBS}" \
                 --config "${BUILD_TYPE}"
    echo -e "\n${GREEN}Build externals done.${NC}"
fi

if [ "${BUILD_RADIUM}" = true ]; then
    echo -e "\n${GREEN}Configure Radium Engine...${NC}"

    eval_verbose cmake -S "${THIS_DIR}/" -B "${RADIUM_BUILD_DIR}" \
                 -DCMAKE_EXECUTE_PROCESS_COMMAND_ECHO=NONE \
                 ${GENERATOR_ARG:+"${GENERATOR_ARG}"} \
                 ${COMPILER_ARG:+"${COMPILER_ARG}"} \
                 -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
                 -DCMAKE_INSTALL_PREFIX="${RADIUM_INSTALL_DIR}" \
                 -C "${EXTERNAL_INSTALL_DIR}/radium-options.cmake" \
                 ${CMAKE_EXTRA_ARGS:+"${CMAKE_EXTRA_ARGS[@]}"} \
                 -DRADIUM_USE_DOUBLE="${USE_DOUBLE}" \
                 -DRADIUM_UPDATE_VERSION="${UPDATE_VERSION}" \
                 -DRADIUM_ENABLE_PCH="${ENABLE_PCH}" \
                 -DRADIUM_INSTALL_DOC="${INSTALL_DOC}" \
                 -DRADIUM_ENABLE_EXAMPLES="${ENABLE_EXAMPLE}" \
                 -DRADIUM_ENABLE_TESTING="${ENABLE_TESTING}" \
                 -DRADIUM_ENABLE_COVERAGE="${ENABLE_COVERAGE}"

    echo -e "\n${GREEN}Configure Radium Engine done.${NC}"
    echo -e "\n${GREEN}Build Radium Engine...${NC}"

    eval_verbose cmake --build "${RADIUM_BUILD_DIR}" --parallel "${JOBS}" \
                 --config "${BUILD_TYPE}" --target install

    echo -e "\n${GREEN}Build Radium Engine done.${NC}"

fi
