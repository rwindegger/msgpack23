//
// Created by Rene Windegger on 14/02/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class msgpack23_uint8 : public testing::TestWithParam<std::uint8_t> {
    };

    struct UInt8Struct {
        std::uint8_t uint8;

        template<class T>
        std::vector<std::byte> pack(T &packer) const {
            return packer(uint8);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(uint8);
        }
    };

    TEST_P(msgpack23_uint8, intTest) {
        UInt8Struct const testIntStruct{
            GetParam()
        };
        auto const data = msgpack23::pack(testIntStruct);
        auto [int8] = msgpack23::unpack<UInt8Struct>(data);
        EXPECT_EQ(int8, GetParam());
    }

    constexpr std::uint8_t uint8_numbers[] = {
        0,
        1,
        std::numeric_limits<std::uint8_t>::min(),
        std::numeric_limits<std::uint8_t>::max(),
        42,
        std::numeric_limits<std::int8_t>::max()
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, msgpack23_uint8, testing::ValuesIn(uint8_numbers));

    TEST(msgpack23, uint8Packing) {
        constexpr auto iterations = 20U;
        for (std::uint8_t i = 0; i < iterations; ++i) {
            msgpack23::Packer packer{};
            auto const expected = static_cast<std::uint8_t>(
                i * (std::numeric_limits<std::uint8_t>::max() / iterations));
            auto data = packer(expected);
            msgpack23::Unpacker unpacker{data.data(), data.size()};
            std::uint8_t actual{};
            unpacker(actual);
            EXPECT_EQ(actual, expected);
        }
    }
}
