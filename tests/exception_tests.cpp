//
// Created by Rene Windegger on 22/02/2025.
//

#include <list>
#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    TEST(msgpack23, MapTooLargeTest) {
        GTEST_SKIP() << "Required map is to large for memory";
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        std::map<std::size_t, std::size_t> expected{};
        for (std::map<std::size_t, std::size_t>::size_type i = 0;
             i < static_cast<std::map<std::size_t, std::size_t>::size_type>(std::numeric_limits<std::uint32_t>::max()) +
             1; ++i) {
            expected.insert(std::make_pair(i, i));
        }
        EXPECT_THROW(packer(expected), std::length_error);
    }

    TEST(msgpack23, CollectionTooLargeTest) {
        GTEST_SKIP() << "Required collection is to large for memory";
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        std::vector<std::size_t> expected{};
        expected.resize(
            static_cast<std::vector<std::size_t>::size_type>(std::numeric_limits<std::uint32_t>::max()) + 1);
        EXPECT_THROW(packer(expected), std::length_error);
    }

    TEST(msgpack23, StringTooLargeTest) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        std::string expected{};
        expected.resize(static_cast<std::string::size_type>(std::numeric_limits<std::uint32_t>::max()) + 1);
        EXPECT_THROW(packer(expected), std::length_error);
    }

    TEST(msgpack23, VectorTooLargeTest) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        std::vector<std::uint8_t> expected{};
        expected.resize(
            static_cast<std::vector<std::uint8_t>::size_type>(std::numeric_limits<std::uint32_t>::max()) + 1);
        EXPECT_THROW(packer(expected), std::length_error);
    }

    TEST(msgpack23, WrongFormatForNullPtrTest) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        constexpr double expected{3.1415};
        packer(expected);
        std::nullptr_t actual;
        msgpack23::Unpacker<std::byte> unpacker{};
        EXPECT_THROW(unpacker(actual), std::logic_error);
    }

    TEST(msgpack23, WrongFormatForBoolTest) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        constexpr double expected{3.1415};
        packer(expected);
        bool actual;
        msgpack23::Unpacker<std::byte> unpacker{};
        EXPECT_THROW(unpacker(actual), std::logic_error);
    }

    TEST(msgpack23, WrongFormatForFloatTest) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        constexpr double expected{3.1415};
        packer(expected);
        float actual{};
        msgpack23::Unpacker unpacker{data};
        EXPECT_THROW(unpacker(actual), std::logic_error);
    }

    TEST(msgpack23, WrongFormatForDoubleTest) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        constexpr float expected{3.1415F};
        packer(expected);
        double actual{};
        msgpack23::Unpacker unpacker{data};
        EXPECT_THROW(unpacker(actual), std::logic_error);
    }

    TEST(msgpack23, WrongFormatForTimeStampTest) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        constexpr std::uint64_t expected{std::numeric_limits<std::uint64_t>::max()};
        packer(expected);
        std::chrono::system_clock::time_point actual{};
        msgpack23::Unpacker unpacker{data};
        EXPECT_THROW(unpacker(actual), std::logic_error);
    }

    TEST(msgpack23, WrongFormatForByteArrayTest) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        constexpr std::uint64_t expected{std::numeric_limits<std::uint64_t>::max()};
        packer(expected);
        std::vector<std::uint8_t> actual{};
        msgpack23::Unpacker unpacker{data};
        EXPECT_THROW(unpacker(actual), std::logic_error);
    }

    TEST(msgpack23, StringNotEnoughDataTest) {
        std::vector<std::byte> const expected_data{
            static_cast<std::byte>(0b10100000 | 4),
            static_cast<std::byte>('t'),
            static_cast<std::byte>('e')
        };
        msgpack23::Unpacker unpacker{expected_data};
        std::string actual{};
        EXPECT_THROW(unpacker(actual), std::out_of_range);
    }

    TEST(msgpack23, ByteArrayNotEnoughDataTest) {
        std::vector<std::byte> const expected_data{
            static_cast<std::byte>(0xc4),
            static_cast<std::byte>(4),
            static_cast<std::byte>(1),
            static_cast<std::byte>(2),
        };
        msgpack23::Unpacker unpacker{expected_data};
        std::vector<std::uint8_t> actual{};
        EXPECT_THROW(unpacker(actual), std::out_of_range);
    }

    TEST(msgpack23, ArrayNotEnoughDataTest) {
        std::vector<std::byte> const expected_data{
            static_cast<std::byte>(0b10010000 | 3),
            static_cast<std::byte>(0b10100000 | 3),
            static_cast<std::byte>('o'),
            static_cast<std::byte>('n'),
            static_cast<std::byte>('e'),
            static_cast<std::byte>(0b10100000 | 3),
            static_cast<std::byte>('t'),
            static_cast<std::byte>('w'),
            static_cast<std::byte>('o')
        };
        msgpack23::Unpacker unpacker{expected_data};
        std::list<std::string> actual{};
        EXPECT_THROW(unpacker(actual), std::out_of_range);
    }

    TEST(msgpack23, MapNotEnoughDataTest) {
        std::vector<std::byte> const expected_data{
            static_cast<std::byte>(0b10000000 | 2),
            static_cast<std::byte>(0),
            static_cast<std::byte>(0b10100000 | 4),
            static_cast<std::byte>('z'),
            static_cast<std::byte>('e'),
            static_cast<std::byte>('r'),
            static_cast<std::byte>('o'),
            static_cast<std::byte>(1),
        };
        msgpack23::Unpacker unpacker{expected_data};
        std::map<std::uint8_t, std::string> actual{};
        EXPECT_THROW(unpacker(actual), std::out_of_range);
    }

    TEST(msgpack23, IntegralNotEnoughDataTest) {
        std::vector<std::byte> const expected_data{
            static_cast<std::byte>(0xcf),
            static_cast<std::byte>(0),
            static_cast<std::byte>('z'),
            static_cast<std::byte>('e'),
            static_cast<std::byte>('r'),
            static_cast<std::byte>('o'),
            static_cast<std::byte>(1),
        };
        msgpack23::Unpacker unpacker{expected_data};
        std::uint64_t actual{};
        EXPECT_THROW(unpacker(actual), std::out_of_range);
    }
}
