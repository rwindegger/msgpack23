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
#include <span>
#include <string>
#include <type_traits>
#include <variant>
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

    template<typename T>
    concept MapLike = requires(T t, const typename T::key_type &k)
    {
        typename T::key_type;
        typename T::mapped_type;
        typename T::size_type;
        typename T::allocator_type;
        typename T::iterator;
        typename T::const_iterator;

        { t[k] } -> std::same_as<typename T::mapped_type &>;
        { t.size() } -> std::same_as<typename T::size_type>;
        { t.begin() } -> std::same_as<typename T::iterator>;
        { t.end() } -> std::same_as<typename T::iterator>;
        { t.cbegin() } -> std::same_as<typename T::const_iterator>;
        { t.cend() } -> std::same_as<typename T::const_iterator>;
    };

    template<typename T>
    concept CollectionLike = requires(T t)
    {
        typename T::value_type;
        typename T::size_type;
        typename T::iterator;
        typename T::const_iterator;

        { t.size() } -> std::same_as<typename T::size_type>;
        { t.begin() } -> std::same_as<typename T::iterator>;
        { t.end() } -> std::same_as<typename T::iterator>;
        { t.cbegin() } -> std::same_as<typename T::const_iterator>;
        { t.cend() } -> std::same_as<typename T::const_iterator>;
    };

    template<typename T>
    concept EmplaceAvailable = requires(T t, typename T::value_type val)
    {
        // Simply require that calling t.emplace_back(val) is valid
        t.emplace_back(val);
    };

    template<typename T>
    concept EnumLike = std::is_enum_v<T>;

    template<typename T>
    struct is_variant : std::false_type {
    };

    template<typename... Types>
    struct is_variant<std::variant<Types...> > : std::true_type {
    };

    template<typename T>
    inline constexpr bool is_variant_v = is_variant<T>::value;

    template<typename T>
    concept VariantLike = is_variant_v<T>;

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
            return data_;
        }

    private:
        std::vector<std::byte> data_;

        void emplace_constant(FormatConstants const &value) {
            data_.emplace_back(static_cast<std::byte>(std::to_underlying(value)));
        }

        template<typename T>
            requires std::is_integral_v<T>
        void emplace_integral(T const &value) {
            auto const serialize_value = to_big_endian(value);
            auto const bytes = std::bit_cast<std::array<std::byte, sizeof(serialize_value)> >(serialize_value);
            data_.insert(data_.end(), bytes.begin(), bytes.end());
        }

        [[nodiscard]] bool pack_map_header(std::size_t const n) {
            if (n < 16) {
                constexpr auto size_mask = static_cast<std::byte>(0b10000000);
                data_.emplace_back(static_cast<std::byte>(n) | size_mask);
            } else if (n < std::numeric_limits<std::uint16_t>::max()) {
                emplace_constant(FormatConstants::map16);
                emplace_integral(static_cast<std::uint16_t>(n));
            } else if (n < std::numeric_limits<std::uint32_t>::max()) {
                emplace_constant(FormatConstants::map32);
                emplace_integral(static_cast<std::uint32_t>(n));
            } else {
                return false;
            }
            return true;
        }

        [[nodiscard]] bool pack_array_header(std::size_t const n) {
            if (n < 16) {
                constexpr auto size_mask = static_cast<std::byte>(0b10010000);
                data_.emplace_back(static_cast<std::byte>(n) | size_mask);
            } else if (n < std::numeric_limits<std::uint16_t>::max()) {
                emplace_constant(FormatConstants::array16);
                emplace_integral(static_cast<std::uint16_t>(n));
            } else if (n < std::numeric_limits<std::uint32_t>::max()) {
                emplace_constant(FormatConstants::array32);
                emplace_integral(static_cast<std::uint32_t>(n));
            } else {
                return false;
            }
            return true;
        }

        template<CollectionLike T>
            requires MapLike<T> && (!EnumLike<T>)
        void pack_type(T const &value) {
            if (!pack_map_header(value.size())) {
                throw std::length_error("Map is too long to be serialized.");
            }
            for (auto const &item: value) {
                pack_type(std::get<0>(item));
                pack_type(std::get<1>(item));
            }
        }

        template<CollectionLike T>
            requires (!MapLike<T>) && (!EnumLike<T>)
        void pack_type(T const &value) {
            if (!pack_array_header(value.size())) {
                throw std::length_error("Collection is too long to be serialized.");
            }
            for (auto const &item: value) {
                pack_type(item);
            }
        }

        template<EnumLike T>
        void pack_type(T const &value) {
            pack_type(std::to_underlying(value));
        }

        template<typename T>
            requires VariantLike<T>
        void pack_type(T const &value) {
            std::vector<std::byte> data{};
            std::visit([this, &data](auto const &arg) {
                Packer packer{};
                data = packer(arg);
            }, value);
            auto const index = static_cast<std::int8_t>(value.index());
            if (index > 127) {
                throw std::overflow_error("Variant index is to large to be serialized.");
            }
            if (data.size() == 1) {
                emplace_constant(FormatConstants::fixext1);
            } else if (data.size() == 2) {
                emplace_constant(FormatConstants::fixext2);
            } else if (data.size() == 4) {
                emplace_constant(FormatConstants::fixext4);
            } else if (data.size() == 8) {
                emplace_constant(FormatConstants::fixext8);
            } else if (data.size() < std::numeric_limits<std::uint8_t>::max()) {
                emplace_constant(FormatConstants::ext8);
                emplace_integral(static_cast<std::uint8_t>(data.size()));
            } else if (data.size() < std::numeric_limits<std::uint16_t>::max()) {
                emplace_constant(FormatConstants::ext16);
                emplace_integral(static_cast<std::uint16_t>(data.size()));
            } else if (data.size() < std::numeric_limits<std::uint32_t>::max()) {
                emplace_constant(FormatConstants::ext32);
                emplace_integral(static_cast<std::uint32_t>(data.size()));
            } else {
                throw std::length_error("Variant is too long to be serialized.");
            }
            emplace_integral(index);
            data_.insert(data_.end(), data.begin(), data.end());
        }

        template<typename T>
            requires (!CollectionLike<T>) && (!MapLike<T>) && (!EnumLike<T>) && (!VariantLike<T>)
        void pack_type(T const &value) {
            value.pack(*this);
        }

        template<typename T>
        void pack_type(std::chrono::time_point<T> const &value) {
            using duration_t = typename std::chrono::time_point<T>::duration;
            auto const count = static_cast<std::int64_t>(value.time_since_epoch().count());
            constexpr auto nano_seconds_per_second = 1'000'000'000;
            constexpr auto nano_seconds_mask = 0xFFFFFFFC00000000LL;

            auto const nano_num = duration_t::period::ratio::num * (nano_seconds_per_second / duration_t::period::den);
            std::int64_t nano_seconds = count % (nano_seconds_per_second / nano_num) * nano_num;
            std::int64_t seconds{};
            if (nano_seconds < 0) {
                nano_seconds = nano_seconds_per_second + nano_seconds;
                --seconds;
            }
            seconds += count * duration_t::period::num / duration_t::period::den;
            if (seconds >> 34 == 0) {
                auto const data64 = static_cast<std::uint64_t>(nano_seconds) << 34 | static_cast<std::uint64_t>(
                                        seconds);
                if ((data64 & nano_seconds_mask) == 0) {
                    emplace_constant(FormatConstants::fixext4);
                    emplace_integral(static_cast<std::int8_t>(-1));
                    auto const data32 = static_cast<std::uint32_t>(data64);
                    emplace_integral(data32);
                } else {
                    emplace_constant(FormatConstants::fixext8);
                    emplace_integral(static_cast<std::int8_t>(-1));
                    emplace_integral(data64);
                }
            } else {
                emplace_constant(FormatConstants::ext8);
                emplace_integral(static_cast<std::uint8_t>(12));
                emplace_integral(static_cast<std::int8_t>(-1));
                emplace_integral(static_cast<std::uint32_t>(nano_seconds));
                emplace_integral(seconds);
            }
        }

        template<typename... Elements>
        void pack_type(std::tuple<Elements...> const &value) {
            std::apply(
                [this](auto const &... elems) {
                    (pack_type(elems), ...);
                },
                value
            );
        }
    };

    template<>
    inline void Packer::pack_type(std::int8_t const &value) {
        if (value > 31 || value < -32) {
            emplace_constant(FormatConstants::int8);
        }
        data_.emplace_back(static_cast<std::byte>(value));
    }

    template<>
    inline void Packer::pack_type(std::int16_t const &value) {
        if (
            value > std::numeric_limits<std::int8_t>::min()
            and value < std::numeric_limits<std::int8_t>::max()
        ) {
            pack_type(static_cast<std::int8_t>(value));
        } else {
            emplace_constant(FormatConstants::int16);
            emplace_integral(value);
        }
    }

    template<>
    inline void Packer::pack_type(std::int32_t const &value) {
        if (
            value > std::numeric_limits<std::int16_t>::min()
            and value < std::numeric_limits<std::int16_t>::max()
        ) {
            pack_type(static_cast<std::int16_t>(value));
        } else {
            emplace_constant(FormatConstants::int32);
            emplace_integral(value);
        }
    }

    template<>
    inline void Packer::pack_type(std::int64_t const &value) {
        if (
            value > std::numeric_limits<std::int32_t>::min()
            and value < std::numeric_limits<std::int32_t>::max()
        ) {
            pack_type(static_cast<std::int32_t>(value));
        } else {
            emplace_constant(FormatConstants::int64);
            emplace_integral(value);
        }
    }

    template<>
    inline void Packer::pack_type(std::uint8_t const &value) {
        if (value < 0x80) {
            data_.emplace_back(static_cast<std::byte>(value));
        } else {
            emplace_constant(FormatConstants::uint8);
            data_.emplace_back(static_cast<std::byte>(value));
        }
    }

    template<>
    inline void Packer::pack_type(std::uint16_t const &value) {
        if (value > std::numeric_limits<std::uint8_t>::max()) {
            emplace_constant(FormatConstants::uint16);
            emplace_integral(value);
        } else {
            pack_type(static_cast<std::uint8_t>(value));
        }
    }

    template<>
    inline void Packer::pack_type(std::uint32_t const &value) {
        if (value > std::numeric_limits<std::uint16_t>::max()) {
            emplace_constant(FormatConstants::uint32);
            emplace_integral(value);
        } else {
            pack_type(static_cast<std::uint16_t>(value));
        }
    }

    template<>
    inline void Packer::pack_type(std::uint64_t const &value) {
        if (value > std::numeric_limits<std::uint32_t>::max()) {
            emplace_constant(FormatConstants::uint64);
            emplace_integral(value);
        } else {
            pack_type(static_cast<std::uint32_t>(value));
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
        emplace_integral(std::bit_cast<std::uint32_t>(value));
    }

    template<>
    inline void Packer::pack_type(double const &value) {
        emplace_constant(FormatConstants::float64);
        emplace_integral(std::bit_cast<std::uint64_t>(value));
    }

    template<>
    inline void Packer::pack_type(std::string const &value) {
        if (value.size() < 32) {
            data_.emplace_back(static_cast<std::byte>(value.size()) | static_cast<std::byte>(0b10100000));
        } else if (value.size() < std::numeric_limits<std::uint8_t>::max()) {
            emplace_constant(FormatConstants::str8);
            data_.emplace_back(static_cast<std::byte>(value.size()));
        } else if (value.size() < std::numeric_limits<std::uint16_t>::max()) {
            emplace_constant(FormatConstants::str16);
            emplace_integral(static_cast<std::uint16_t>(value.size()));
        } else if (value.size() < std::numeric_limits<std::uint32_t>::max()) {
            emplace_constant(FormatConstants::str32);
            emplace_integral(static_cast<std::uint32_t>(value.size()));
        } else {
            throw std::length_error("String is too long to be serialized.");
        }

        data_.reserve(data_.size() + value.size());
        data_.insert(
            data_.end(),
            reinterpret_cast<const std::byte *>(value.data()),
            reinterpret_cast<const std::byte *>(value.data()) + value.size()
        );
    }

    template<>
    inline void Packer::pack_type(std::vector<std::uint8_t> const &value) {
        if (value.size() < std::numeric_limits<std::uint8_t>::max()) {
            emplace_constant(FormatConstants::bin8);
            data_.emplace_back(static_cast<std::byte>(value.size()));
        } else if (value.size() < std::numeric_limits<std::uint16_t>::max()) {
            emplace_constant(FormatConstants::bin16);
            emplace_integral(static_cast<std::uint16_t>(value.size()));
        } else if (value.size() < std::numeric_limits<std::uint32_t>::max()) {
            emplace_constant(FormatConstants::bin32);
            emplace_integral(static_cast<std::uint32_t>(value.size()));
        } else {
            throw std::length_error("Vector is too long to be serialized.");
        }

        data_.reserve(data_.size() + value.size());
        data_.insert(
            data_.end(),
            reinterpret_cast<const std::byte *>(value.data()),
            reinterpret_cast<const std::byte *>(value.data()) + value.size()
        );
    }

    template<typename T>
    concept PackableObject = requires(T t, Packer p)
    {
        { t.pack(p) } -> std::same_as<std::vector<std::byte> >;
    };

    class Unpacker {
    public:
        Unpacker() : data_() {
        }

        explicit Unpacker(std::span<std::byte const> data) : data_(data) {
        }

        template<typename... Types>
        void operator()(Types &... args) {
            (unpack_type(args), ...);
        }

    private:
        std::span<std::byte const> data_;
        std::size_t position_{0};

        [[nodiscard]] std::byte current() const {
            if (position_ < data_.size()) {
                return data_[position_];
            }
            throw std::out_of_range("Unpacker doesn't have enough data.");
        }

        void increment(std::size_t const count = 1) {
            if (position_ + count > data_.size()) {
                throw std::out_of_range("Unpacker doesn't have enough data.");
            }
            position_ += count;
        }

        [[nodiscard]] bool check_constant(FormatConstants const &value) const {
            return current() == static_cast<std::byte>(std::to_underlying(value));
        }

        template<typename T, std::enable_if_t<std::is_unsigned_v<T>, int>  = 0>
        [[nodiscard]] T read_integral() {
            if (position_ + sizeof(T) > data_.size()) {
                throw std::out_of_range("Unpacker doesn't have enough data.");
            }
            T result{};
            std::memcpy(&result, data_.data() + position_, sizeof(T));
            increment(sizeof(T));
            result = from_big_endian(result);
            return result;
        }

        template<FormatConstants FC, typename T, typename U>
        [[nodiscard]] bool read_conditional(U &out) {
            if (check_constant(FC)) {
                increment();
                out = static_cast<U>(read_integral<T>());
                return true;
            }
            return false;
        }

        template<typename Variant, std::size_t Index = 0>
        Variant create_variant_by_index(std::size_t const i) {
            if constexpr (Index < std::variant_size_v<Variant>) {
                if (i == Index) {
                    return Variant{std::in_place_index<Index>};
                }
                return create_variant_by_index<Variant, Index + 1>(i);
            } else {
                throw std::logic_error("Invalid variant index");
            }
        }

        [[nodiscard]] std::size_t unpack_map_header() {
            std::size_t map_size = 0;
            if (read_conditional<FormatConstants::map32, std::uint32_t>(map_size)) {
            } else if (read_conditional<FormatConstants::map16, std::uint16_t>(map_size)) {
            } else {
                map_size = std::to_integer<std::size_t>(current() & static_cast<std::byte>(0b00001111));
                increment();
            }
            return map_size;
        }

        [[nodiscard]] std::size_t unpack_array_header() {
            std::size_t array_size = 0;
            if (read_conditional<FormatConstants::array32, std::uint32_t>(array_size)) {
            } else if (read_conditional<FormatConstants::array16, std::uint16_t>(array_size)) {
            } else {
                array_size = std::to_integer<std::size_t>(current() & static_cast<std::byte>(0b00001111));
                increment();
            }
            return array_size;
        }

        template<MapLike T>
            requires CollectionLike<T> && (!EnumLike<T>)
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

        template<CollectionLike T>
            requires (!MapLike<T>) && EmplaceAvailable<T> && (!EnumLike<T>)
        void unpack_type(T &value) {
            using ValueType = typename T::value_type;
            auto const array_size = unpack_array_header();
            for (auto i = 0; i < array_size; ++i) {
                ValueType val{};
                unpack_type(val);
                value.emplace_back(val);
            }
        }

        template<CollectionLike T>
            requires (!MapLike<T>) && (!EmplaceAvailable<T>) && (!EnumLike<T>)
        void unpack_type(T &value) {
            using ValueType = typename T::value_type;
            std::vector<ValueType> vec;
            unpack_type(vec);
            std::copy(vec.begin(), vec.end(), value.begin());
        }

        template<EnumLike T>
        void unpack_type(T &value) {
            unpack_type(reinterpret_cast<std::underlying_type_t<T> &>(value));
        }

        template<typename T>
            requires VariantLike<T>
        void unpack_type(T &value) {
            std::size_t size = 0;
            if (check_constant(FormatConstants::fixext1)) {
                increment();
                size = 1;
            } else if (check_constant(FormatConstants::fixext2)) {
                increment();
                size = 2;
            } else if (check_constant(FormatConstants::fixext4)) {
                increment();
                size = 4;
            } else if (check_constant(FormatConstants::fixext8)) {
                increment();
                size = 8;
            } else if (check_constant(FormatConstants::ext8)) {
                increment();
                size = read_integral<std::uint8_t>();
            } else if (check_constant(FormatConstants::ext16)) {
                increment();
                size = read_integral<std::uint16_t>();
            } else if (check_constant(FormatConstants::ext32)) {
                increment();
                size = read_integral<std::uint32_t>();
            } else {
                throw std::logic_error("Unexpected format for std::variant");
            }
            auto const index = static_cast<std::int8_t>(read_integral<std::uint8_t>());

            if (index < 0 || index > static_cast<std::int8_t>(std::variant_size_v<T> - 1)) {
                throw std::out_of_range("Invalid variant index");
            }

            auto const data_start = data_.subspan(position_, size);
            increment(size);

            value = create_variant_by_index<T>(index);

            std::visit([this, &data_start](auto &arg) {
                Unpacker unpacker(data_start);
                unpacker(arg);
            }, value);
        }

        template<typename T>
            requires (!CollectionLike<T>) && (!MapLike<T>) && (!EnumLike<T>) && (!VariantLike<T>)
        void unpack_type(T &value) {
            value.unpack(*this);
        }

        template<typename Clock, typename Duration>
        void unpack_type(std::chrono::time_point<Clock, Duration> &value) {
            using duration_t = typename std::chrono::time_point<Clock, Duration>::duration;
            using time_point_t = std::chrono::time_point<Clock, Duration>;
            time_point_t tp{};
            if (check_constant(FormatConstants::fixext4)) {
                increment();
                if (static_cast<std::int8_t>(current()) == -1) {
                    increment();
                    auto const seconds = read_integral<std::uint32_t>();
                    tp += std::chrono::seconds(seconds);
                    value = tp;
                    return;
                }
            } else if (check_constant(FormatConstants::fixext8)) {
                increment();
                if (static_cast<std::int8_t>(current()) == -1) {
                    increment();
                    constexpr auto seconds_mask = 0x00000003FFFFFFFFLL;
                    auto const data64 = read_integral<std::uint64_t>();
                    auto const nano_seconds = static_cast<std::uint32_t>(data64 >> 34);
                    auto const seconds = data64 & seconds_mask;
                    tp += std::chrono::duration_cast<duration_t>(std::chrono::nanoseconds(nano_seconds));
                    tp += std::chrono::seconds(seconds);
                    value = tp;
                    return;
                }
            } else if (check_constant(FormatConstants::ext8)) {
                increment();
                auto const size = read_integral<std::uint8_t>();
                if (static_cast<std::int8_t>(current()) == -1) {
                    increment();
                    auto const nano_seconds = read_integral<std::uint32_t>();
                    auto const seconds = static_cast<std::int64_t>(read_integral<std::uint64_t>());
                    tp += std::chrono::duration_cast<duration_t>(std::chrono::nanoseconds(nano_seconds));
                    tp += std::chrono::seconds(seconds);
                    value = tp;
                    return;
                }
            }
            throw std::logic_error("Unexpected value");
        }

        template<typename... Elements>
        void unpack_type(std::tuple<Elements...> &value) {
            std::apply(
                [this](auto &... elems) {
                    (unpack_type(elems), ...);
                },
                value
            );
        }
    };

    template<>
    inline void Unpacker::unpack_type(std::int8_t &value) {
        if (check_constant(FormatConstants::int8)) {
            increment();
            value = static_cast<std::int8_t>(current());
            increment();
        } else {
            value = static_cast<std::int8_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(std::int16_t &value) {
        if (check_constant(FormatConstants::int16)) {
            increment();
            auto const tmp = read_integral<std::uint16_t>();
            value = static_cast<std::int16_t>(tmp);
        } else if (check_constant(FormatConstants::int8)) {
            std::int8_t val;
            unpack_type(val);
            value = static_cast<std::int16_t>(val);
        } else {
            value = static_cast<std::int16_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(std::int32_t &value) {
        if (check_constant(FormatConstants::int32)) {
            increment();
            auto const tmp = read_integral<std::uint32_t>();
            value = static_cast<std::int32_t>(tmp);
        } else if (check_constant(FormatConstants::int16)) {
            std::int16_t val;
            unpack_type(val);
            value = val;
        } else if (check_constant(FormatConstants::int8)) {
            std::int8_t val;
            unpack_type(val);
            value = static_cast<std::int32_t>(val);
        } else {
            value = static_cast<std::int32_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(std::int64_t &value) {
        if (check_constant(FormatConstants::int64)) {
            increment();
            auto const tmp = read_integral<std::uint64_t>();
            value = static_cast<std::int64_t>(tmp);
        } else if (check_constant(FormatConstants::int32)) {
            std::int32_t val;
            unpack_type(val);
            value = val;
        } else if (check_constant(FormatConstants::int16)) {
            std::int16_t val;
            unpack_type(val);
            value = val;
        } else if (check_constant(FormatConstants::int8)) {
            std::int8_t val;
            unpack_type(val);
            value = static_cast<std::int64_t>(val);
        } else {
            value = static_cast<std::int64_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(std::uint8_t &value) {
        if (check_constant(FormatConstants::uint8)) {
            increment();
            value = std::to_integer<std::uint8_t>(current());
            increment();
        } else {
            value = std::to_integer<std::uint8_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(std::uint16_t &value) {
        if (check_constant(FormatConstants::uint16)) {
            increment();
            value = read_integral<std::uint16_t>();
        } else if (check_constant(FormatConstants::uint8)) {
            increment();
            value = std::to_integer<std::uint16_t>(current());
            increment();
        } else {
            value = std::to_integer<std::uint16_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(std::uint32_t &value) {
        if (check_constant(FormatConstants::uint32)) {
            increment();
            value = read_integral<std::uint32_t>();
        } else if (check_constant(FormatConstants::uint16)) {
            increment();
            value = read_integral<std::uint16_t>();
        } else if (check_constant(FormatConstants::uint8)) {
            increment();
            value = std::to_integer<std::uint32_t>(current());
            increment();
        } else {
            value = std::to_integer<std::uint32_t>(current());
            increment();
        }
    }

    template<>
    inline void Unpacker::unpack_type(std::uint64_t &value) {
        if (check_constant(FormatConstants::uint64)) {
            increment();
            value = read_integral<std::uint64_t>();
        } else if (check_constant(FormatConstants::uint32)) {
            increment();
            value = read_integral<std::uint32_t>();
        } else if (check_constant(FormatConstants::uint16)) {
            increment();
            value = read_integral<std::uint16_t>();
        } else if (check_constant(FormatConstants::uint8)) {
            increment();
            value = std::to_integer<std::uint64_t>(current());
            increment();
        } else {
            value = std::to_integer<std::uint64_t>(current());
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
            auto const data = read_integral<std::uint32_t>();
            value = std::bit_cast<float>(data);
        } else {
            throw std::logic_error("Unexpected value");
        }
    }

    template<>
    inline void Unpacker::unpack_type(double &value) {
        if (check_constant(FormatConstants::float64)) {
            increment();
            auto const data = read_integral<std::uint64_t>();
            value = std::bit_cast<double>(data);
        } else {
            throw std::logic_error("Unexpected value");
        }
    }

    template<>
    inline void Unpacker::unpack_type(std::string &value) {
        std::size_t str_size = 0;
        if (read_conditional<FormatConstants::str32, std::uint32_t>(str_size)) {
        } else if (read_conditional<FormatConstants::str16, std::uint16_t>(str_size)) {
        } else if (read_conditional<FormatConstants::str8, std::uint8_t>(str_size)) {
        } else {
            str_size = std::to_integer<std::size_t>(current() & static_cast<std::byte>(0b00011111));
            increment();
        }
        if (position_ + str_size > data_.size()) {
            throw std::out_of_range("String position is out of range");
        }
        value = std::string(reinterpret_cast<const char *>(data_.data() + position_), str_size);
        increment(str_size);
    }

    template<>
    inline void Unpacker::unpack_type(std::vector<std::uint8_t> &value) {
        std::size_t bin_size = 0;
        if (read_conditional<FormatConstants::bin32, std::uint32_t>(bin_size)) {
        } else if (read_conditional<FormatConstants::bin16, std::uint16_t>(bin_size)) {
        } else if (read_conditional<FormatConstants::bin8, std::uint8_t>(bin_size)) {
        } else {
            throw std::logic_error("Unexpected value");
        }
        if (position_ + bin_size > data_.size()) {
            throw std::out_of_range("Vector position is out of range");
        }
        auto const *src = reinterpret_cast<std::uint8_t const *>(data_.data() + position_);
        value.assign(src, src + bin_size);
        increment(bin_size);
    }

    template<typename T>
    concept UnpackableObject = requires(T t, Unpacker u)
    {
        t.unpack(u);
    };

    template<PackableObject PackableObject>
    [[nodiscard]] std::vector<std::byte> pack(PackableObject const &obj) {
        Packer packer;
        return obj.pack(packer);
    }

    template<UnpackableObject UnpackableObject>
    [[nodiscard]] UnpackableObject unpack(std::span<const std::byte> const data) {
        Unpacker unpacker(data);
        UnpackableObject obj{};
        obj.unpack(unpacker);
        return obj;
    }
}
