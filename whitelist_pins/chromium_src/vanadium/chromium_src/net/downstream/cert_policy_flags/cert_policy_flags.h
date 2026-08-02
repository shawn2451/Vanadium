// Copyright 2026
// Use of this source code is governed by a GPL-2.0-only style license that can
// be found in the LICENSE file.

#ifndef VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_CERT_POLICY_FLAGS_CERT_POLICY_FLAGS_H_
#define VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_CERT_POLICY_FLAGS_CERT_POLICY_FLAGS_H_

#include <string>

namespace vanadium {
namespace cert_policy_flags {

// Runtime toggles for downstream cert policy (denylist / pins / cert monitor).
// Loaded from a JSON file written by the browser settings UI. Defaults are ON.
struct Flags {
  bool denylist_enabled = true;
  bool whitelist_pins_enabled = true;
  bool cert_monitor_enabled = true;
  bool cert_monitor_block_on_change = true;
};

// Optional: absolute path set from Java at startup (preferred on Android).
void SetFlagsFilePath(const std::string& absolute_path);

// Returns current flags (reloads when the flags file mtime changes).
Flags GetFlags();

}  // namespace cert_policy_flags
}  // namespace vanadium

#endif  // VANADIUM_CHROMIUM_SRC_NET_DOWNSTREAM_CERT_POLICY_FLAGS_CERT_POLICY_FLAGS_H_
