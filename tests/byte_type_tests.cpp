//
// Created by Rene Windegger on 31/10/2025.
//

#include <array>
#include <map>
#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {
    enum class TestEnum : std::uint8_t {
        First = 0,
        Second = 1,
        Third = 2,
    };

    struct NestedStruct {
        std::array<std::string, 3> names;
        std::vector<std::string> values;
        std::tuple<int, std::string> tuple;

        template<class T>
        void pack(T &packer) const {
            packer(names, values, tuple);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(names, values, tuple);
        }
    };

    struct TestStruct {
        std::int64_t int64;
        std::uint32_t uint32;
        float float32;
        double double64;
        std::string string;
        std::vector<std::uint8_t> data;
        std::map<std::string, std::string> map;
        TestEnum testEnum;
        std::chrono::time_point<std::chrono::system_clock> time_point;
        NestedStruct nestedStruct;

        template<class T>
        void pack(T &packer) const {
            packer(int64, uint32, float32, double64, string, data, map, testEnum, time_point, nestedStruct);
        }

        template<class T>
        void unpack(T &unpacker) {
            unpacker(int64, uint32, float32, double64, string, data, map, testEnum, time_point, nestedStruct);
        }
    };

    TEST(msgpack23, UnsignedCharNestedObjectPacking) {
        std::map<std::string, std::string> map;
        map.insert(std::pair<std::string, std::string>("first", "hello"));
        map.insert(std::pair<std::string, std::string>("second", "world"));
        std::vector<std::string> values;
        values.emplace_back("first");
        values.emplace_back("second");
        values.emplace_back("third");
        values.emplace_back("fourth");
        TestStruct const test{
            -57128,
            42,
            250.42f,
            3.1415926535,
            "hello world",
            {0x15, 0x16, 42},
            std::move(map),
            TestEnum::First,
            std::chrono::system_clock::now(),
            {
                {"John", "Bjarne", "Rene"},
                std::move(values),
                {42, "The answer to everything"},
            }
        };
        std::vector<unsigned char> data{};
        auto inserter = std::back_insert_iterator(data);
        msgpack23::pack(inserter, test);
        auto const obj = msgpack23::unpack<unsigned char, TestStruct>(data);

        EXPECT_EQ(obj.int64, test.int64);
        EXPECT_EQ(obj.uint32, test.uint32);
        EXPECT_EQ(obj.float32, test.float32);
        EXPECT_EQ(obj.double64, test.double64);
        EXPECT_EQ(obj.string, test.string);
        EXPECT_EQ(obj.data, test.data);
        EXPECT_EQ(obj.map, test.map);
        EXPECT_EQ(obj.testEnum, test.testEnum);
        EXPECT_EQ(obj.time_point, test.time_point);
        EXPECT_EQ(obj.nestedStruct.names, test.nestedStruct.names);
        EXPECT_EQ(obj.nestedStruct.values, test.nestedStruct.values);
        EXPECT_EQ(obj.nestedStruct.tuple, test.nestedStruct.tuple);
    }

    TEST(msgpack23, uint8_tNestedObjectPacking) {
        std::map<std::string, std::string> map;
        map.insert(std::pair<std::string, std::string>("first", "hello"));
        map.insert(std::pair<std::string, std::string>("second", "world"));
        std::vector<std::string> values;
        values.emplace_back("first");
        values.emplace_back("second");
        values.emplace_back("third");
        values.emplace_back("fourth");
        TestStruct const test{
            -57128,
            42,
            250.42f,
            3.1415926535,
            "hello world",
            {0x15, 0x16, 42},
            std::move(map),
            TestEnum::First,
            std::chrono::system_clock::now(),
            {
                    {"John", "Bjarne", "Rene"},
                    std::move(values),
                    {42, "The answer to everything"},
                }
        };
        std::vector<std::uint8_t> data{};
        auto inserter = std::back_insert_iterator(data);
        msgpack23::pack(inserter, test);
        auto const obj = msgpack23::unpack<std::uint8_t, TestStruct>(data);

        EXPECT_EQ(obj.int64, test.int64);
        EXPECT_EQ(obj.uint32, test.uint32);
        EXPECT_EQ(obj.float32, test.float32);
        EXPECT_EQ(obj.double64, test.double64);
        EXPECT_EQ(obj.string, test.string);
        EXPECT_EQ(obj.data, test.data);
        EXPECT_EQ(obj.map, test.map);
        EXPECT_EQ(obj.testEnum, test.testEnum);
        EXPECT_EQ(obj.time_point, test.time_point);
        EXPECT_EQ(obj.nestedStruct.names, test.nestedStruct.names);
        EXPECT_EQ(obj.nestedStruct.values, test.nestedStruct.values);
        EXPECT_EQ(obj.nestedStruct.tuple, test.nestedStruct.tuple);
    }

    TEST(msgpack23, CharNestedObjectPacking) {
        std::map<std::string, std::string> map;
        map.insert(std::pair<std::string, std::string>("first", "hello"));
        map.insert(std::pair<std::string, std::string>("second", "world"));
        std::vector<std::string> values;
        values.emplace_back("first");
        values.emplace_back("second");
        values.emplace_back("third");
        values.emplace_back("fourth");
        TestStruct const test{
            -57128,
            42,
            250.42f,
            3.1415926535,
            "hello world",
            {0x15, 0x16, 42},
            std::move(map),
            TestEnum::First,
            std::chrono::system_clock::now(),
            {
                    {"John", "Bjarne", "Rene"},
                    std::move(values),
                    {42, "The answer to everything"},
                }
        };
        std::vector<char> data{};
        auto inserter = std::back_insert_iterator(data);
        msgpack23::pack(inserter, test);
        auto const obj = msgpack23::unpack<char, TestStruct>(data);

        EXPECT_EQ(obj.int64, test.int64);
        EXPECT_EQ(obj.uint32, test.uint32);
        EXPECT_EQ(obj.float32, test.float32);
        EXPECT_EQ(obj.double64, test.double64);
        EXPECT_EQ(obj.string, test.string);
        EXPECT_EQ(obj.data, test.data);
        EXPECT_EQ(obj.map, test.map);
        EXPECT_EQ(obj.testEnum, test.testEnum);
        EXPECT_EQ(obj.time_point, test.time_point);
        EXPECT_EQ(obj.nestedStruct.names, test.nestedStruct.names);
        EXPECT_EQ(obj.nestedStruct.values, test.nestedStruct.values);
        EXPECT_EQ(obj.nestedStruct.tuple, test.nestedStruct.tuple);
    }
}
