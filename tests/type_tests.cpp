//
// Created by Rene Windegger on 14/02/2025.
//

#include <cmath>
#include <gtest/gtest.h>
#include <list>
#include <map>
#include <msgpack23/msgpack23.h>
#include <unordered_map>

namespace {
    TEST(msgpack23, FloatTypePacking) {
        for (auto i = -5; i < 5; ++i) {
            float expected = 5.0f + static_cast<float>(i) * 23456.78f / 3.14f;
            msgpack23::Packer packer{};
            auto data = packer(expected);
            msgpack23::Unpacker unpacker{data.data(), data.size()};
            float actual{};
            unpacker(actual);
            EXPECT_EQ(expected, actual);
        }

        for (auto i = -5; i < 5; ++i) {
            float expected = static_cast<float>(i) * std::pow(0.1f, std::abs(i));
            msgpack23::Packer packer{};
            auto data = packer(expected);
            msgpack23::Unpacker unpacker{data.data(), data.size()};
            float actual{};
            unpacker(actual);
            EXPECT_EQ(expected, actual);
        }
    }

    TEST(msgpack23, DoubleTypePacking) {
        for (auto i = -5; i < 5; ++i) {
            double expected = 5.0 + static_cast<double>(i) * 23456.78 / 3.14;
            msgpack23::Packer packer{};
            auto data = packer(expected);
            msgpack23::Unpacker unpacker{data.data(), data.size()};
            double actual{};
            unpacker(actual);
            EXPECT_EQ(expected, actual);
        }

        for (auto i = -5; i < 5; ++i) {
            double expected = static_cast<double>(i) * std::pow(0.1, std::abs(i));
            msgpack23::Packer packer{};
            auto data = packer(expected);
            msgpack23::Unpacker unpacker{data.data(), data.size()};
            double actual{};
            unpacker(actual);
            EXPECT_EQ(expected, actual);
        }
    }

    TEST(msgpack23, NilTypePacking) {
        constexpr std::nullptr_t expected{};
        msgpack23::Packer packer{};
        auto const data = packer(expected);
        ASSERT_EQ(data, std::vector<std::byte>{static_cast<std::byte>(0xc0)});
    }

    TEST(msgpack23, BooleanTypePacking) { {
            msgpack23::Packer packer{};
            auto const data = packer(false);
            ASSERT_EQ(data, std::vector<std::byte>{static_cast<std::byte>(0xc2)});
            msgpack23::Unpacker unpacker{data.data(), data.size()};
            bool actual{};
            unpacker(actual);
            EXPECT_EQ(false, actual);
        } {
            msgpack23::Packer packer{};
            auto const data = packer(true);
            ASSERT_EQ(data, std::vector<std::byte>{static_cast<std::byte>(0xc3)});
            msgpack23::Unpacker unpacker{data.data(), data.size()};
            bool actual{};
            unpacker(actual);
            EXPECT_EQ(true, actual);
        }
    }

    TEST(msgpack23, ChronoTypePacking) {
        msgpack23::Packer packer{};
        auto const expected = std::chrono::system_clock::now();
        auto const data = packer(expected);
        msgpack23::Unpacker unpacker{data.data(), data.size()};
        decltype(std::chrono::system_clock::now()) actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, StringTypePacking) {
        msgpack23::Packer packer{};
        std::string const expected = "test";
        std::vector<std::byte> const expected_data{
            static_cast<std::byte>(0b10100000 | 4),
            static_cast<std::byte>('t'),
            static_cast<std::byte>('e'),
            static_cast<std::byte>('s'),
            static_cast<std::byte>('t')
        };
        auto const data = packer(expected);
        EXPECT_EQ(data, expected_data);
        msgpack23::Unpacker unpacker{data.data(), data.size()};
        std::string actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, ByteArrayTypePacking) {
        msgpack23::Packer packer{};
        std::vector<uint8_t> const expected{1, 2, 3, 4};
        std::vector<std::byte> const expected_data{
            static_cast<std::byte>(0xc4),
            static_cast<std::byte>(4),
            static_cast<std::byte>(1),
            static_cast<std::byte>(2),
            static_cast<std::byte>(3),
            static_cast<std::byte>(4)
        };
        auto const data = packer(expected);
        EXPECT_EQ(data, expected_data);
        msgpack23::Unpacker unpacker{data.data(), data.size()};
        std::vector<uint8_t> actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, ArrayTypePacking) {
        msgpack23::Packer packer{};
        std::list<std::string> const expected{"one", "two", "three"};
        std::vector<std::byte> const expected_data{
            static_cast<std::byte>(0b10010000 | 3),
            static_cast<std::byte>(0b10100000 | 3),
            static_cast<std::byte>('o'),
            static_cast<std::byte>('n'),
            static_cast<std::byte>('e'),
            static_cast<std::byte>(0b10100000 | 3),
            static_cast<std::byte>('t'),
            static_cast<std::byte>('w'),
            static_cast<std::byte>('o'),
            static_cast<std::byte>(0b10100000 | 5),
            static_cast<std::byte>('t'),
            static_cast<std::byte>('h'),
            static_cast<std::byte>('r'),
            static_cast<std::byte>('e'),
            static_cast<std::byte>('e'),
        };
        auto const data = packer(expected);
        EXPECT_EQ(data, expected_data);
        msgpack23::Unpacker unpacker{data.data(), data.size()};
        std::list<std::string> actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, StdArrayTypePacking) {
        msgpack23::Packer packer{};
        std::array<std::string, 3> const expected{"one", "two", "three"};
        std::vector<std::byte> const expected_data{
            static_cast<std::byte>(0b10010000 | 3),
            static_cast<std::byte>(0b10100000 | 3),
            static_cast<std::byte>('o'),
            static_cast<std::byte>('n'),
            static_cast<std::byte>('e'),
            static_cast<std::byte>(0b10100000 | 3),
            static_cast<std::byte>('t'),
            static_cast<std::byte>('w'),
            static_cast<std::byte>('o'),
            static_cast<std::byte>(0b10100000 | 5),
            static_cast<std::byte>('t'),
            static_cast<std::byte>('h'),
            static_cast<std::byte>('r'),
            static_cast<std::byte>('e'),
            static_cast<std::byte>('e'),
        };
        auto const data = packer(expected);
        EXPECT_EQ(data, expected_data);
        msgpack23::Unpacker unpacker{data.data(), data.size()};
        std::array<std::string, 3> actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, MapTypePacking) {
        msgpack23::Packer packer{};
        std::map<uint8_t, std::string> const expected{std::make_pair(0, "zero"), std::make_pair(1, "one")};
        std::vector<std::byte> const expected_data{
            static_cast<std::byte>(0b10000000 | 2),
            static_cast<std::byte>(0),
            static_cast<std::byte>(0b10100000 | 4),
            static_cast<std::byte>('z'),
            static_cast<std::byte>('e'),
            static_cast<std::byte>('r'),
            static_cast<std::byte>('o'),
            static_cast<std::byte>(1),
            static_cast<std::byte>(0b10100000 | 3),
            static_cast<std::byte>('o'),
            static_cast<std::byte>('n'),
            static_cast<std::byte>('e'),
        };
        auto const data = packer(expected);
        EXPECT_EQ(data, expected_data);
        msgpack23::Unpacker unpacker{data.data(), data.size()};
        std::map<uint8_t, std::string> actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, UnorderedMapTypePacking) {
        msgpack23::Packer packer{};
        std::unordered_map<uint8_t, std::string> const expected{std::make_pair(0, "zero"), std::make_pair(1, "one")};
        auto const data = packer(expected);
        msgpack23::Unpacker unpacker{data.data(), data.size()};
        std::unordered_map<uint8_t, std::string> actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }
}
