//
// Created by Rene Windegger on 14/02/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class msgpack23_int64 : public testing::TestWithParam<int64_t> {
    };

    struct Int64Struct {
        int64_t int64;

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
        auto [int64] = msgpack23::unpack<Int64Struct>(data);
        EXPECT_EQ(int64, GetParam());
    }

    constexpr int64_t int64_numbers[] = {
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
        std::numeric_limits<int32_t>::min(),
        std::numeric_limits<int32_t>::max() - 1,
        std::numeric_limits<int32_t>::min() + 1,
        std::numeric_limits<int64_t>::max(),
        std::numeric_limits<int64_t>::min()
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, msgpack23_int64, testing::ValuesIn(int64_numbers));
}
