#include "gapless/byte_order.h"

#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

using namespace gapless;

namespace {

template<typename... Ints>
constexpr std::array<std::byte, sizeof...(Ints)> bytes(Ints... values) {
    return {static_cast<std::byte>(values)...};
}

constexpr std::uint64_t kMax48 = (std::uint64_t{1} << 48) - 1;

} // namespace

// ── Byte order ───────────────────────────────────────────────────────────────

TEST(ByteOrderTest, LoadsMostSignificantByteFirst) {
    const auto wire = bytes(0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08);

    EXPECT_EQ(load_be16(wire.data()), std::uint16_t{0x0102});
    EXPECT_EQ(load_be32(wire.data()), std::uint32_t{0x01020304});
    EXPECT_EQ(load_be48(wire.data()), std::uint64_t{0x010203040506});
    EXPECT_EQ(load_be64(wire.data()), std::uint64_t{0x0102030405060708});
}

TEST(ByteOrderTest, StoresMostSignificantByteFirst) {
    std::array<std::byte, 2> b16{};
    store_be16(b16.data(), 0x0102);
    EXPECT_EQ(b16, bytes(0x01, 0x02));

    std::array<std::byte, 4> b32{};
    store_be32(b32.data(), 0x01020304);
    EXPECT_EQ(b32, bytes(0x01, 0x02, 0x03, 0x04));

    std::array<std::byte, 6> b48{};
    store_be48(b48.data(), 0x010203040506);
    EXPECT_EQ(b48, bytes(0x01, 0x02, 0x03, 0x04, 0x05, 0x06));

    std::array<std::byte, 8> b64{};
    store_be64(b64.data(), 0x0102030405060708);
    EXPECT_EQ(b64, bytes(0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08));
}

// A set top bit must not turn into a sign extension.
TEST(ByteOrderTest, LoadsAllOnesAsMaximum) {
    const auto wire = bytes(0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

    EXPECT_EQ(load_be16(wire.data()), std::numeric_limits<std::uint16_t>::max());
    EXPECT_EQ(load_be32(wire.data()), std::numeric_limits<std::uint32_t>::max());
    EXPECT_EQ(load_be48(wire.data()), kMax48);
    EXPECT_EQ(load_be64(wire.data()), std::numeric_limits<std::uint64_t>::max());
}

TEST(ByteOrderTest, RoundTripsBoundaryValues) {
    std::array<std::byte, 8> buffer{};

    for (const std::uint16_t v : {std::uint16_t{0}, std::uint16_t{1}, std::uint16_t{0x8000},
                                  std::numeric_limits<std::uint16_t>::max()}) {
        store_be16(buffer.data(), v);
        EXPECT_EQ(load_be16(buffer.data()), v);
    }
    for (const std::uint32_t v : {std::uint32_t{0}, std::uint32_t{1}, std::uint32_t{0x80000000},
                                  std::numeric_limits<std::uint32_t>::max()}) {
        store_be32(buffer.data(), v);
        EXPECT_EQ(load_be32(buffer.data()), v);
    }
    for (const std::uint64_t v :
         {std::uint64_t{0}, std::uint64_t{1}, std::uint64_t{1} << 47, kMax48}) {
        store_be48(buffer.data(), v);
        EXPECT_EQ(load_be48(buffer.data()), v);
    }
    for (const std::uint64_t v : {std::uint64_t{0}, std::uint64_t{1}, std::uint64_t{1} << 63,
                                  std::numeric_limits<std::uint64_t>::max()}) {
        store_be64(buffer.data(), v);
        EXPECT_EQ(load_be64(buffer.data()), v);
    }
}

// An ITCH timestamp is 6 bytes of nanoseconds since midnight. 09:30:00 is the market open.
TEST(ByteOrderTest, LoadsItchTimestamp) {
    const auto wire = bytes(0x1F, 0x1A, 0xCE, 0xD9, 0xF0, 0x00);

    EXPECT_EQ(load_be48(wire.data()), std::uint64_t{34'200'000'000'000});
}

// ── Memory access ────────────────────────────────────────────────────────────
// These pass on any correct implementation; they exist so that ASan and UBSan see every
// function touch memory at unaligned addresses and at the very end of an allocation.

TEST(ByteOrderTest, WorksAtEveryAlignment) {
    std::vector<std::byte> heap(16);

    for (std::size_t offset = 0; offset < 8; ++offset) {
        std::byte* p = heap.data() + offset;

        store_be16(p, 0xA1B2);
        EXPECT_EQ(load_be16(p), std::uint16_t{0xA1B2});
        store_be32(p, 0xA1B2C3D4);
        EXPECT_EQ(load_be32(p), std::uint32_t{0xA1B2C3D4});
        store_be48(p, 0xA1B2C3D4E5F6);
        EXPECT_EQ(load_be48(p), std::uint64_t{0xA1B2C3D4E5F6});
        store_be64(p, 0xA1B2C3D4E5F60718);
        EXPECT_EQ(load_be64(p), std::uint64_t{0xA1B2C3D4E5F60718});
    }
}

// Each buffer is exactly as large as the access, so one byte too many is out of bounds.
TEST(ByteOrderTest, TouchesOnlyItsOwnWidth) {
    std::vector<std::byte> two(2);
    store_be16(two.data(), 0x0102);
    EXPECT_EQ(load_be16(two.data()), std::uint16_t{0x0102});

    std::vector<std::byte> four(4);
    store_be32(four.data(), 0x01020304);
    EXPECT_EQ(load_be32(four.data()), std::uint32_t{0x01020304});

    std::vector<std::byte> six(6);
    store_be48(six.data(), 0x010203040506);
    EXPECT_EQ(load_be48(six.data()), std::uint64_t{0x010203040506});

    std::vector<std::byte> eight(8);
    store_be64(eight.data(), 0x0102030405060708);
    EXPECT_EQ(load_be64(eight.data()), std::uint64_t{0x0102030405060708});
}

TEST(ByteOrderTest, Store48LeavesNeighboursUntouched) {
    auto buffer = bytes(0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE);

    store_be48(buffer.data() + 1, 0x010203040506);

    EXPECT_EQ(buffer, bytes(0xEE, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0xEE));
}
