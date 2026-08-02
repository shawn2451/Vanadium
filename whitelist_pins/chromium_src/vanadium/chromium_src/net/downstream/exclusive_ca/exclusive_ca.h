// Copyright 2026
// Use of this source code is governed by a GPL-2.0-only style license that can
// be found in the LICENSE file.

#ifndef VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_EXCLUSIVE_CA_EXCLUSIVE_CA_H_
#define VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_EXCLUSIVE_CA_EXCLUSIVE_CA_H_

#include <vector>

#include "net/base/hash_value.h"

namespace vanadium {
namespace exclusive_ca {

// When exclusive mode is enabled (compile-time kEnabled), returns false unless
// some SPKI in |public_key_hashes| matches the allowlisted MITM CA.
// When disabled, always returns true.
bool CheckExclusiveCaAllowlist(
    const std::vector<net::SHA256HashValue>& public_key_hashes);

}  // namespace exclusive_ca
}  // namespace vanadium

#endif  // VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_EXCLUSIVE_CA_EXCLUSIVE_CA_H_
