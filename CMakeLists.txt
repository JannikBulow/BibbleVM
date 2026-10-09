cmake_minimum_required(VERSION 3.26)
project(BibbleVM VERSION 0.1.0 LANGUAGES C CXX ASM_NASM)

include(CMakePackageConfigHelpers)

enable_language(ASM_NASM)

add_subdirectory(framework)
add_subdirectory(main)
add_subdirectory(native_interface)
add_subdirectory(tests)

configure_package_config_file(
    cmake/BibbleVMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/BibbleVMConfig.cmake
    INSTALL_DESTINATION lib/cmake/BibbleVM
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/BibbleVMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/BibbleVMConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/BibbleVMConfigVersion.cmake
    DESTINATION lib/cmake/BibbleVM
)
