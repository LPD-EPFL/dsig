#pragma once

#include <array>
#include <vector>

#include <dory/third-party/blake3/blake3.h>
#include <dory/shared/concepts.hpp>

namespace dory::crypto::hash {
static constexpr size_t Blake3HashLength = BLAKE3_OUT_LEN;

using Blake3Hash = std::array<uint8_t, Blake3HashLength>;
using Blake3HalfHash = std::array<uint8_t, Blake3HashLength / 2>;

// Multi-part

using Blake3Hasher = blake3_hasher;

static inline Blake3Hasher blake3_init() {
  Blake3Hasher state;
  blake3_hasher_init(&state);
  return state;
}

static inline void blake3_update(Blake3Hasher &state,
                                 uint8_t const *const begin,
                                 uint8_t const *const end) {
  blake3_hasher_update(&state, begin, static_cast<size_t>(end - begin));
}

template <typename ContiguousIt,
          concepts::IsRandomIterator<ContiguousIt> = true>
static inline void blake3_update(Blake3Hasher &state, ContiguousIt begin,
                                 ContiguousIt end) {
  blake3_update(state, reinterpret_cast<uint8_t const *const>(&*begin),
                reinterpret_cast<uint8_t const *const>(&*end));
}

static inline void blake3_update(
    Blake3Hasher &state, std::vector<Blake3Hash::value_type> const &message) {
  blake3_update(state, message.begin(), message.end());
}

template <typename T, concepts::IsTrivial<T> = true>
static inline void blake3_update(Blake3Hasher &state, T const &value) {
  auto const *const begin = reinterpret_cast<uint8_t const *const>(&value);
  blake3_update(state, begin, begin + sizeof(T));
}

static inline void blake3_final_there(Blake3Hasher &state, void *const out,
                                      size_t const len) {
  blake3_hasher_finalize(&state, reinterpret_cast<uint8_t*>(out), len);
}

template <typename Hash = Blake3Hash>
static inline Hash blake3_final(Blake3Hasher &state) {
  Hash ret;
  blake3_final_there(state, &ret, sizeof(Hash));
  return ret;
}

// Single-part

template <typename Hash = Blake3Hash>
static inline Hash blake3(uint8_t const *const begin,
                                uint8_t const *const end) {
  auto state = blake3_init();
  blake3_update(state, begin, end);
  return blake3_final<Hash>(state);
}

template <typename Hash = Blake3Hash, typename ContiguousIt,
          concepts::IsRandomIterator<ContiguousIt> = true>
static inline Hash blake3(ContiguousIt begin, ContiguousIt end) {
  return blake3<Hash>(reinterpret_cast<uint8_t const *const>(&*begin),
                      reinterpret_cast<uint8_t const *const>(&*end));
}

template <typename Hash = Blake3Hash>
static inline Hash blake3(
    std::vector<Blake3Hash::value_type> const &message) {
  return blake3<Hash>(message.begin(), message.end());
}

template <typename Hash = Blake3Hash, typename T, concepts::IsTrivial<T> = true>
static inline Hash blake3(T const &value) {
  auto const *const begin = reinterpret_cast<uint8_t const *const>(&value);
  return blake3<Hash>(begin, begin + sizeof(T));
}

}  // namespace dory::crypto::hash
