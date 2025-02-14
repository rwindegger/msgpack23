//
// Created by Rene Windegger on 14/02/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class UInt64Test : public testing::TestWithParam<uint64_t> {
    };

    struct UInt64Struct {
        uint64_t uint64;

        template<class T>
        std::vector<std::byte> pack(T &packer) const {
            return packer(uint64);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(uint64);
        }
    };

    TEST_P(UInt64Test, intTest) {
        UInt64Struct const testIntStruct{
            GetParam()
        };
        auto const data = msgpack23::pack(testIntStruct);
        auto [uint64] = msgpack23::unpack<UInt64Struct>(data);
        EXPECT_EQ(uint64, GetParam());
    }

    constexpr uint64_t uint64_numbers[] = {
        0,
        1,
        std::numeric_limits<int8_t>::max(),
        std::numeric_limits<int8_t>::max() - 1,
        42,
        std::numeric_limits<int16_t>::max(),
        std::numeric_limits<int16_t>::max() - 1,
        std::numeric_limits<int32_t>::max(),
        std::numeric_limits<int32_t>::max() - 1,
        std::numeric_limits<int64_t>::max(),
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, UInt64Test, testing::ValuesIn(uint64_numbers));
}
