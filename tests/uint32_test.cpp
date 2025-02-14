//
// Created by Rene Windegger on 14/02/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class UInt32Test : public testing::TestWithParam<uint32_t> {
    };

    struct UInt32Struct {
        uint32_t uint32;

        template<class T>
        std::vector<std::byte> pack(T &packer) const {
            return packer(uint32);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(uint32);
        }
    };

    TEST_P(UInt32Test, intTest) {
        UInt32Struct const testIntStruct{
            GetParam()
        };
        auto const data = msgpack23::pack(testIntStruct);
        auto [int32] = msgpack23::unpack<UInt32Struct>(data);
        EXPECT_EQ(int32, GetParam());
    }

    constexpr uint32_t uint32_numbers[] = {
        0,
        1,
        std::numeric_limits<int8_t>::max(),
        std::numeric_limits<int8_t>::max() - 1,
        42,
        std::numeric_limits<int16_t>::max(),
        std::numeric_limits<int16_t>::max() - 1,
        std::numeric_limits<int32_t>::max(),
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, UInt32Test, testing::ValuesIn(uint32_numbers));
}
