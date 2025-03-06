//
// Created by Rene Windegger on 06/03/2025.
//

#include <gtest/gtest.h>
#include <msgpack23/msgpack23.h>

struct TestStruct {
    std::int64_t int64;
    bool boolean;
    std::vector<std::byte> bytes;
    std::string string;

    template<typename T>
    std::vector<std::byte> pack(T& packer) {
        return packer(int64, boolean, bytes, string);
    }
};

TEST(msgpack23, DryRunTest) {
    TestStruct test_struct{ 42, false, { static_cast<std::byte>(42) }, "Hello World!" };
    msgpack23::Packer<true> dry_run_packer{};
    auto dry_run_struct = test_struct.pack(dry_run_packer);
    msgpack23::Packer<false> packer{};
    auto packed_struct = test_struct.pack(packer);
    EXPECT_EQ(packer.size(), dry_run_packer.size());
}
