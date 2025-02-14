//
// Created by Rene Windegger on 14/02/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class Int8Test : public testing::TestWithParam<int8_t> {
    };

    struct Int8Struct {
        int8_t int8;

        template<class T>
        std::vector<std::byte> pack(T &packer) const {
            return packer(int8);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(int8);
        }
    };

    TEST_P(Int8Test, intTest) {
        Int8Struct const testIntStruct{
            GetParam()
        };
        auto const data = msgpack23::pack(testIntStruct);
        auto [int8] = msgpack23::unpack<Int8Struct>(data);
        EXPECT_EQ(int8, GetParam());
    }

    constexpr int8_t int8_numbers[] = {
        0, 1, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max(), 42, -42
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, Int8Test, testing::ValuesIn(int8_numbers));
}
