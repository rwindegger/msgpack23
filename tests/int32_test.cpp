//
// Created by Rene Windegger on 14/02/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class Int32Test : public testing::TestWithParam<int32_t> {
    };

    struct Int32Struct {
        int32_t int32;

        template<class T>
        std::vector<std::byte> pack(T &packer) const {
            return packer(int32);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(int32);
        }
    };

    TEST_P(Int32Test, intTest) {
        Int32Struct const testIntStruct{
            GetParam()
        };
        auto const data = msgpack23::pack(testIntStruct);
        auto [int32] = msgpack23::unpack<Int32Struct>(data);
        EXPECT_EQ(int32, GetParam());
    }

    constexpr int32_t int32_numbers[] = {
        0,
        1,
        std::numeric_limits<int8_t>::min(),
        std::numeric_limits<int8_t>::min() + 1,
        std::numeric_limits<int8_t>::max(),
        std::numeric_limits<int8_t>::max() - 1,
        42,
        -42,
        std::numeric_limits<int16_t>::max(),
        std::numeric_limits<int16_t>::min(),
        std::numeric_limits<int16_t>::max() - 1,
        std::numeric_limits<int16_t>::min() + 1,
        std::numeric_limits<int32_t>::max(),
        std::numeric_limits<int32_t>::min()
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, Int32Test, testing::ValuesIn(int32_numbers));
}
