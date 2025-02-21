//
// Created by Rene Windegger on 14/02/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class msgpack23_uint64 : public testing::TestWithParam<std::uint64_t> {
    };

    struct UInt64Struct {
        std::uint64_t uint64;

        template<class T>
        std::vector<std::byte> pack(T &packer) const {
            return packer(uint64);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(uint64);
        }
    };

    TEST_P(msgpack23_uint64, intTest) {
        UInt64Struct const testIntStruct{
            GetParam()
        };
        auto const data = msgpack23::pack(testIntStruct);
        auto [uint64] = msgpack23::unpack<UInt64Struct>(data);
        EXPECT_EQ(uint64, GetParam());
    }

    constexpr std::uint64_t uint64_numbers[] = {
        0,
        1,
        std::numeric_limits<std::int8_t>::max(),
        std::numeric_limits<std::int8_t>::max() - 1,
        42,
        0x81,
        std::numeric_limits<std::int16_t>::max(),
        std::numeric_limits<std::int16_t>::max() - 1,
        std::numeric_limits<std::int32_t>::max(),
        std::numeric_limits<std::int32_t>::max() - 1,
        std::numeric_limits<std::int64_t>::max(),
        std::numeric_limits<std::int64_t>::max() - 1,
        std::numeric_limits<std::uint64_t>::max(),
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, msgpack23_uint64, testing::ValuesIn(uint64_numbers));

    TEST(msgpack23, uint64Packing) {
        constexpr auto iterations = 200U;
        for (std::uint64_t i = 0; i < iterations; ++i) {
            msgpack23::Packer packer{};
            auto const expected = static_cast<std::uint64_t>(
                i * (std::numeric_limits<std::uint64_t>::max() / iterations));
            auto data = packer(expected);
            msgpack23::Unpacker unpacker{data.data(), data.size()};
            std::uint64_t actual{};
            unpacker(actual);
            EXPECT_EQ(actual, expected);
        }
    }
}
