//
// Created by Rene Windegger on 16/02/2025.
//

#include <gtest/gtest.h>
#include <map>
#include <msgpack23/msgpack23.h>

namespace {
    class msgpack23_map : public testing::TestWithParam<std::size_t> {
    };

    TEST_P(msgpack23_map, arrayTest) {
        std::map<std::size_t, std::size_t> expected{};
        for (std::size_t i = 0; i < GetParam(); ++i) {
            expected.insert_or_assign(i, i);
        }
        msgpack23::Packer packer{};
        auto data = packer(expected);
        msgpack23::Unpacker unpacker{data};
        std::map<std::size_t, std::size_t> actual{};
        unpacker(actual);
        EXPECT_EQ(actual, expected);
    }

    constexpr std::size_t map_sizes[] = {
        1,
        std::numeric_limits<std::int8_t>::max(),
        42,
        std::numeric_limits<std::uint16_t>::max() - 1,
        std::numeric_limits<std::uint16_t>::max() + 1,
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, msgpack23_map, testing::ValuesIn(map_sizes));
}
