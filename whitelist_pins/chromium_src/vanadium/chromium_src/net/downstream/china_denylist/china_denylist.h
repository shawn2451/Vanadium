// Copyright 2026
// Use of this source code is governed by a GPL-2.0-only style license that can
// be found in the LICENSE file.

#ifndef VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_CHINA_DENYLIST_CHINA_DENYLIST_H_
#define VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_CHINA_DENYLIST_CHINA_DENYLIST_H_

#include <vector>

#include "net/base/hash_value.h"

namespace vanadium {
namespace china_denylist {

// Returns false if any SPKI in |public_key_hashes| (leaf→root) matches a
// country-related CA denylist entry (China / Malaysia / Singapore / Russia /
// Iran), compiled from CCADB / operator roots.
//
// Historical name kept for patch stability; checks all country packs.
bool CheckChinaRelatedCaDenylist(
    const std::vector<net::SHA256HashValue>& public_key_hashes);

}  // namespace china_denylist
}  // namespace vanadium

#endif  // VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_CHINA_DENYLIST_CHINA_DENYLIST_H_
