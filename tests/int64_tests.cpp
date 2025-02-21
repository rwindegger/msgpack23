//
// Created by Rene Windegger on 14/02/2025.
//

#include <cstddef>
#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class msgpack23_int64 : public testing::TestWithParam<std::int64_t> {
    };

    struct Int64Struct {
        std::int64_t int64;

        template<class T>
        std::vector<std::byte> pack(T &packer) const {
            return packer(int64);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(int64);
        }
    };

    TEST_P(msgpack23_int64, intTest) {
        Int64Struct const testIntStruct{
            GetParam()
        };
        auto const data = msgpack23::pack(testIntStruct);
        auto [actual] = msgpack23::unpack<Int64Struct>(data);
        EXPECT_EQ(actual, GetParam());
    }

    constexpr std::int64_t int64_numbers[] = {
        0,
        1,
        std::numeric_limits<std::int8_t>::min(),
        std::numeric_limits<std::int8_t>::min() + 1,
        std::numeric_limits<std::int8_t>::max(),
        std::numeric_limits<std::int8_t>::max() - 1,
        42,
        -42,
        std::numeric_limits<std::int16_t>::max(),
        std::numeric_limits<std::int16_t>::min(),
        std::numeric_limits<std::int16_t>::max() - 1,
        std::numeric_limits<std::int16_t>::min() + 1,
        std::numeric_limits<std::int32_t>::max(),
        std::numeric_limits<std::int32_t>::min(),
        std::numeric_limits<std::int32_t>::max() - 1,
        std::numeric_limits<std::int32_t>::min() + 1,
        std::numeric_limits<std::int64_t>::max(),
        std::numeric_limits<std::int64_t>::min()
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, msgpack23_int64, testing::ValuesIn(int64_numbers));

    TEST(msgpack23, int64Packing) {
        for (std::int64_t i = -10; i < 10; ++i) {
            msgpack23::Packer packer{};
            auto const expected = static_cast<std::int64_t>(i * (std::numeric_limits<std::int64_t>::max() / 10));
            auto data = packer(expected);
            msgpack23::Unpacker unpacker{data};
            std::int64_t actual{};
            unpacker(actual);
            EXPECT_EQ(actual, expected);
        }
    }
}
