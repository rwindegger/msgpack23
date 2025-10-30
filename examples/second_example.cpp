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
    msgpack23::pack(converting_insert_iterator{ std::back_inserter(data) }, original);

    const auto [my_integer, my_string] = msgpack23::unpack<MyData>(std::span{
        reinterpret_cast<std::byte *>(data.data()),
        data.size()
    });

    std::cout << my_integer << ' ' << my_string << '\n';
}
