cmake_minimum_required(VERSION 3.28)

find_package(GTest REQUIRED)

add_executable(
        msgpack23_tests
        int8_test.cpp
        int16_test.cpp
        int32_test.cpp
        int64_test.cpp
        main.cpp
        object_packing_tests.cpp
        uint8_test.cpp
        uint16_test.cpp
        uint32_test.cpp
        uint64_test.cpp
        type_packing_tests.cpp
)

target_link_libraries(
        msgpack23_tests
        PRIVATE
        msgpack23::msgpack23
        GTest::gtest
        GTest::gtest_main
)
add_test(msgpack23_tests msgpack23_tests)

set_target_properties(msgpack23_tests PROPERTIES CXX_STANDARD 23)
target_compile_features(msgpack23_tests PUBLIC cxx_std_23)
