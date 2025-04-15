//
// Created by Rene Windegger on 14/02/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class msgpack23_uint16 : public testing::TestWithParam<std::uint16_t> {
    };

    struct UInt16Struct {
        std::uint16_t uint16;

        template<class T>
        void pack(T &packer) const {
            packer(uint16);
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
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::pack(inserter, testIntStruct);
        auto [uint16] = msgpack23::unpack<UInt16Struct>(data);
        EXPECT_EQ(uint16, GetParam());
    }

    constexpr std::uint16_t uint16_numbers[] = {
        0,
        1,
        std::numeric_limits<std::int8_t>::max(),
        std::numeric_limits<std::int8_t>::max() - 1,
        42,
        0x81,
        std::numeric_limits<std::int16_t>::max(),
        std::numeric_limits<std::int16_t>::max() - 1,
        std::numeric_limits<std::uint16_t>::max(),
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, msgpack23_uint16, testing::ValuesIn(uint16_numbers));

    TEST(msgpack23, uint16Packing) {
        constexpr auto iterations = 200U;
        for (std::uint16_t i = 0U; i < iterations; ++i) {
            std::vector<std::byte> data{};
            auto const inserter = std::back_insert_iterator(data);
            msgpack23::Packer packer{inserter};
            auto const expected = static_cast<std::uint16_t>(
                i * (std::numeric_limits<std::uint16_t>::max() / iterations));
            packer(expected);
            msgpack23::Unpacker unpacker{data};
            std::uint16_t actual{};
            unpacker(actual);
            EXPECT_EQ(actual, expected);
        }
    }
}
