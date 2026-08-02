// Copyright 2026
// Use of this source code is governed by a GPL-2.0-only style license that can
// be found in the LICENSE file.

#include "vanadium/chromium_src/net/downstream/cert_policy_flags/cert_policy_flags.h"

#include <optional>
#include <string>

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/json/json_reader.h"
#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/path_service.h"
#include "base/synchronization/lock.h"
#include "base/values.h"
#include "build/build_config.h"

namespace vanadium {
namespace cert_policy_flags {
namespace {

constexpr char kFlagsFileName[] = "cert_policy_flags.json";

base::Lock& GetLock() {
  static base::NoDestructor<base::Lock> lock;
  return *lock;
}

std::string& GetOverridePath() {
  static base::NoDestructor<std::string> path;
  return *path;
}

struct Cache {
  Flags flags;
  base::Time last_mtime;
  bool loaded = false;
};

Cache& GetCache() {
  static base::NoDestructor<Cache> cache;
  return *cache;
}

base::FilePath ResolveFlagsPathUnlocked() {
  if (!GetOverridePath().empty()) {
    return base::FilePath(GetOverridePath());
  }

  base::FilePath dir;
#if BUILDFLAG(IS_ANDROID)
  if (base::PathService::Get(base::DIR_ANDROID_APP_DATA, &dir)) {
    return dir.AppendASCII(kFlagsFileName);
  }
#endif
  if (base::PathService::Get(base::DIR_HOME, &dir)) {
    return dir.AppendASCII(kFlagsFileName);
  }
  return base::FilePath();
}

bool ReadBool(const base::Value::Dict& dict, const char* key, bool default_val) {
  if (const std::optional<bool> v = dict.FindBool(key)) {
    return *v;
  }
  return default_val;
}

Flags ParseFlags(const std::string& json) {
  Flags flags;
  auto parsed = base::JSONReader::Read(json, base::JSON_PARSE_RFC);
  if (!parsed || !parsed->is_dict()) {
    LOG(WARNING) << "vanadium cert_policy_flags: parse failed, using defaults";
    return flags;
  }
  const base::Value::Dict& dict = parsed->GetDict();
  flags.denylist_enabled = ReadBool(dict, "denylistEnabled", true);
  flags.whitelist_pins_enabled = ReadBool(dict, "whitelistPinsEnabled", true);
  flags.cert_monitor_enabled = ReadBool(dict, "certMonitorEnabled", true);
  flags.cert_monitor_block_on_change =
      ReadBool(dict, "certMonitorBlockOnChange", true);
  return flags;
}

Flags LoadFromDiskUnlocked() {
  const base::FilePath path = ResolveFlagsPathUnlocked();
  if (path.empty()) {
    return Flags();
  }

  base::File::Info info;
  if (!base::GetFileInfo(path, &info)) {
    return Flags();
  }

  Cache& cache = GetCache();
  if (cache.loaded && cache.last_mtime == info.last_modified) {
    return cache.flags;
  }

  std::string json;
  if (!base::ReadFileToString(path, &json)) {
    return Flags();
  }

  Flags flags = ParseFlags(json);
  cache.flags = flags;
  cache.last_mtime = info.last_modified;
  cache.loaded = true;
  return flags;
}

}  // namespace

void SetFlagsFilePath(const std::string& absolute_path) {
  base::AutoLock lock(GetLock());
  GetOverridePath() = absolute_path;
  GetCache().loaded = false;
}

Flags GetFlags() {
  base::AutoLock lock(GetLock());
  return LoadFromDiskUnlocked();
}

}  // namespace cert_policy_flags
}  // namespace vanadium
