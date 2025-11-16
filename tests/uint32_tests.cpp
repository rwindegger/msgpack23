//
// Created by Rene Windegger on 14/02/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class msgpack23_uint32 : public testing::TestWithParam<std::uint32_t> {
    };

    struct UInt32Struct {
        std::uint32_t uint32;

        template<class T>
        void pack(T &packer) const {
            packer(uint32);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(uint32);
        }
    };

    TEST_P(msgpack23_uint32, intTest) {
        UInt32Struct const testIntStruct{
            GetParam()
        };
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::pack(inserter, testIntStruct);
        auto [int32] = msgpack23::unpack<UInt32Struct>(data);
        EXPECT_EQ(int32, GetParam());
    }

    constexpr std::uint32_t uint32_numbers[] = {
        0,
        1,
        std::numeric_limits<std::int8_t>::max(),
        std::numeric_limits<std::int8_t>::max() - 1,
        42,
        0x81,
        std::numeric_limits<std::int16_t>::max(),
        std::numeric_limits<std::int16_t>::max() - 1,
        std::numeric_limits<std::int32_t>::max(),
        std::numeric_limits<std::uint32_t>::max(),
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, msgpack23_uint32, testing::ValuesIn(uint32_numbers));

    TEST(msgpack23, uint32Packing) {
        constexpr auto iterations = 200U;
        for (std::uint32_t i = 0; i < iterations; ++i) {
            std::vector<std::byte> data{};
            auto const inserter = std::back_insert_iterator(data);
            msgpack23::Packer packer{inserter};
            auto const expected = static_cast<std::uint32_t>(
                i * (std::numeric_limits<std::uint32_t>::max() / iterations));
            packer(expected);
            msgpack23::Unpacker unpacker{data};
            std::uint32_t actual{};
            unpacker(actual);
            EXPECT_EQ(actual, expected);
        }
    }
}
