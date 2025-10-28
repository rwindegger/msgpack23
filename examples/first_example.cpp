//
// Created by Rene Windegger on 28/10/2025.
//
#include <iostream>
#include <cstddef>
#include <iterator>
#include <utility>
#include <vector>
#include <map>
#include <msgpack23/msgpack23.h>

class converting_insert_iterator final {
public:
    using difference_type = std::ptrdiff_t;

    constexpr explicit converting_insert_iterator(std::back_insert_iterator<std::vector<unsigned char>> &&iterator) : store_{std::move(iterator)} {}

    constexpr converting_insert_iterator &operator=(const std::byte &value) {
        store_ = std::to_underlying(value);
        return *this;
    }

    constexpr converting_insert_iterator &operator=(std::byte &&value) {
        store_ = std::to_underlying(value);
        return *this;
    }

    [[nodiscard]] constexpr converting_insert_iterator &operator*() {
        return *this;
    }

    constexpr converting_insert_iterator &operator++() {
        return *this;
    }

    constexpr converting_insert_iterator operator++(int) {
        return *this;
    }
private:
    std::back_insert_iterator<std::vector<unsigned char>> store_;
};

int main() {
    std::map<std::string, int> const original {{"apple", 1}, {"banana", 2}};

    std::vector<unsigned char> data{};
    msgpack23::Packer packer { converting_insert_iterator{ std::back_inserter(data) } };
    packer(original);

    std::map<std::string, int> unpacked;
    msgpack23::Unpacker unpacker {
        std::span<std::byte>{
            reinterpret_cast<std::byte *>(data.data()),
            data.size()
        }
    };
    unpacker(unpacked);

    for (auto const& [key, value] : unpacked) {
        std::cout << key << ": " << value << '\n';
    }
}
