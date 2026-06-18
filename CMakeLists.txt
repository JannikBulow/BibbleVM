cmake_minimum_required(VERSION 3.26)
project(BibbleVM LANGUAGES C CXX ASM_NASM)

enable_language(ASM_NASM)

add_subdirectory(framework)
add_subdirectory(main)
add_subdirectory(native_interface)
add_subdirectory(tests)