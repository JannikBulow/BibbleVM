cmake_minimum_required(VERSION 3.26)

include(FetchContent)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

set(SOURCES
    gc_test.cpp
    test_main.cpp
)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES})

add_executable(BibbleVM-tests ${SOURCES})

target_include_directories(BibbleVM-tests PUBLIC include)

target_compile_features(BibbleVM-tests PUBLIC cxx_std_20)

target_link_libraries(BibbleVM-tests PRIVATE BibbleVM-framework-shared gtest_main)

add_custom_command(TARGET BibbleVM-tests PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:BibbleVM-framework-shared>
    $<TARGET_FILE_DIR:BibbleVM-tests>
)

if (MSVC)
    add_custom_command(TARGET BibbleVM-tests PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_PDB_FILE:BibbleVM-framework-shared>
        $<TARGET_FILE_DIR:BibbleVM-tests>
    )
endif()

add_test(NAME BibbleVM-tests COMMAND BibbleVM-tests)