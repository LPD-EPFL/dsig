#pragma once

#include <cstddef>

#include "../../buffer.hpp"
#include "../../crypto.hpp"
#include "../../message.hpp"
#include "../types.hpp"
#include "../sig-switch.hpp"

namespace dory::ubft::certifier::internal {

template <typename CryptoScheme=Crypto::Dalek>
class ShareMessage : public Message {
  using Message::Message;
  using Signature = typename CryptoScheme::Signature;

 public:
  struct BufferLayout {
    Index index;
    Signature signature;
  };

  // static_assert(sizeof(BufferLayout) ==
  //                   sizeof(Index) + sizeof(Signature),
  //               "The Header struct is not packed. Use "
  //               "`__attribute__((__packed__))` to pack it");

  auto static constexpr BufferSize = sizeof(BufferLayout);

  static std::variant<std::invalid_argument, ShareMessage> tryFrom(
      Buffer &&buffer) {
    if (buffer.size() != BufferSize) {
      return std::invalid_argument("Buffer is smaller than BufferSize.");
    }
    return ShareMessage(std::move(buffer));
  }

  Index msgIndex() const {
    return reinterpret_cast<BufferLayout const *>(rawBuffer().data())->index;
  }

  Signature const &signature() const {
    return reinterpret_cast<BufferLayout const *>(rawBuffer().data())
        ->signature;
  }
};

}  // namespace dory::ubft::certifier::internal
