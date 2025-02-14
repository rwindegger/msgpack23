//
// Created by Rene Windegger on 14/02/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class Int16Test : public testing::TestWithParam<int16_t> {
    };

    struct Int16Struct {
        int16_t int16;

        template<class T>
        std::vector<std::byte> pack(T &packer) const {
            return packer(int16);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(int16);
        }
    };

    TEST_P(Int16Test, intTest) {
        Int16Struct const testIntStruct{
            GetParam()
        };
        auto const data = msgpack23::pack(testIntStruct);
        auto [int16] = msgpack23::unpack<Int16Struct>(data);
        EXPECT_EQ(int16, GetParam());
    }

    constexpr int16_t int16_numbers[] = {
        0,
        1,
        std::numeric_limits<int8_t>::min(),
        std::numeric_limits<int8_t>::min() + 1,
        std::numeric_limits<int8_t>::max(),
        std::numeric_limits<int8_t>::max() - 1,
        42,
        -42,
        std::numeric_limits<int16_t>::max(),
        std::numeric_limits<int16_t>::min()
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, Int16Test, testing::ValuesIn(int16_numbers));
}
