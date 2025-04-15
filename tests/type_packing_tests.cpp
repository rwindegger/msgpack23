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
            std::vector<std::byte> data{};
            auto const inserter = std::back_insert_iterator(data);
            msgpack23::Packer packer{inserter};
            packer(expected);
            msgpack23::Unpacker unpacker{data};
            float actual{};
            unpacker(actual);
            EXPECT_EQ(expected, actual);
        }

        for (auto i = -5; i < 5; ++i) {
            float expected = static_cast<float>(i) * std::pow(0.1f, std::abs(i));
            std::vector<std::byte> data{};
            auto const inserter = std::back_insert_iterator(data);
            msgpack23::Packer packer{inserter};
            packer(expected);
            msgpack23::Unpacker unpacker{data};
            float actual{};
            unpacker(actual);
            EXPECT_EQ(expected, actual);
        }
    }

    TEST(msgpack23, DoubleTypePacking) {
        for (auto i = -5; i < 5; ++i) {
            double expected = 5.0 + static_cast<double>(i) * 23456.78 / 3.14;
            std::vector<std::byte> data{};
            auto const inserter = std::back_insert_iterator(data);
            msgpack23::Packer packer{inserter};
            packer(expected);
            msgpack23::Unpacker unpacker{data};
            double actual{};
            unpacker(actual);
            EXPECT_EQ(expected, actual);
        }

        for (auto i = -5; i < 5; ++i) {
            double expected = static_cast<double>(i) * std::pow(0.1, std::abs(i));
            std::vector<std::byte> data{};
            auto const inserter = std::back_insert_iterator(data);
            msgpack23::Packer packer{inserter};
            packer(expected);
            msgpack23::Unpacker unpacker{data};
            double actual{};
            unpacker(actual);
            EXPECT_EQ(expected, actual);
        }
    }

    TEST(msgpack23, NilTypePacking) {
        constexpr std::nullptr_t expected{};
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        packer(expected);
        ASSERT_EQ(data, std::vector<std::byte>{static_cast<std::byte>(0xc0)});
    }

    TEST(msgpack23, BooleanTypePacking) { {
            std::vector<std::byte> data{};
            auto const inserter = std::back_insert_iterator(data);
            msgpack23::Packer packer{inserter};
            packer(false);
            ASSERT_EQ(data, std::vector<std::byte>{static_cast<std::byte>(0xc2)});
            msgpack23::Unpacker unpacker{data};
            bool actual{};
            unpacker(actual);
            EXPECT_EQ(false, actual);
        } {
            std::vector<std::byte> data{};
            auto const inserter = std::back_insert_iterator(data);
            msgpack23::Packer packer{inserter};
            packer(true);
            ASSERT_EQ(data, std::vector<std::byte>{static_cast<std::byte>(0xc3)});
            msgpack23::Unpacker unpacker{data};
            bool actual{};
            unpacker(actual);
            EXPECT_EQ(true, actual);
        }
    }

    TEST(msgpack23, SystemClockTypePacking) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        auto const expected = std::chrono::system_clock::now();
        packer(expected);
        msgpack23::Unpacker unpacker{data};
        decltype(std::chrono::system_clock::now()) actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, SteadyClockTypePacking) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        auto const expected = std::chrono::steady_clock::now();
        packer(expected);
        msgpack23::Unpacker unpacker{data};
        decltype(std::chrono::steady_clock::now()) actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, HighResolutionClockTypePacking) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        auto const expected = std::chrono::high_resolution_clock::now();
        packer(expected);
        msgpack23::Unpacker unpacker{data};
        decltype(std::chrono::high_resolution_clock::now()) actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, StringTypePacking) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        std::string const expected = "test";
        std::vector<std::byte> const expected_data{
            static_cast<std::byte>(0b10100000 | 4),
            static_cast<std::byte>('t'),
            static_cast<std::byte>('e'),
            static_cast<std::byte>('s'),
            static_cast<std::byte>('t')
        };
        packer(expected);
        EXPECT_EQ(data, expected_data);
        msgpack23::Unpacker unpacker{data};
        std::string actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, ByteArrayTypePacking) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        std::vector<std::uint8_t> const expected{1, 2, 3, 4};
        std::vector<std::byte> const expected_data{
            static_cast<std::byte>(0xc4),
            static_cast<std::byte>(4),
            static_cast<std::byte>(1),
            static_cast<std::byte>(2),
            static_cast<std::byte>(3),
            static_cast<std::byte>(4)
        };
        packer(expected);
        EXPECT_EQ(data, expected_data);
        msgpack23::Unpacker unpacker{data};
        std::vector<std::uint8_t> actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, ArrayTypePacking) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
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
        packer(expected);
        EXPECT_EQ(data, expected_data);
        msgpack23::Unpacker unpacker{data};
        std::list<std::string> actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, StdArrayTypePacking) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
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
        packer(expected);
        EXPECT_EQ(data, expected_data);
        msgpack23::Unpacker unpacker{data};
        std::array<std::string, 3> actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, MapTypePacking) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        std::map<std::uint8_t, std::string> const expected{std::make_pair(0, "zero"), std::make_pair(1, "one")};
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
        packer(expected);
        EXPECT_EQ(data, expected_data);
        msgpack23::Unpacker unpacker{data};
        std::map<std::uint8_t, std::string> actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, UnorderedMapTypePacking) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        std::unordered_map<std::uint8_t, std::string> const expected{
            std::make_pair(0, "zero"), std::make_pair(1, "one")
        };
        packer(expected);
        msgpack23::Unpacker unpacker{data};
        std::unordered_map<std::uint8_t, std::string> actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, TupleTypePacking) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        std::tuple<std::uint8_t, std::string> const expected{0, "zero"};
        packer(expected);
        msgpack23::Unpacker unpacker{data};
        std::tuple<std::uint8_t, std::string> actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    TEST(msgpack23, VariantTypePacking) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        std::variant<std::uint8_t, std::string> const expected{"Hello, Variant!"};
        packer(expected);
        msgpack23::Unpacker unpacker{data};
        std::variant<std::uint8_t, std::string> actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }
}
