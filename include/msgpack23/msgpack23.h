//
// Created by Rene Windegger on 12/02/2025.
//
#pragma once

#include <array>
#include <bit>
#include <bitset>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

namespace msgpack23 {
    enum class FormatConstants : std::uint8_t {
        // positive fixint = 0x00 - 0x7f
        // fixmap = 0x80 - 0x8f
        // fixarray = 0x90 - 0x9a
        // fixstr = 0xa0 - 0xbf
        // negative fixint = 0xe0 - 0xff

        nil = 0xc0,
        false_bool = 0xc2,
        true_bool = 0xc3,
        bin8 = 0xc4,
        bin16 = 0xc5,
        bin32 = 0xc6,
        ext8 = 0xc7,
        ext16 = 0xc8,
        ext32 = 0xc9,
        float32 = 0xca,
        float64 = 0xcb,
        uint8 = 0xcc,
        uint16 = 0xcd,
        uint32 = 0xce,
        uint64 = 0xcf,
        int8 = 0xd0,
        int16 = 0xd1,
        int32 = 0xd2,
        int64 = 0xd3,
        fixext1 = 0xd4,
        fixext2 = 0xd5,
        fixext4 = 0xd6,
        fixext8 = 0xd7,
        fixext16 = 0xd8,
        str8 = 0xd9,
        str16 = 0xda,
        str32 = 0xdb,
        array16 = 0xdc,
        array32 = 0xdd,
        map16 = 0xde,
        map32 = 0xdf
    };

    template<typename T, typename U = void>
    struct is_map : std::false_type {
    };

    template<typename T>
    struct is_map<
                T,
                std::void_t<
                    typename T::key_type,
                    typename T::mapped_type,
                    typename T::size_type,
                    typename T::allocator_type,
                    typename T::iterator,
                    typename T::const_iterator,
                    decltype(std::declval<T &>()[std::declval<const typename T::key_type &>()]),
                    decltype(std::declval<T>().size()),
                    decltype(std::declval<T>().begin()),
                    decltype(std::declval<T>().end()),
                    decltype(std::declval<T>().cbegin()),
                    decltype(std::declval<T>().cend())
                >
            > : std::true_type {
    };

    template<typename T, typename U = void>
    struct is_collection : std::false_type {
    };

    template<typename T>
    struct is_collection<
                T,
                std::void_t<
                    typename T::value_type,
                    typename T::size_type,
                    typename T::iterator,
                    typename T::const_iterator,
                    decltype(std::declval<T>().size()),
                    decltype(std::declval<T>().begin()),
                    decltype(std::declval<T>().end()),
                    decltype(std::declval<T>().cbegin()),
                    decltype(std::declval<T>().cend())
                >
            > : std::true_type {
    };

    template<typename T, typename U = void>
    struct is_emplace_available : std::false_type {
    };

    template<typename T>
    struct is_emplace_available<
                T,
                std::void_t<
                    decltype(std::declval<T>().emplace_back(std::declval<typename T::value_type>()))
                >
            > : std::true_type {
    };

    template<typename T, std::enable_if_t<std::is_integral_v<T>, int>  = 0>
    [[nodiscard]] constexpr T to_big_endian(T const value) noexcept {
        if constexpr (std::endian::native == std::endian::little) {
            return std::byteswap(value);
        } else {
            return value;
        }
    }

    template<typename T, std::enable_if_t<std::is_integral_v<T>, int>  = 0>
    [[nodiscard]] constexpr T from_big_endian(T const value) noexcept {
        if constexpr (std::endian::native == std::endian::little) {
            return std::byteswap(value);
        } else {
            return value;
        }
    }

    class Packer {
    public:
        template<class... Types>
        [[nodiscard]] std::vector<std::byte> operator()(Types const &... args) {
            (pack_type(args), ...);
            return data;
        }

    private:
        std::vector<std::byte> data;

        void emplace_constant(FormatConstants const &value) {
            data.emplace_back(static_cast<std::byte>(std::to_underlying(value)));
        }

        template<typename T, std::enable_if_t<std::is_integral_v<T>, int>  = 0>
        void emplace_integral(T const &value) {
            auto const serialize_value = to_big_endian(value);
            auto const bytes = std::bit_cast<std::array<std::byte, sizeof(serialize_value)> >(serialize_value);
            data.insert(data.end(), bytes.begin(), bytes.end());
        }

        [[nodiscard]] bool pack_map_header(size_t const n) {
            if (n < 16) {
                constexpr auto size_mask = static_cast<std::byte>(0b10000000);
                data.emplace_back(static_cast<std::byte>(n) | size_mask);
            } else if (n < std::numeric_limits<uint16_t>::max()) {
                emplace_constant(FormatConstants::map16);
                emplace_integral<uint16_t>(static_cast<uint16_t>(n));
            } else if (n < std::numeric_limits<uint32_t>::max()) {
                emplace_constant(FormatConstants::map32);
                emplace_integral<uint32_t>(static_cast<uint32_t>(n));
            } else {
                return false;
            }
            return true;
        }

        [[nodiscard]] bool pack_array_header(size_t const n) {
            if (n < 16) {
                constexpr auto size_mask = static_cast<std::byte>(0b10010000);
                data.emplace_back(static_cast<std::byte>(n) | size_mask);
            } else if (n < std::numeric_limits<uint16_t>::max()) {
                emplace_constant(FormatConstants::array16);
                emplace_integral<uint16_t>(static_cast<uint16_t>(n));
            } else if (n < std::numeric_limits<uint32_t>::max()) {
                emplace_constant(FormatConstants::array32);
                emplace_integral<uint32_t>(static_cast<uint32_t>(n));
            } else {
                return false;
            }
            return true;
        }

        template<
            typename T,
            std::enable_if_t<is_collection<T>{}, int>  = 0,
            std::enable_if_t<is_map<T>{}, int>  = 0,
            std::enable_if_t<!std::is_enum<T>{}, int>  = 0
        >
        void pack_type(T const &value) {
            if (!pack_map_header(value.size())) {
                return;
            }
            for (auto const &item: value) {
                pack_type(std::get<0>(item));
                pack_type(std::get<1>(item));
            }
        }

        template<
            typename T,
            std::enable_if_t<is_collection<T>{}, int>  = 0,
            std::enable_if_t<!is_map<T>{}, int>  = 0,
            std::enable_if_t<!std::is_enum<T>{}, int>  = 0
        >
        void pack_type(T const &value) {
            if (!pack_array_header(value.size())) {
                return;
            }
            for (auto const &item: value) {
                pack_type(item);
            }
        }

        template<
            typename T,
            std::enable_if_t<!is_collection<T>{}, int>  = 0,
            std::enable_if_t<!is_map<T>{}, int>  = 0,
            std::enable_if_t<std::is_enum<T>{}, int>  = 0
        >
        void pack_type(T const &value) {
            pack_type(std::to_underlying(value));
        }

        template<
            typename T,
            std::enable_if_t<!is_collection<T>{}, int>  = 0,
            std::enable_if_t<!is_map<T>{}, int>  = 0,
            std::enable_if_t<!std::is_enum<T>{}, int>  = 0
        >
        void pack_type(T const &value) {
            value.pack(*this);
        }

        template<typename T>
        void pack_type(std::chrono::time_point<T> const &value) {
            using duration_t = typename std::chrono::time_point<T>::duration;
            auto const count = static_cast<int64_t>(value.time_since_epoch().count());

            auto const nano_num = duration_t::period::ratio::num * (1000000000 / duration_t::period::den);
            int64_t nano_seconds = count % (1000000000 / nano_num) * nano_num;
            int64_t seconds{};
            if (nano_seconds < 0) {
                nano_seconds = 1000000000 + nano_seconds;
                --seconds;
            }
            seconds += count * duration_t::period::num / duration_t::period::den;
            if (seconds >> 34 == 0) {
                auto const data64 = static_cast<uint64_t>(nano_seconds) << 34 | static_cast<uint64_t>(seconds);
                if ((data64 & 0xFFFFFFFC00000000LL) == 0) {
                    emplace_constant(FormatConstants::fixext4);
                    emplace_integral(static_cast<int8_t>(-1));
                    auto const data32 = static_cast<uint32_t>(data64);
                    emplace_integral(data32);
                } else {
                    emplace_constant(FormatConstants::fixext8);
                    emplace_integral(static_cast<int8_t>(-1));
                    emplace_integral(data64);
                }
            } else {
                emplace_constant(FormatConstants::ext8);
                emplace_integral(static_cast<uint8_t>(12));
                emplace_integral(static_cast<int8_t>(-1));
                emplace_integral(static_cast<uint32_t>(nano_seconds));
                emplace_integral(seconds);
            }
        }

        template<size_t I = 0, typename... Elements, std::enable_if_t<I == sizeof...(Elements), int>  = 0>
        void pack_tuple(std::tuple<Elements...> const &) {
        }

        template<size_t I = 0, typename... Elements, std::enable_if_t<I < sizeof...(Elements), int>  = 0>
        void pack_tuple(std::tuple<Elements...> const &tuple) {
            pack_type(std::get<I>(tuple));
            pack_tuple<I + 1, Elements...>(tuple);
        }

        template<typename... Elements>
        void pack_type(std::tuple<Elements...> const &value) {
            pack_tuple(value);
        }
    };

    template<>
    inline void Packer::pack_type(int8_t const &value) {
        if (value > 31 || value < -32) {
            emplace_constant(FormatConstants::int8);
        }
        data.emplace_back(static_cast<std::byte>(value));
    }

    template<>
    inline void Packer::pack_type(int16_t const &value) {
        if (
            value > std::numeric_limits<int8_t>::min()
            and value < std::numeric_limits<int8_t>::max()
        ) {
            pack_type(static_cast<int8_t>(value));
        } else {
            emplace_constant(FormatConstants::int16);
            emplace_integral(value);
        }
    }

    template<>
    inline void Packer::pack_type(int32_t const &value) {
        if (
            value > std::numeric_limits<int16_t>::min()
            and value < std::numeric_limits<int16_t>::max()
        ) {
            pack_type(static_cast<int16_t>(value));
        } else {
            emplace_constant(FormatConstants::int32);
            emplace_integral(value);
        }
    }

    template<>
    inline void Packer::pack_type(int64_t const &value) {
        if (
            value > std::numeric_limits<int32_t>::min()
            and value < std::numeric_limits<int32_t>::max()
        ) {
            pack_type(static_cast<int32_t>(value));
        } else {
            emplace_constant(FormatConstants::int64);
            emplace_integral(value);
        }
    }

    template<>
    inline void Packer::pack_type(uint8_t const &value) {
        if (value < 0x80) {
            data.emplace_back(static_cast<std::byte>(value));
        } else {
            emplace_constant(FormatConstants::uint8);
            data.emplace_back(static_cast<std::byte>(value));
        }
    }

    template<>
    inline void Packer::pack_type(uint16_t const &value) {
        if (value > std::numeric_limits<uint8_t>::max()) {
            emplace_constant(FormatConstants::uint16);
            emplace_integral(value);
        } else {
            pack_type(static_cast<uint8_t>(value));
        }
    }

    template<>
    inline void Packer::pack_type(uint32_t const &value) {
        if (value > std::numeric_limits<uint16_t>::max()) {
            emplace_constant(FormatConstants::uint32);
            emplace_integral(value);
        } else {
            pack_type(static_cast<uint16_t>(value));
        }
    }

    template<>
    inline void Packer::pack_type(uint64_t const &value) {
        if (value > std::numeric_limits<uint32_t>::max()) {
            emplace_constant(FormatConstants::uint64);
            emplace_integral(value);
        } else {
            pack_type(static_cast<uint32_t>(value));
        }
    }

    template<>
    inline void Packer::pack_type(std::nullptr_t const &) {
        emplace_constant(FormatConstants::nil);
    }

    template<>
    inline void Packer::pack_type(bool const &value) {
        if (value) {
            emplace_constant(FormatConstants::true_bool);
        } else {
            emplace_constant(FormatConstants::false_bool);
        }
    }

    template<>
    inline void Packer::pack_type(float const &value) {
        emplace_constant(FormatConstants::float32);
        emplace_integral(std::bit_cast<uint32_t>(value));
    }

    template<>
    inline void Packer::pack_type(double const &value) {
        emplace_constant(FormatConstants::float64);
        emplace_integral(std::bit_cast<uint64_t>(value));
    }

    template<>
    inline void Packer::pack_type(std::string const &value) {
        if (value.size() < 32) {
            data.emplace_back(static_cast<std::byte>(value.size()) | static_cast<std::byte>(0b10100000));
        } else if (value.size() < std::numeric_limits<uint8_t>::max()) {
            emplace_constant(FormatConstants::str8);
            data.emplace_back(static_cast<std::byte>(value.size()));
        } else if (value.size() < std::numeric_limits<uint16_t>::max()) {
            emplace_constant(FormatConstants::str16);
            emplace_integral<uint16_t>(static_cast<uint16_t>(value.size()));
        } else if (value.size() < std::numeric_limits<uint32_t>::max()) {
            emplace_constant(FormatConstants::str32);
            emplace_integral<uint32_t>(static_cast<uint32_t>(value.size()));
        } else {
            return; // Give up if string is too long
        }

        for (auto const c: value) {
            data.emplace_back(static_cast<std::byte>(c));
        }
    }

    template<>
    inline void Packer::pack_type(std::vector<uint8_t> const &value) {
        if (value.size() < std::numeric_limits<uint8_t>::max()) {
            emplace_constant(FormatConstants::bin8);
            data.emplace_back(static_cast<std::byte>(value.size()));
        } else if (value.size() < std::numeric_limits<uint16_t>::max()) {
            emplace_constant(FormatConstants::bin16);
            emplace_integral<uint16_t>(static_cast<uint16_t>(value.size()));
        } else if (value.size() < std::numeric_limits<uint32_t>::max()) {
            emplace_constant(FormatConstants::bin32);
            emplace_integral<uint32_t>(static_cast<uint32_t>(value.size()));
        } else {
            return; // Give up if vector is too large
        }

        for (auto const elem: value) {
            data.emplace_back(static_cast<std::byte>(elem));
        }
    }

    template<typename T, typename U = void>
    struct is_packable_object : std::false_type {
    };

    template<typename T>
    struct is_packable_object<
                T,
                std::void_t<decltype(std::declval<T>().pack(std::declval<Packer &>()))>
            > : std::true_type {
    };

    class Unpacker {
    public:
        Unpacker() : data_start(nullptr), data_end(nullptr) {
        }

        Unpacker(std::byte const *const data, size_t const size) : data_start(data), data_end(data + size) {
        }

        template<typename... Types>
        void operator()(Types &... args) {
            (unpack_type(args), ...);
        }

    private:
        std::byte const *data_start;
        std::byte const *const data_end;

        [[nodiscard]] std::byte current() const {
            if (data_start < data_end) {
                return *data_start;
            }
            return static_cast<std::byte>(0);
        }

        void increment(size_t const count = 1) {
            if (data_end - data_start >= 0) {
                data_start += count;
            }
        }

        [[nodiscard]] bool check_constant(FormatConstants const &value) const {
            return current() == static_cast<std::byte>(std::to_underlying(value));
        }

        template<typename T, std::enable_if_t<std::is_unsigned_v<T>, int>  = 0>
        [[nodiscard]] T read_integral() {
            T result{};
            std::memcpy(&result, data_start, sizeof(T));
            data_start += sizeof(T);
            result = from_big_endian(result);
            return result;
        }

        template<FormatConstants FC, typename T, typename U>
        bool read_conditional(U &out) {
            if (check_constant(FC)) {
                increment();
                out = static_cast<U>(read_integral<T>());
                return true;
            }
            return false;
        }

        [[nodiscard]] size_t unpack_map_header() {
            size_t map_size = 0;
            if (read_conditional<FormatConstants::map32, uint32_t>(map_size)) {
            } else if (read_conditional<FormatConstants::map16, uint16_t>(map_size)) {
            } else {
                map_size = std::to_integer<size_t>(current() & static_cast<std::byte>(0b00001111));
                increment();
            }
            return map_size;
        }

        [[nodiscard]] size_t unpack_array_header() {
            size_t array_size = 0;
            if (read_conditional<FormatConstants::array32, uint32_t>(array_size)) {
            } else if (read_conditional<FormatConstants::array16, uint16_t>(array_size)) {
            } else {
                array_size = std::to_integer<size_t>(current() & static_cast<std::byte>(0b00001111));
                increment();
            }
            return array_size;
        }

        template<
            typename T,
            std::enable_if_t<is_collection<T>{}, int>  = 0,
            std::enable_if_t<is_map<T>{}, int>  = 0,
            std::enable_if_t<!std::is_enum<T>{}, int>  = 0
        >
        void unpack_type(T &value) {
            using KeyType = typename T::key_type;
            using ValueType = typename T::mapped_type;
            auto const map_size = unpack_map_header();
            for (auto i = 0; i < map_size; ++i) {
                KeyType key{};
                ValueType val{};
                unpack_type(key);
                unpack_type(val);
                value.insert_or_assign(key, val);
            }
        }

        template<
            typename T,
            std::enable_if_t<is_collection<T>{}, int>  = 0,
            std::enable_if_t<is_emplace_available<T>{}, int>  = 0,
            std::enable_if_t<!is_map<T>{}, int>  = 0,
            std::enable_if_t<!std::is_enum<T>{}, int>  = 0
        >
        void unpack_type(T &value) {
            using ValueType = typename T::value_type;
            auto const array_size = unpack_array_header();
            for (auto i = 0; i < array_size; ++i) {
                ValueType val{};
                unpack_type(val);
                value.emplace_back(val);
            }
        }

        template<
            typename T,
            std::enable_if_t<is_collection<T>{}, int>  = 0,
            std::enable_if_t<!is_emplace_available<T>{}, int>  = 0,
            std::enable_if_t<!is_map<T>{}, int>  = 0,
            std::enable_if_t<!std::is_enum<T>{}, int>  = 0
        >
        void unpack_type(T &value) {
            using ValueType = typename T::value_type;
            std::vector<ValueType> vec;
            unpack_type(vec);
            std::copy(vec.begin(), vec.end(), value.begin());
        }

        template<
            typename T,
            std::enable_if_t<!is_collection<T>{}, int>  = 0,
            std::enable_if_t<!is_map<T>{}, int>  = 0,
            std::enable_if_t<std::is_enum<T>{}, int>  = 0
        >
        void unpack_type(T &value) {
            unpack_type(reinterpret_cast<std::underlying_type_t<T> &>(value));
        }

        template<
            typename T,
            std::enable_if_t<!is_collection<T>{}, int>  = 0,
            std::enable_if_t<!is_map<T>{}, int>  = 0,
            std::enable_if_t<!std::is_enum<T>{}, int>  = 0
        >
        void unpack_type(T &value) {
            value.unpack(*this);
        }

        template<typename Clock, typename Duration>
        void unpack_type(std::chrono::time_point<Clock, Duration> &value) {
            using duration_t = typename std::chrono::time_point<Clock, Duration>::duration;
            using time_point_t = std::chrono::time_point<Clock, Duration>;
            time_point_t tp {};
            if (check_constant(FormatConstants::fixext4)) {
                increment();
                if (static_cast<int8_t>(current()) == -1) {
                    increment();
                    auto const seconds = read_integral<uint32_t>();
                    tp += std::chrono::seconds(seconds);
                    value = tp;
                    return;
                }
            } else if (check_constant(FormatConstants::fixext8)) {
                increment();
                if (static_cast<int8_t>(current()) == -1) {
                    increment();
                    auto const data64 = read_integral<uint64_t>();
                    auto const nano_seconds = static_cast<uint32_t>(data64 >> 34);
                    auto const seconds = data64 & 0x00000003FFFFFFFFLL;
                    tp += std::chrono::duration_cast<duration_t>(std::chrono::nanoseconds(nano_seconds));
                    tp += std::chrono::seconds(seconds);
                    value = tp;
                    return;
                }
            } else if (check_constant(FormatConstants::ext8)) {
                increment();
                auto const size = read_integral<uint8_t>();
                if (static_cast<int8_t>(current()) == -1) {
                    increment();
                    auto const nano_seconds = read_integral<uint32_t>();
                    auto const seconds = static_cast<int64_t>(read_integral<uint64_t>());
                    tp += std::chrono::duration_cast<duration_t>(std::chrono::nanoseconds(nano_seconds));
                    tp += std::chrono::seconds(seconds);
                    value = tp;
                    return;
                }
            }
        }

        template<size_t I = 0, typename... Elements, std::enable_if_t<I == sizeof...(Elements), int>  = 0>
        void unpack_tuple(std::tuple<Elements...> &) {
        }

        template<size_t I = 0, typename... Elements, std::enable_if_t<I < sizeof...(Elements), int>  = 0>
        void unpack_tuple(std::tuple<Elements...> &tuple) {
            unpack_type(std::get<I>(tuple));
            unpack_tuple<I + 1, Elements...>(tuple);
        }

        template<typename... Elements>
        void unpack_type(std::tuple<Elements...> &value) {
            unpack_tuple(value);
        }
    };

    template<>
    inline void Unpacker::unpack_type(int8_t &value) {
        if (check_constant(FormatConstants::int8)) {
            increment();
            value = static_cast<int8_t>(current());
            increment();
        } else {
            value = static_cast<int8_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(int16_t &value) {
        if (check_constant(FormatConstants::int16)) {
            increment();
            auto const tmp = read_integral<uint16_t>();
            value = static_cast<int16_t>(tmp);
        } else if (check_constant(FormatConstants::int8)) {
            int8_t val;
            unpack_type(val);
            value = static_cast<int16_t>(val);
        } else {
            value = static_cast<int16_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(int32_t &value) {
        if (check_constant(FormatConstants::int32)) {
            increment();
            auto const tmp = read_integral<uint32_t>();
            value = static_cast<int32_t>(tmp);
        } else if (check_constant(FormatConstants::int16)) {
            int16_t val;
            unpack_type(val);
            value = val;
        } else if (check_constant(FormatConstants::int8)) {
            int8_t val;
            unpack_type(val);
            value = static_cast<int32_t>(val);
        } else {
            value = static_cast<int32_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(int64_t &value) {
        if (check_constant(FormatConstants::int64)) {
            increment();
            auto const tmp = read_integral<uint64_t>();
            value = static_cast<int64_t>(tmp);
        } else if (check_constant(FormatConstants::int32)) {
            int32_t val;
            unpack_type(val);
            value = val;
        } else if (check_constant(FormatConstants::int16)) {
            int16_t val;
            unpack_type(val);
            value = val;
        } else if (check_constant(FormatConstants::int8)) {
            int8_t val;
            unpack_type(val);
            value = static_cast<int64_t>(val);
        } else {
            value = static_cast<int64_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(uint8_t &value) {
        if (check_constant(FormatConstants::uint8)) {
            increment();
            value = std::to_integer<uint8_t>(current());
            increment();
        } else {
            value = std::to_integer<uint8_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(uint16_t &value) {
        if (check_constant(FormatConstants::uint16)) {
            increment();
            value = read_integral<uint16_t>();
        } else if (check_constant(FormatConstants::uint8)) {
            increment();
            value = std::to_integer<uint16_t>(current());
            increment();
        } else {
            value = std::to_integer<uint16_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(uint32_t &value) {
        if (check_constant(FormatConstants::uint32)) {
            increment();
            value = read_integral<uint32_t>();
        } else if (check_constant(FormatConstants::uint16)) {
            increment();
            value = read_integral<uint16_t>();
        } else if (check_constant(FormatConstants::uint8)) {
            increment();
            value = std::to_integer<uint32_t>(current());
            increment();
        } else {
            value = std::to_integer<uint32_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(uint64_t &value) {
        if (check_constant(FormatConstants::uint64)) {
            increment();
            value = read_integral<uint64_t>();
        } else if (check_constant(FormatConstants::uint32)) {
            increment();
            value = read_integral<uint32_t>();
        } else if (check_constant(FormatConstants::uint16)) {
            increment();
            value = read_integral<uint16_t>();
        } else if (check_constant(FormatConstants::uint8)) {
            increment();
            value = std::to_integer<uint64_t>(current());
            increment();
        } else {
            value = std::to_integer<uint64_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(std::nullptr_t &) {
        increment();
    }

    template<>
    inline void Unpacker::unpack_type(bool &value) {
        value = !check_constant(FormatConstants::false_bool);
        increment();
    }

    template<>
    inline void Unpacker::unpack_type(float &value) {
        if (check_constant(FormatConstants::float32)) {
            increment();
            auto const data = read_integral<uint32_t>();
            value = std::bit_cast<float>(data);
        }
    }

    template<>
    inline void Unpacker::unpack_type(double &value) {
        if (check_constant(FormatConstants::float64)) {
            increment();
            auto const data = read_integral<uint64_t>();
            value = std::bit_cast<double>(data);
        }
    }

    template<>
    inline void Unpacker::unpack_type(std::string &value) {
        size_t str_size = 0;
        if (read_conditional<FormatConstants::str32, uint32_t>(str_size)) {
        } else if (read_conditional<FormatConstants::str16, uint16_t>(str_size)) {
        } else if (read_conditional<FormatConstants::str8, uint8_t>(str_size)) {
        } else {
            str_size = std::to_integer<size_t>(current() & static_cast<std::byte>(0b00011111));
            increment();
        }
        if (data_start + str_size <= data_end) {
            value = std::string(reinterpret_cast<const char *>(data_start), str_size);
            increment(str_size);
        }
    }

    template<>
    inline void Unpacker::unpack_type(std::vector<uint8_t> &value) {
        size_t bin_size = 0;
        if (read_conditional<FormatConstants::bin32, uint32_t>(bin_size)) {
        } else if (read_conditional<FormatConstants::bin16, uint16_t>(bin_size)) {
        } else if (read_conditional<FormatConstants::bin8, uint8_t>(bin_size)) {
        }
        if (data_start + bin_size <= data_end) {
            value = std::vector<uint8_t>(reinterpret_cast<uint8_t const *>(data_start),
                                         reinterpret_cast<uint8_t const *>(data_start) + bin_size);
            increment(bin_size);
        }
    }

    template<typename T, typename U = void>
    struct is_unpackable_object : std::false_type {
    };

    template<typename T>
    struct is_unpackable_object<
                T,
                std::void_t<decltype(std::declval<T>().unpack(std::declval<Packer &>()))>
            > : std::true_type {
    };


    template<
        typename PackableObject,
        std::enable_if_t<is_packable_object<PackableObject>{}, int>  = 0
    >
    [[nodiscard]] std::vector<std::byte> pack(PackableObject const &obj) {
        Packer packer;
        return obj.pack(packer);
    }

    template<
        typename PackableObject,
        std::enable_if_t<!is_packable_object<PackableObject>{}, int>  = 0
    >
    [[nodiscard]] std::vector<std::byte> pack(PackableObject const &obj) {
        static_assert(false, "PackableObject is not implemented.");
        return {};
    }

    template<
        typename UnpackableObject,
        std::enable_if_t<is_unpackable_object<UnpackableObject>{}, int>  = 0
    >
    [[nodiscard]] UnpackableObject unpack(std::byte const *data_start, size_t const size) {
        UnpackableObject obj{};
        Unpacker unpacker{data_start, size};
        obj.unpack(unpacker);
        return obj;
    }

    template<
        typename UnpackableObject,
        std::enable_if_t<!is_unpackable_object<UnpackableObject>{}, int>  = 0
    >
    [[nodiscard]] UnpackableObject unpack(std::byte const *, size_t const) {
        static_assert(false, "UnpackableObject is not implemented.");
        return {};
    }

    template<typename UnpackableObject>
    [[nodiscard]] UnpackableObject unpack(std::vector<std::byte> const &data) {
        return unpack<UnpackableObject>(data.data(), data.size());
    }
}
