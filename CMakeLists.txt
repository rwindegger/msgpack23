cmake_minimum_required(VERSION 3.28)

project(msgpack23 LANGUAGES C CXX VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "Missing CMAKE_BUILD_TYPE!")
endif ()

include(GNUInstallDirs)

add_library(
        msgpack23
        INTERFACE
        include/msgpack23/msgpack23.h
)
add_library(msgpack23::msgpack23 ALIAS msgpack23)

target_compile_features(msgpack23 INTERFACE cxx_std_23)

target_include_directories(
        msgpack23
        INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(
        TARGETS msgpack23
        EXPORT msgpack23Targets
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

set(msgpack23_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/msgpack23")

install(
        EXPORT msgpack23Targets
        FILE msgpack23Targets.cmake
        NAMESPACE msgpack23::
        DESTINATION ${msgpack23_CMAKE_CONFIG_DESTINATION}
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/msgpack23ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/cmake/msgpack23Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/msgpack23Config.cmake
        INSTALL_DESTINATION ${msgpack23_CMAKE_CONFIG_DESTINATION}
)

install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/msgpack23ConfigVersion.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/msgpack23Config.cmake
        DESTINATION ${msgpack23_CMAKE_CONFIG_DESTINATION}
)

export(
        EXPORT msgpack23Targets
        FILE ${CMAKE_CURRENT_BINARY_DIR}/msgpack23Targets.cmake
        NAMESPACE msgpack23::
)
export(PACKAGE msgpack23)

add_subdirectory(tests)
