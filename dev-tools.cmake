# Static analysis (clang-tidy)
option(CMake_RUN_CLANG_TIDY "Run clang-tidy with the compiler." ON)
if (CMake_RUN_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if (CLANG_TIDY_EXE)
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")

        # A) To all targets
        #        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE} -config-file ${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy)

        # B) Selective
        file(GLOB_RECURSE ALL_SOURCE_FILES src/*.cpp src/*.hpp)
        add_custom_target(
                clang-tidy
                COMMAND ${CLANG_TIDY_EXE}
                --warnings-as-errors="*"
                --config-file ${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy
                -p ${CMAKE_BINARY_DIR} # So that compile commands are found
                ${ALL_SOURCE_FILES}
        )
    else ()
        message(AUTHOR_WARNING "clang-tidy not found.")
    endif ()
else ()
    message(AUTHOR_WARNING "clang-tidy disabled.")
endif ()