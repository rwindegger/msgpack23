//
// Created by Rene Windegger on 28/10/2025.
//
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>
#include <map>
#include <msgpack23/msgpack23.h>

int main() {
    std::map<std::string, int> const original {{"apple", 1}, {"banana", 2}};

    std::vector<unsigned char> data{};
    msgpack23::Packer packer { std::back_inserter(data) };
    packer(original);

    std::map<std::string, int> unpacked;
    msgpack23::Unpacker unpacker { data };
    unpacker(unpacked);

    for (auto const& [key, value] : unpacked) {
        std::cout << key << ": " << value << '\n';
    }
}
