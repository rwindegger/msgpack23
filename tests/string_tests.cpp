//
// Created by Rene Windegger on 16/02/2025.
//

#include <gtest/gtest.h>
#include <map>
#include <msgpack23/msgpack23.h>

namespace {
    class msgpack23_string : public testing::TestWithParam<std::size_t> {
    };

    TEST_P(msgpack23_string, arrayTest) {
        std::string expected{};
        for (std::size_t i = 0; i < GetParam(); ++i) {
            expected.append("*");
        }
        msgpack23::Packer packer{};
        auto data = packer(expected);
        msgpack23::Unpacker unpacker{data.data(), data.size()};
        std::string actual{};
        unpacker(actual);
        EXPECT_EQ(actual, expected);
    }

    constexpr std::size_t string_sizes[] = {
        1,
        std::numeric_limits<std::int8_t>::max(),
        42,
        std::numeric_limits<std::uint16_t>::max() - 1,
        std::numeric_limits<std::uint16_t>::max() + 1,
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, msgpack23_string, testing::ValuesIn(string_sizes));
}
