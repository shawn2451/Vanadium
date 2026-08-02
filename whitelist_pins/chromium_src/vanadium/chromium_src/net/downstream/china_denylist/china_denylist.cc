// Copyright 2026
// Use of this source code is governed by a GPL-2.0-only style license that can
// be found in the LICENSE file.

#include "vanadium/chromium_src/net/downstream/china_denylist/china_denylist.h"

#include <cstring>

#include "vanadium/chromium_src/net/downstream/china_denylist/china_denylist_data.inc"

namespace vanadium {
namespace china_denylist {
namespace {

bool MatchesDeniedSpki(const std::vector<net::SHA256HashValue>& public_key_hashes,
                       bool enabled,
                       size_t count,
                       const uint8_t (*denied)[32]) {
  if (!enabled || count == 0) {
    return false;
  }
  for (const net::SHA256HashValue& hash : public_key_hashes) {
    for (size_t i = 0; i < count; ++i) {
      if (std::memcmp(hash.data(), denied[i], 32) == 0) {
        return true;
      }
    }
  }
  return false;
}

}  // namespace

bool CheckChinaRelatedCaDenylist(
    const std::vector<net::SHA256HashValue>& public_key_hashes) {
  if (MatchesDeniedSpki(public_key_hashes, data::china::kEnabled,
                        data::china::kSpkiCount, data::china::kDeniedSpkis) ||
      MatchesDeniedSpki(public_key_hashes, data::malaysia::kEnabled,
                        data::malaysia::kSpkiCount,
                        data::malaysia::kDeniedSpkis) ||
      MatchesDeniedSpki(public_key_hashes, data::singapore::kEnabled,
                        data::singapore::kSpkiCount,
                        data::singapore::kDeniedSpkis) ||
      MatchesDeniedSpki(public_key_hashes, data::spain::kEnabled,
                        data::spain::kSpkiCount, data::spain::kDeniedSpkis) ||
      MatchesDeniedSpki(public_key_hashes, data::russia::kEnabled,
                        data::russia::kSpkiCount, data::russia::kDeniedSpkis) ||
      MatchesDeniedSpki(public_key_hashes, data::iran::kEnabled,
                        data::iran::kSpkiCount, data::iran::kDeniedSpkis) ||
      MatchesDeniedSpki(public_key_hashes, data::digicert_tls_cn::kEnabled,
                        data::digicert_tls_cn::kSpkiCount,
                        data::digicert_tls_cn::kDeniedSpkis)) {
    return false;
  }
  return true;
}

}  // namespace china_denylist
}  // namespace vanadium
