#pragma once

#include <bit>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>

/**
 * Big-endian integer access for wire formats.
 *
 * MoldUDP64 and ITCH 5.0 put every integer on the wire most significant byte first, at
 * offsets that are not aligned. These functions copy the bytes with memcpy and swap them on
 * a little-endian host, so they work at any address. Casting the buffer to an integer or
 * struct pointer does not: it breaks alignment and aliasing rules.
 *
 * None of them checks bounds. The caller guarantees that `p` points at as many readable
 * (load) or writable (store) bytes as the function's width: 2, 4, 6 or 8.
 */
namespace gapless {

namespace detail {

static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big,
              "mixed-endian hosts are not supported");

template<std::unsigned_integral T>
[[nodiscard]] inline T load_be(const std::byte* p) noexcept {
    T value;
    std::memcpy(&value, p, sizeof(T));
    return std::endian::native == std::endian::little ? std::byteswap(value) : value;
}

template<std::unsigned_integral T>
inline void store_be(std::byte* p, T value) noexcept {
    if (std::endian::native == std::endian::little) {
        value = std::byteswap(value);
    }
    std::memcpy(p, &value, sizeof(T));
}

} // namespace detail

[[nodiscard]] inline std::uint16_t load_be16(const std::byte* p) noexcept {
    return detail::load_be<std::uint16_t>(p);
}

[[nodiscard]] inline std::uint32_t load_be32(const std::byte* p) noexcept {
    return detail::load_be<std::uint32_t>(p);
}

[[nodiscard]] inline std::uint64_t load_be64(const std::byte* p) noexcept {
    return detail::load_be<std::uint64_t>(p);
}

/// Reads a 6-byte integer, the width of an ITCH timestamp.
[[nodiscard]] inline std::uint64_t load_be48(const std::byte* p) noexcept {
    // Big-endian, so the six bytes are the low end of an 8-byte value: pad the front.
    std::byte padded[8]{};
    std::memcpy(padded + 2, p, 6);
    return load_be64(padded);
}

inline void store_be16(std::byte* p, std::uint16_t value) noexcept {
    detail::store_be(p, value);
}

inline void store_be32(std::byte* p, std::uint32_t value) noexcept {
    detail::store_be(p, value);
}

inline void store_be64(std::byte* p, std::uint64_t value) noexcept {
    detail::store_be(p, value);
}

/// Writes the low 6 bytes of `value`, which must fit in 48 bits.
inline void store_be48(std::byte* p, std::uint64_t value) noexcept {
    assert((value >> 48) == 0);
    std::byte padded[8];
    store_be64(padded, value);
    std::memcpy(p, padded + 2, 6);
}

} // namespace gapless
