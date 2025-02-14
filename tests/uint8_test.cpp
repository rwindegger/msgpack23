//
// Created by Rene Windegger on 14/02/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class UInt8Test : public testing::TestWithParam<uint8_t> {
    };

    struct UInt8Struct {
        uint8_t uint8;

        template<class T>
        std::vector<std::byte> pack(T &packer) const {
            return packer(uint8);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(uint8);
        }
    };

    TEST_P(UInt8Test, intTest) {
        UInt8Struct const testIntStruct{
            GetParam()
        };
        auto const data = msgpack23::pack(testIntStruct);
        auto [int8] = msgpack23::unpack<UInt8Struct>(data);
        EXPECT_EQ(int8, GetParam());
    }

    constexpr uint8_t uint8_numbers[] = {
        0,
        1,
        std::numeric_limits<uint8_t>::min(),
        std::numeric_limits<uint8_t>::max(),
        42,
        std::numeric_limits<int8_t>::max()
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, UInt8Test, testing::ValuesIn(uint8_numbers));
}
