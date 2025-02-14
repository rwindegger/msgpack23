//
// Created by Rene Windegger on 14/02/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class msgpack23_uint16 : public testing::TestWithParam<uint16_t> {
    };

    struct UInt16Struct {
        uint16_t uint16;

        template<class T>
        std::vector<std::byte> pack(T &packer) const {
            return packer(uint16);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(uint16);
        }
    };

    TEST_P(msgpack23_uint16, intTest) {
        UInt16Struct const testIntStruct{
            GetParam()
        };
        auto const data = msgpack23::pack(testIntStruct);
        auto [uint16] = msgpack23::unpack<UInt16Struct>(data);
        EXPECT_EQ(uint16, GetParam());
    }

    constexpr uint16_t uint16_numbers[] = {
        0,
        1,
        std::numeric_limits<int8_t>::max(),
        std::numeric_limits<int8_t>::max() - 1,
        42,
        std::numeric_limits<int16_t>::max(),
        std::numeric_limits<int16_t>::max() - 1,
        std::numeric_limits<uint16_t>::max(),
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, msgpack23_uint16, testing::ValuesIn(uint16_numbers));
}
