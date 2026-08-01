// Copyright 2026
// Use of this source code is governed by a GPL-2.0-only style license that can
// be found in the LICENSE file.

#include "vanadium/chromium_src/net/downstream/whitelist_pins/whitelist_pins.h"

#include <cstring>
#include <string>

#include "base/strings/string_util.h"
#include "vanadium/chromium_src/net/downstream/whitelist_pins/whitelist_pins_data.inc"

namespace vanadium {
namespace whitelist_pins {
namespace {

bool HostMatches(std::string_view host,
                 std::string_view pattern,
                 bool include_subdomains) {
  if (host == pattern) {
    return true;
  }
  if (!include_subdomains) {
    return false;
  }
  return base::EndsWith(host, std::string(".") + std::string(pattern),
                        base::CompareCase::SENSITIVE);
}

bool HashInList(const net::SHA256HashValue& hash,
                const uint8_t (*pins)[32],
                size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (std::memcmp(hash.data(), pins[i], 32) == 0) {
      return true;
    }
  }
  return false;
}

const data::HostPinEntry* FindEntry(std::string_view host) {
  for (size_t i = 0; i < data::kHostCount; ++i) {
    const data::HostPinEntry& entry = data::kHosts[i];
    if (HostMatches(host, entry.host, entry.include_subdomains)) {
      return &entry;
    }
  }
  return nullptr;
}

}  // namespace

bool CheckWhitelistPins(
    std::string_view host,
    const std::vector<net::SHA256HashValue>& public_key_hashes) {
  const std::string normalized = base::ToLowerASCII(host);
  const data::HostPinEntry* entry = FindEntry(normalized);
  if (!entry) {
    return true;
  }

  // public_key_hashes is ordered leaf → root.
  if (public_key_hashes.empty()) {
    return false;
  }

  if (!HashInList(public_key_hashes[0], entry->leaf_pins, entry->leaf_count)) {
    return false;
  }

  bool intermediate_ok = false;
  for (size_t i = 1; i < public_key_hashes.size(); ++i) {
    if (HashInList(public_key_hashes[i], entry->intermediate_pins,
                   entry->intermediate_count)) {
      intermediate_ok = true;
      break;
    }
  }
  return intermediate_ok;
}

}  // namespace whitelist_pins
}  // namespace vanadium
