//
// Created by Rene Windegger on 14/02/2025.
//

#include <cstddef>
#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class msgpack23_int8 : public testing::TestWithParam<std::int8_t> {
    };

    struct Int8Struct {
        std::int8_t int8;

        template<class T>
        void pack(T &packer) const {
            packer(int8);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(int8);
        }
    };

    TEST_P(msgpack23_int8, int8Test) {
        Int8Struct const testIntStruct{
            GetParam()
        };
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::pack(inserter, testIntStruct);
        auto [actual] = msgpack23::unpack<std::byte, Int8Struct>(data);
        EXPECT_EQ(actual, GetParam());
    }

    constexpr std::int8_t int8_numbers[] = {
        0, 1, std::numeric_limits<std::int8_t>::min(), std::numeric_limits<std::int8_t>::max(), 42, -42
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, msgpack23_int8, testing::ValuesIn(int8_numbers));

    TEST(msgpack23, int8Packing) {
        for (std::int8_t i = -10; i < 10; ++i) {
            std::vector<std::byte> data{};
            auto const inserter = std::back_insert_iterator(data);
            msgpack23::Packer packer{inserter};
            auto const expected = static_cast<std::int8_t>(i * (std::numeric_limits<std::int8_t>::max() / 10));
            packer(expected);
            msgpack23::Unpacker unpacker {data};
            std::int8_t actual {};
            unpacker(actual);
            EXPECT_EQ(actual, expected);
        }
    }
}
