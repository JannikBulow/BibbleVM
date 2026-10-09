cmake_minimum_required(VERSION 3.26)

set(HEADERS
    include/BibbleInterface.h
)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${HEADERS})

add_library(BibbleVM-native-interface INTERFACE ${HEADERS})

target_include_directories(BibbleVM-native-interface INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(BibbleVM-native-interface INTERFACE cxx_std_20)

install(
    TARGETS BibbleVM-native-interface
    EXPORT BibbleVMTargets
)

install(
    DIRECTORY include/
    DESTINATION include
)