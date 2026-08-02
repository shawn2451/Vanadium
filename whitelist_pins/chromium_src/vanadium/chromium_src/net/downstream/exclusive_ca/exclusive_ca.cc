// Copyright 2026
// Use of this source code is governed by a GPL-2.0-only style license that can
// be found in the LICENSE file.

#include "vanadium/chromium_src/net/downstream/exclusive_ca/exclusive_ca.h"

#include <cstring>

#include "vanadium/chromium_src/net/downstream/exclusive_ca/exclusive_ca_data.inc"

namespace vanadium {
namespace exclusive_ca {

bool CheckExclusiveCaAllowlist(
    const std::vector<net::SHA256HashValue>& public_key_hashes) {
  if (!data::kEnabled) {
    return true;
  }
  if (data::kSpkiCount == 0) {
    return false;
  }
  for (const net::SHA256HashValue& hash : public_key_hashes) {
    for (size_t i = 0; i < data::kSpkiCount; ++i) {
      if (std::memcmp(hash.data(), data::kAllowedSpkis[i], 32) == 0) {
        return true;
      }
    }
  }
  return false;
}

}  // namespace exclusive_ca
}  // namespace vanadium
