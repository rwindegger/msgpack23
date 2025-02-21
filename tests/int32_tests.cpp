//
// Created by Rene Windegger on 14/02/2025.
//

#include <cstddef>
#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class msgpack23_int32 : public testing::TestWithParam<std::int32_t> {
    };

    struct Int32Struct {
        std::int32_t int32;

        template<class T>
        std::vector<std::byte> pack(T &packer) const {
            return packer(int32);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(int32);
        }
    };

    TEST_P(msgpack23_int32, intTest) {
        Int32Struct const testIntStruct{
            GetParam()
        };
        auto const data = msgpack23::pack(testIntStruct);
        auto [actual] = msgpack23::unpack<Int32Struct>(data);
        EXPECT_EQ(actual, GetParam());
    }

    constexpr std::int32_t int32_numbers[] = {
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
        std::numeric_limits<std::int32_t>::min()
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, msgpack23_int32, testing::ValuesIn(int32_numbers));

    TEST(msgpack23, int32Packing) {
        for (std::int32_t i = -10; i < 10; ++i) {
            msgpack23::Packer packer{};
            auto const expected = static_cast<std::int32_t>(i * (std::numeric_limits<std::int32_t>::max() / 10));
            auto data = packer(expected);
            msgpack23::Unpacker unpacker{data};
            std::int32_t actual{};
            unpacker(actual);
            EXPECT_EQ(actual, expected);
        }
    }
}
