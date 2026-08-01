// Copyright 2026
// Use of this source code is governed by a GPL-2.0-only style license that can
// be found in the LICENSE file.

#include "vanadium/chromium_src/net/downstream/china_denylist/china_denylist.h"

#include <cstring>

#include "vanadium/chromium_src/net/downstream/china_denylist/china_denylist_data.inc"

namespace vanadium {
namespace china_denylist {

bool CheckChinaRelatedCaDenylist(
    const std::vector<net::SHA256HashValue>& public_key_hashes) {
  if (!data::kEnabled) {
    return true;
  }
  for (const net::SHA256HashValue& hash : public_key_hashes) {
    for (size_t i = 0; i < data::kSpkiCount; ++i) {
      if (std::memcmp(hash.data(), data::kDeniedSpkis[i], 32) == 0) {
        return false;
      }
    }
  }
  return true;
}

}  // namespace china_denylist
}  // namespace vanadium
