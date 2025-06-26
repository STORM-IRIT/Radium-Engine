if(CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang" OR CMAKE_CXX_COMPILER_ID MATCHES
                                                     "(Apple)?[Cc]lang"
)
    set(LLVM_COVERAGE ON)
else()
    set(GCOV_COVERAGE ON)
endif()

if(DEFINED LLVM_COVERAGE)
    find_program(LLVM_PROFDATA_PATH llvm-profdata)
    find_program(LLVM_COV_PATH llvm-cov)
    if(NOT LLVM_COV_PATH)
        message(FATAL_ERROR "llvm-cov not found! Aborting.")
    endif()
    if(NOT LLVM_PROFDATA_PATH)
        message(FATAL_ERROR "llvm-profdata not found! Aborting.")
    endif()
    set(PROF_DIR ${CMAKE_BINARY_DIR}/llvm-cov/prof)
    set(REPORT_DIR ${CMAKE_BINARY_DIR}/llvm-cov/report)
    set(TEST_ENV_VAR "LLVM_PROFILE_FILE=${PROF_DIR}/coverage-%m-%p.profraw")
    set(TEST_ENV_VAR ${TEST_ENV_VAR} PARENT_SCOPE)
else()
    include(CodeCoverage)
endif()

function(radium_setup_coverage_targets)
    if(DEFINED LLVM_COVERAGE)
        add_custom_target(
            llvm_coverage
            COMMAND find ${PROF_DIR} -name \"*.profraw\" -delete
            COMMAND rm -rf ${REPORT_DIR}
            COMMAND ${CMAKE_CTEST_COMMAND} -L unittests
            COMMAND ${LLVM_PROFDATA_PATH} merge -sparse -o ${PROF_DIR}/unittests.profdata `find
                    ${PROF_DIR} -name \"*.profraw\"`
        )
    endif()
    # prevent flags on catch2 and examples, add to specific targets
    include(ListTargets)
    get_all_targets(TARGETS)
    foreach(TARGET ${TARGETS})
        if(NOT ${TARGET} MATCHES ".*stream.*")
            message(STATUS "add coverage flags to ${TARGET}")
            if(DEFINED LLVM_COVERAGE)
                target_compile_options(
                    ${TARGET} PRIVATE -fprofile-instr-generate -fcoverage-mapping
                )
                target_link_options(${TARGET} PRIVATE -fprofile-instr-generate -fcoverage-mapping)
                if(NOT ${TARGET} MATCHES "unittests")
                    list(APPEND LLVM_OBJS "--object"
                         "$<PATH:RELATIVE_PATH,$<TARGET_FILE:${TARGET}>,${CMAKE_BINARY_DIR}>"
                    )
                endif()

                add_dependencies(llvm_coverage ${TARGET})
            else()
                append_coverage_compiler_flags_to_target(${TARGET})
            endif()
        endif()
    endforeach()
    #
    if(DEFINED LLVM_COVERAGE)
        set(LLVM_COV
            ${LLVM_COV_PATH}
            show
            $<TARGET_FILE:unittests>
            ${LLVM_OBJS}
            -instr-profile=${PROF_DIR}/unittests.profdata
            -format=html
            -output-dir=${REPORT_DIR}
            -show-line-counts-or-regions
            -show-directory-coverage
            -Xdemangler
            c++filt
            -Xdemangler
            -n
            ${CMAKE_CURRENT_SOURCE_DIR}
        )
        add_custom_command(TARGET llvm_coverage POST_BUILD COMMAND ${LLVM_COV})
    else()
        # fix error on github's lcov
        foreach(LABEL unittests integration)
            setup_target_for_coverage_lcov(
                NAME
                lcov_${LABEL}
                EXECUTABLE
                ${CMAKE_CTEST_COMMAND}
                -L
                ${LABEL}
                --output-on-failure
                -C
                ${CMAKE_BUILD_TYPE}
                -j
                $ENV{CMAKE_BUILD_PARALLEL_LEVEL}
                BASE_DIRECTORY
                "/"
                DEPENDENCIES
                RadiumLibs
                ${LABEL}
                EXCLUDE
                "${CMAKE_BINARY_DIR}/_deps/*"
                "${CMAKE_BINARY_DIR}/*_autogen/*"
                "/*external*/"
                "/usr/*"
            )

            # Fastcov is not supported with gcov llvm: disabling for MacOS Source:
            # https://github.com/RPGillespie6/fastcov/issues/36
            if(UNIX AND NOT APPLE AND FASTCOV_PATH)
                setup_target_for_coverage_fastcov(
                    NAME
                    fastcov_${LABEL}
                    EXECUTABLE
                    ${CMAKE_CTEST_COMMAND}
                    -L
                    ${LABEL}
                    --output-on-failure
                    -C
                    ${CMAKE_BUILD_TYPE}
                    -j
                    $ENV{CMAKE_BUILD_PARALLEL_LEVEL}
                    DEPENDENCIES
                    RadiumLibs
                    ${LABEL}
                    BASE_DIRECTORY
                    "/"
                    EXCLUDE
                    "_deps"
                    "_autogen"
                    "external"
                    "/usr"
                )
            endif()
        endforeach()
    endif()
endfunction()
