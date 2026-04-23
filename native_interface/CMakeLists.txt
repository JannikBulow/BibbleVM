cmake_minimum_required(VERSION 3.26)

set(HEADERS
    include/BibbleInterface.h
)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${HEADERS})

add_library(BibbleVM-native-interface INTERFACE ${HEADERS})

target_include_directories(BibbleVM-native-interface INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)