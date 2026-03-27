cmake_minimum_required(VERSION 3.26)

include(FetchContent)
include(GenerateExportHeader)

FetchContent_Declare(libos
    GIT_REPOSITORY https://github.com/JannikBulow/libos
    GIT_TAG master
)
FetchContent_MakeAvailable(libos)

set(SOURCES
    src/allocator/arena.cpp

    src/core/opcodes.cpp

    src/executor/function.cpp

    src/module/const_pool.cpp
    src/module/function.cpp
    src/module/module.cpp

    src/util/string_pool.cpp

    src/init.cpp
    src/vm.cpp
)

set(HEADERS
    include/BibbleVM/allocator/arena.h

    include/BibbleVM/core/opcodes.h

    include/BibbleVM/executor/instruction.h
    include/BibbleVM/executor/interpreter.h

    include/BibbleVM/module/function.h
    include/BibbleVM/module/module.h

    include/BibbleVM/util/list.h
    include/BibbleVM/util/string_pool.h

    include/BibbleVM/init.h
    include/BibbleVM/vm.h
    include/BibbleVM/executor/function.h
    include/BibbleVM/core/value.h
    include/BibbleVM/module/const_pool.h
    include/BibbleVM/module/instruction.h
    include/BibbleVM/executor/module.h
)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_library(BibbleVM-framework-shared SHARED ${SOURCES} ${HEADERS})
add_library(BibbleVM-framework-static STATIC ${SOURCES} ${HEADERS})

generate_export_header(BibbleVM-framework-shared
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/BibbleVM/api.h
    EXPORT_MACRO_NAME BIBBLEVM_EXPORT
    NO_EXPORT_MACRO_NAME BIBBLEVM_NO_EXPORT
    STATIC_DEFINE BIBBLEVM_STATIC_DEFINE
)

target_compile_definitions(BibbleVM-framework-static PUBLIC BIBBLEVM_STATIC_DEFINE)

target_include_directories(BibbleVM-framework-shared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_include_directories(BibbleVM-framework-static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(BibbleVM-framework-shared PUBLIC cxx_std_20)
target_compile_features(BibbleVM-framework-static PUBLIC cxx_std_20)

target_link_libraries(BibbleVM-framework-shared libos-static)
target_link_libraries(BibbleVM-framework-static libos-static)