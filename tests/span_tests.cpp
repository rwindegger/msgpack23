//
// Created by Neara Software on 28/02/2026.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class msgpack23_span : public testing::TestWithParam<std::size_t> {
    };

    TEST_P(msgpack23_span, binarySpanRoundTrip) {
        std::vector<std::uint8_t> expected{};
        for (std::size_t i = 0; i < GetParam(); ++i) {
            expected.emplace_back(static_cast<std::uint8_t>(i));
        }
        std::vector<std::uint8_t> data{};
        msgpack23::Packer packer{std::back_insert_iterator(data)};
        packer(expected);

        msgpack23::Unpacker unpacker{data};
        std::span<const std::uint8_t> actual{};
        unpacker(actual);

        ASSERT_EQ(actual.size(), expected.size());
        EXPECT_TRUE(std::equal(actual.begin(), actual.end(), expected.begin()));
    }

    TEST_P(msgpack23_span, binarySpanIsZeroCopy) {
        std::vector<std::uint8_t> expected(GetParam(), 0x42);
        std::vector<std::uint8_t> data{};
        msgpack23::Packer packer{std::back_insert_iterator(data)};
        packer(expected);

        msgpack23::Unpacker unpacker{data};
        std::span<const std::uint8_t> actual{};
        unpacker(actual);

        // Span must point into the packed buffer, not a separate allocation
        EXPECT_GE(actual.data(), data.data());
        EXPECT_LE(actual.data() + actual.size(), data.data() + data.size());
    }

    constexpr std::size_t span_sizes[] = {
        1,
        std::numeric_limits<std::uint8_t>::max() - 1,   // bin8 near-max
        std::numeric_limits<std::uint8_t>::max() + 1,   // bin16 boundary
        std::numeric_limits<std::uint16_t>::max() - 1,  // bin16 near-max
        std::numeric_limits<std::uint16_t>::max() + 1,  // bin32 boundary
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, msgpack23_span, testing::ValuesIn(span_sizes));
}
