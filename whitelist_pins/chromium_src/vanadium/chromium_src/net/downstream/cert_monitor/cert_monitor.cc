// Copyright 2026
// Use of this source code is governed by a GPL-2.0-only style license that can
// be found in the LICENSE file.

#include "vanadium/chromium_src/net/downstream/cert_monitor/cert_monitor.h"

#include <cstring>
#include <map>
#include <string>

#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/strings/string_util.h"
#include "base/synchronization/lock.h"
#include "vanadium/chromium_src/net/downstream/cert_monitor/cert_monitor_data.inc"

namespace vanadium {
namespace cert_monitor {
namespace {

struct Baseline {
  net::SHA256HashValue leaf{};
  bool has_issuer = false;
  net::SHA256HashValue issuer{};
};

base::Lock& GetLock() {
  static base::NoDestructor<base::Lock> lock;
  return *lock;
}

std::map<std::string, Baseline>& GetBaselines() {
  static base::NoDestructor<std::map<std::string, Baseline>> baselines;
  return *baselines;
}

bool SpkiEqual(const net::SHA256HashValue& a, const net::SHA256HashValue& b) {
  return std::memcmp(a.data(), b.data(), 32) == 0;
}

}  // namespace

bool CheckCertBaseline(
    std::string_view host,
    const std::vector<net::SHA256HashValue>& public_key_hashes) {
  if (!data::kEnabled) {
    return true;
  }
  if (public_key_hashes.empty()) {
    return true;
  }

  const std::string normalized = base::ToLowerASCII(host);
  if (normalized.empty()) {
    return true;
  }

  Baseline current;
  current.leaf = public_key_hashes[0];
  // public_key_hashes is ordered leaf → root. Index 1 is the issuing
  // intermediate when present.
  if (public_key_hashes.size() > 1) {
    current.has_issuer = true;
    current.issuer = public_key_hashes[1];
  }

  base::AutoLock lock(GetLock());
  auto& baselines = GetBaselines();
  auto it = baselines.find(normalized);
  if (it == baselines.end()) {
    baselines[normalized] = current;
    LOG(WARNING) << "vanadium cert_monitor: baseline established for "
                 << normalized;
    return true;
  }

  const Baseline& baseline = it->second;
  bool leaf_changed = false;
  bool ca_changed = false;

  if (data::kAlertOnLeafChange && !SpkiEqual(baseline.leaf, current.leaf)) {
    leaf_changed = true;
    LOG(WARNING) << "vanadium cert_monitor: ALERT leaf_key_changed host="
                 << normalized;
  }

  if (data::kAlertOnCaChange) {
    if (baseline.has_issuer != current.has_issuer) {
      ca_changed = true;
    } else if (baseline.has_issuer &&
               !SpkiEqual(baseline.issuer, current.issuer)) {
      ca_changed = true;
    }
    if (ca_changed) {
      LOG(WARNING) << "vanadium cert_monitor: ALERT intermediate_ca_changed host="
                   << normalized;
    }
  }

  if (!(leaf_changed || ca_changed)) {
    return true;
  }

  if (data::kBlockOnChange) {
    LOG(ERROR) << "vanadium cert_monitor: blocking TLS for " << normalized
               << " due to certificate baseline mismatch";
    return false;
  }
  return true;
}

}  // namespace cert_monitor
}  // namespace vanadium
