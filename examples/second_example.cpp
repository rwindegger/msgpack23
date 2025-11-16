//
// Created by Rene Windegger on 28/10/2025.
//
#include <iostream>
#include <iterator>
#include <vector>
#include <msgpack23/msgpack23.h>

struct MyData {
    std::int64_t my_integer;
    std::string my_string;

    template<typename T>
    void pack(T &packer) const {
        packer(my_integer, my_string);
    }

    template<typename T>
    void unpack(T &unpacker) {
        unpacker(my_integer, my_string);
    }
};

int main() {
    MyData const original {42, "Hello"};

    std::vector<unsigned char> data{};
    msgpack23::pack(std::back_inserter(data), original);

    const auto [my_integer, my_string] = msgpack23::unpack<MyData>(data);

    std::cout << my_integer << ' ' << my_string << '\n';
}
