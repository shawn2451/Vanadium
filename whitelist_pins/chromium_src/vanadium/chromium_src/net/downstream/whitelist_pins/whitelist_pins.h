// Copyright 2026
// Use of this source code is governed by a GPL-2.0-only style license that can
// be found in the LICENSE file.

#ifndef VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_WHITELIST_PINS_WHITELIST_PINS_H_
#define VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_WHITELIST_PINS_WHITELIST_PINS_H_

#include <string_view>
#include <vector>

#include "net/base/hash_value.h"

namespace vanadium {
namespace whitelist_pins {

// Returns true when |host| is not on the whitelist, or when leaf SPKI and at
// least one intermediate/root SPKI both match the configured pin sets (AND).
bool CheckWhitelistPins(
    std::string_view host,
    const std::vector<net::SHA256HashValue>& public_key_hashes);

}  // namespace whitelist_pins
}  // namespace vanadium

#endif  // VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_WHITELIST_PINS_WHITELIST_PINS_H_
