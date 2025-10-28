cmake_minimum_required(VERSION 3.28)

add_executable(
        msgpack23_first_example
        first_example.cpp
)

target_link_libraries(
        msgpack23_first_example
        PRIVATE
        msgpack23::msgpack23
)

set_target_properties(msgpack23_first_example PROPERTIES CXX_STANDARD 23)
target_compile_features(msgpack23_first_example PUBLIC cxx_std_23)

add_executable(
        msgpack23_second_example
        second_example.cpp
)

target_link_libraries(
        msgpack23_second_example
        PRIVATE
        msgpack23::msgpack23
)

set_target_properties(msgpack23_second_example PROPERTIES CXX_STANDARD 23)
target_compile_features(msgpack23_second_example PUBLIC cxx_std_23)
