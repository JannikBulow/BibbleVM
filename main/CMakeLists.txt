cmake_minimum_required(VERSION 3.26)

set(SOURCES
    src/main.cpp
)

set(HEADERS

)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_executable(BibbleVM-main ${SOURCES} ${HEADERS})

target_include_directories(BibbleVM-main PUBLIC include)

target_compile_features(BibbleVM-main PUBLIC cxx_std_20)

target_link_libraries(BibbleVM-main PRIVATE BibbleVM-framework-shared)