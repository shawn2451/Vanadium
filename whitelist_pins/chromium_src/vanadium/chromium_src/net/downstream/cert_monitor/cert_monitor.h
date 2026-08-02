// Copyright 2026
// Use of this source code is governed by a GPL-2.0-only style license that can
// be found in the LICENSE file.

#ifndef VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_CERT_MONITOR_CERT_MONITOR_H_
#define VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_CERT_MONITOR_CERT_MONITOR_H_

#include <string_view>
#include <vector>

#include "net/base/hash_value.h"

namespace vanadium {
namespace cert_monitor {

// First successful sighting of |host| establishes a process-local baseline
// (leaf SPKI + issuer/intermediate SPKI). Later mismatches LOG a warning
// (alert) and, when compile-time kBlockOnChange is true, return false so the
// TLS connection is rejected.
//
// This covers live CA/leaf swaps (MITM re-sign, unexpected issuer). Chromium's
// built-in Certificate Transparency enforcement remains separate; historical
// crt.sh-style CT scans live in the pin-browser prototype.
bool CheckCertBaseline(
    std::string_view host,
    const std::vector<net::SHA256HashValue>& public_key_hashes);

}  // namespace cert_monitor
}  // namespace vanadium

#endif  // VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_CERT_MONITOR_CERT_MONITOR_H_
