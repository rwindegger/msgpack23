//
// Created by Neara Software on 06/03/2026.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

namespace {

    // Counts individual byte emplace calls (operator=) and bulk write() calls.
    // test_container holds a raw pointer so that all copies (the Packer stores
    // the iterator by value) update the same counts object.
    struct counts {
        std::size_t emplace_count{0};
        std::size_t write_count{0};
    };

    struct test_container {
        using value_type      = std::uint8_t;
        using difference_type = std::ptrdiff_t;

        counts *c;

        test_container &operator*()     { return *this; }
        test_container &operator++()    { return *this; }
        test_container  operator++(int) { return *this; }

        test_container &operator=(std::uint8_t) {
            ++c->emplace_count;
            return *this;
        }

        void write(const std::uint8_t *, std::size_t) {
            ++c->write_count;
        }
    };

    template<typename T>
    counts pack(T const &value) {
        counts c{};
        test_container iter{&c};
        msgpack23::Packer<std::uint8_t, test_container> packer{iter};
        packer(value);
        return c;
    }

    // ── int8_t(42) ────────────────────────────────────────────────────────────
    // 42 > 31: int8 format byte + value byte, no write.
    TEST(msgpack23_bulk_write, Int8UsesNoBulkWrite) {
        auto c = pack(std::int8_t{42});
        EXPECT_EQ(c.emplace_count, 2u);
        EXPECT_EQ(c.write_count,   0u);
    }

    // ── uint32_t(70000) ───────────────────────────────────────────────────────
    // 70000 > 0xFFFF: uint32 format byte + 4 bytes via std::copy, no write.
    // emplace_integral's requires-check on sizeof(T) does not match write().
    TEST(msgpack23_bulk_write, Uint32UsesNoBulkWrite) {
        auto c = pack(std::uint32_t{70'000});
        EXPECT_EQ(c.emplace_count, 5u);
        EXPECT_EQ(c.write_count,   0u);
    }

    // ── std::string{"hello"} ─────────────────────────────────────────────────
    // fixstr header byte (emplace) + body via write().
    TEST(msgpack23_bulk_write, StringUsesBulkWrite) {
        auto c = pack(std::string{"hello"});
        EXPECT_EQ(c.emplace_count, 1u);
        EXPECT_EQ(c.write_count,   1u);
    }

    // ── std::vector<uint8_t> ─────────────────────────────────────────────────
    // bin8 format byte + length byte (emplace) + body via write().
    TEST(msgpack23_bulk_write, VectorUsesBulkWrite) {
        auto c = pack(std::vector<std::uint8_t>{1, 2, 3, 4, 5});
        EXPECT_EQ(c.emplace_count, 2u);
        EXPECT_EQ(c.write_count,   1u);
    }

    // ── std::span<const uint8_t> ─────────────────────────────────────────────
    // Same bin8 layout as vector.
    TEST(msgpack23_bulk_write, SpanUsesBulkWrite) {
        std::vector<std::uint8_t> storage{10, 20, 30};
        auto c = pack(std::span<const std::uint8_t>{storage});
        EXPECT_EQ(c.emplace_count, 2u);
        EXPECT_EQ(c.write_count,   1u);
    }

    // ── std::vector<std::string>{"foo","bar","baz"} ───────────────────────────
    // fixarray header byte + per element: fixstr header byte + write().
    TEST(msgpack23_bulk_write, VectorOfStringsUsesBulkWritePerElement) {
        std::vector<std::string> strings{"foo", "bar", "baz"};
        auto c = pack(strings);
        EXPECT_EQ(c.emplace_count, 4u);
        EXPECT_EQ(c.write_count,   3u);
    }

} // namespace
