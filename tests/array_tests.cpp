//
// Created by Rene Windegger on 16/02/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class msgpack23_array : public testing::TestWithParam<std::size_t> {
    };

    TEST_P(msgpack23_array, arrayTest) {
        std::vector<std::size_t> expected{};
        for (std::size_t i = 0; i < GetParam(); ++i) {
            expected.emplace_back(i);
        }
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        packer(expected);
        msgpack23::Unpacker unpacker{data};
        std::vector<std::size_t> actual{};
        unpacker(actual);
        EXPECT_EQ(actual, expected);
    }

    TEST_P(msgpack23_array, binaryTest) {
        std::vector<std::uint8_t> expected{};
        for (std::size_t i = 0; i < GetParam(); ++i) {
            expected.emplace_back(42U);
        }
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        packer(expected);
        msgpack23::Unpacker unpacker{data};
        std::vector<std::uint8_t> actual{};
        unpacker(actual);
        EXPECT_EQ(actual, expected);
    }

    constexpr std::size_t array_sizes[] = {
        1,
        std::numeric_limits<std::int8_t>::max(),
        42,
        std::numeric_limits<std::uint16_t>::max() - 1,
        std::numeric_limits<std::uint16_t>::max() + 1,
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, msgpack23_array, testing::ValuesIn(array_sizes));
}
