cmake_minimum_required(VERSION 3.26)

include(FetchContent)
include(GenerateExportHeader)

FetchContent_Declare(BibbleBytecode
    GIT_REPOSITORY https://github.com/JannikBulow/BibbleBytecode
    GIT_TAG master
)
FetchContent_MakeAvailable(BibbleBytecode)

FetchContent_Declare(BibbleOpcodes
    GIT_REPOSITORY https://github.com/JannikBulow/BibbleOpcodes
    GIT_TAG master
)
FetchContent_MakeAvailable(BibbleOpcodes)

FetchContent_Declare(libos
    GIT_REPOSITORY https://github.com/JannikBulow/libos
    GIT_TAG master
)
FetchContent_MakeAvailable(libos)

set(ASMJIT_STATIC ON)
FetchContent_Declare(asmjit
    GIT_REPOSITORY https://github.com/asmjit/asmjit.git
    GIT_TAG master
)
FetchContent_MakeAvailable(asmjit)

FetchContent_Declare(utf8cpp
    GIT_REPOSITORY https://github.com/nemtrif/utfcpp
    GIT_TAG v4.0.9
)
FetchContent_MakeAvailable(utf8cpp)

FetchContent_Declare(frozen
    GIT_REPOSITORY https://github.com/serge-sans-paille/frozen
    GIT_TAG "1.2.0"
)
FetchContent_MakeAvailable(frozen)

set(SUPPORTED_ARCH off)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|amd64")
    set(ARCH "x64")

    if(WIN32)
        set(ABI "win64")
    else()
        set(ABI "sysv")
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(ARCH "aarch64")
    set(ABI "generic")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "riscv64|RISCV64")
    set(ARCH "riscv64")
    set(ABI "generic")
else()
    message(FATAL_ERROR "Unsupported architecture")
endif()

set(ASM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/asm/${ARCH}_${ABI}")

set(ASM_SOURCES
    ${ASM_DIR}/enter.asm
)

add_library(BibbleVM-assemblies OBJECT ${ASM_SOURCES})

set_property(TARGET BibbleVM-assemblies PROPERTY POSITION_INDEPENDENT_CODE ON)

set_target_properties(BibbleVM-assemblies PROPERTIES LINKER_LANGUAGE CXX)

set(SOURCES
    src/allocator/arena.cpp
    src/allocator/code_alloc.cpp

    src/compiler/jit/compiler.cpp

    src/compiler/aot_trap.cpp
    src/compiler/compiler.cpp
    src/compiler/trampoline.cpp

    src/core/executor/const_pool.cpp
    src/core/executor/function.cpp
    src/core/executor/interpreter.cpp
    src/core/executor/module.cpp
    src/core/executor/scheduler.cpp
    src/core/executor/stack.cpp
    src/core/executor/task.cpp

    src/core/gc/memory_manager.cpp
    src/core/gc/nursery.cpp
    src/core/gc/old_gen_heap.cpp

    src/core/module/instruction.cpp

    src/core/oop/class.cpp
    src/core/oop/future.cpp
    src/core/oop/string.cpp
    src/core/oop/type.cpp

    src/core/error.cpp
    src/core/value.cpp

    src/linker/intrinsics.cpp
    src/linker/linker.cpp

    src/native/plugin_manager.cpp
    src/native/interface.cpp
    src/native/trampoline.cpp

    src/util/string_pool.cpp

    src/config.cpp
    src/init.cpp
    src/vm.cpp
)

set(HEADERS
    include/BibbleVM/allocator/arena.h
    include/BibbleVM/allocator/code_alloc.h

    include/BibbleVM/compiler/jit/compiler.h

    include/BibbleVM/compiler/abi.h
    include/BibbleVM/compiler/aot_trap.h
    include/BibbleVM/compiler/code.h
    include/BibbleVM/compiler/compilation_unit.h
    include/BibbleVM/compiler/compiler.h
    include/BibbleVM/compiler/trampoline.h

    include/BibbleVM/core/executor/const_pool.h
    include/BibbleVM/core/executor/function.h
    include/BibbleVM/core/executor/instruction.h
    include/BibbleVM/core/executor/interpreter.h
    include/BibbleVM/core/executor/module.h
    include/BibbleVM/core/executor/scheduler.h
    include/BibbleVM/core/executor/scheduler_message.h
    include/BibbleVM/core/executor/stack.h
    include/BibbleVM/core/executor/task.h

    include/BibbleVM/core/gc/large_object_heap.h
    include/BibbleVM/core/gc/memory_manager.h
    include/BibbleVM/core/gc/nursery.h
    include/BibbleVM/core/gc/old_gen_heap.h

    include/BibbleVM/core/module/instruction.h
    include/BibbleVM/core/module/module.h

    include/BibbleVM/core/oop/array_view.h
    include/BibbleVM/core/oop/class.h
    include/BibbleVM/core/oop/field.h
    include/BibbleVM/core/oop/method.h
    include/BibbleVM/core/oop/object.h
    include/BibbleVM/core/oop/string.h
    include/BibbleVM/core/oop/type.h
    include/BibbleVM/core/oop/visit_children.h

    include/BibbleVM/core/error.h
    include/BibbleVM/core/opcodes.h
    include/BibbleVM/core/value.h

    include/BibbleVM/linker/intrinsics.h
    include/BibbleVM/linker/linker.h
    include/BibbleVM/linker/module.h

    include/BibbleVM/native/interface.h
    include/BibbleVM/native/plugin_manager.h
    include/BibbleVM/native/trampoline.h

    include/BibbleVM/util/safe_math.h
    include/BibbleVM/util/string_pool.h
    include/BibbleVM/util/time_manager.h

    include/BibbleVM/_compatibility.h
    include/BibbleVM/_debug.h
    include/BibbleVM/config.h
    include/BibbleVM/init.h
    include/BibbleVM/vm.h
)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_library(BibbleVM-framework-shared SHARED ${SOURCES} ${HEADERS})
add_library(BibbleVM-framework-static STATIC ${SOURCES} ${HEADERS})

target_sources(BibbleVM-framework-shared PRIVATE
    $<TARGET_OBJECTS:BibbleVM-assemblies>
)

target_sources(BibbleVM-framework-static PRIVATE
    $<TARGET_OBJECTS:BibbleVM-assemblies>
)

generate_export_header(BibbleVM-framework-shared
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/BibbleVM/api.h
    EXPORT_MACRO_NAME BIBBLEVM_EXPORT
    NO_EXPORT_MACRO_NAME BIBBLEVM_NO_EXPORT
    STATIC_DEFINE BIBBLEVM_STATIC_DEFINE
)

set_target_properties(BibbleVM-framework-shared PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)

target_compile_definitions(BibbleVM-framework-static PUBLIC BIBBLEVM_STATIC_DEFINE)

target_compile_definitions(BibbleVM-framework-shared PRIVATE $<$<CONFIG:Debug>:BIBBLEVM_ENABLE_DEBUG>)
target_compile_definitions(BibbleVM-framework-static PRIVATE $<$<CONFIG:Debug>:BIBBLEVM_ENABLE_DEBUG>)

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

target_link_libraries(BibbleVM-framework-shared BibbleVM-native-interface BibbleBytecode-shared BibbleOpcodes libos-static asmjit::asmjit utf8cpp frozen)
target_link_libraries(BibbleVM-framework-static BibbleVM-native-interface BibbleBytecode-static BibbleOpcodes libos-static asmjit::asmjit utf8cpp frozen)

set_target_properties(BibbleVM-framework-shared PROPERTIES OUTPUT_NAME "bibblevm")
set_target_properties(BibbleVM-framework-static PROPERTIES OUTPUT_NAME "bibblevm-aio")
