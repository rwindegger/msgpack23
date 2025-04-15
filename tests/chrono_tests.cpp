//
// Created by Rene Windegger on 21/02/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    class msgpack23_time_point : public testing::TestWithParam<std::chrono::system_clock::time_point> {
    };

    TEST_P(msgpack23_time_point, int8Test) {
        std::vector<std::byte> data{};
        auto const inserter = std::back_insert_iterator(data);
        msgpack23::Packer packer{inserter};
        auto const expected = GetParam();
        packer(expected);
        msgpack23::Unpacker unpacker{data};
        std::chrono::system_clock::time_point actual{};
        unpacker(actual);
        EXPECT_EQ(expected, actual);
    }

    std::chrono::system_clock::time_point const time_points[] = {
        std::chrono::system_clock::now(),
        std::chrono::system_clock::time_point{},
        std::chrono::system_clock::time_point{} - std::chrono::days(200),
        std::chrono::time_point_cast<std::chrono::system_clock::duration>(std::chrono::system_clock::time_point{} - std::chrono::nanoseconds(200)),
    };
    INSTANTIATE_TEST_SUITE_P(SomeValuesTest, msgpack23_time_point, testing::ValuesIn(time_points));
}
