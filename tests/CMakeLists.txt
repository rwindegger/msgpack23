cmake_minimum_required(VERSION 3.28)

include(GoogleTest)

find_package(GTest REQUIRED)

add_executable(
        msgpack23_tests
        array_tests.cpp
        chrono_tests.cpp
        int8_tests.cpp
        int16_tests.cpp
        int32_tests.cpp
        int64_tests.cpp
        main.cpp
        map_tests.cpp
        object_packing_tests.cpp
        string_tests.cpp
        type_packing_tests.cpp
        uint8_tests.cpp
        uint16_tests.cpp
        uint32_tests.cpp
        uint64_tests.cpp
)

target_link_libraries(
        msgpack23_tests
        PRIVATE
        msgpack23::msgpack23
        GTest::gtest
        GTest::gtest_main
)
gtest_discover_tests(msgpack23_tests)

set_target_properties(msgpack23_tests PROPERTIES CXX_STANDARD 23)
target_compile_features(msgpack23_tests PUBLIC cxx_std_23)
