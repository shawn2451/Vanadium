# Whitelist leaf + intermediate pinning (Vanadium downstream)

GrapheneOS [Vanadium](https://github.com/GrapheneOS/Vanadium) does not ship user-configurable certificate pinning. This branch adds it as a **downstream Chromium patch**, applied the same way as every other Vanadium patch (`git am ../patches/*.patch`).

## Behaviour

Global (all hosts), before whitelist pins:

0. **CA denylist** — if any chain SPKI matches CN / MY / SG / ES / RU / IR packs **or** DigiCert TLS CN intermediates, deny  
   (`denylist/*.json`, patches `0290`–`0294`)
0b. **Cert baseline monitor** (default ON) — first sighting learns leaf + intermediate SPKI; later mismatches LOG + reject  
   (`cert_monitor/`, patch `0295`; alert **and** block)

For each host in `policy.json`:

1. Chromium/Vanadium default certificate verification must still pass
2. Presented **leaf SPKI** must be in `leafSpkiSha256` (backup pins allowed)
3. Some **non-leaf** chain SPKI must be in `intermediateSpkiSha256`
4. (2) and (3) are **AND** — both required
5. Hosts not listed are unaffected (except denylist above)

This hooks `net/socket/ssl_client_socket_impl.cc` (same place Chromium runs HPKP/static PKP checks).

**WebView:** Trichrome shares that `net/` code with `trichrome_webview_*`.
Building/installing Vanadium WebView is enough — no duplicate patch for WebView.

**Realtime:** patch `0296` sets SSL session cache size to 0 so policy checks are
not skipped via TLS session tickets (see `realtime_tls/README.md`).

## Build (official Vanadium flow)

Follow https://grapheneos.org/build#browser-and-webview with this fork/branch instead of upstream Vanadium:

```bash
git clone <this-repo> Vanadium
cd Vanadium
# checkout this branch, then:
gclient sync --nohooks
cd src
git fetch --tags
git checkout 151.0.7922.71   # match args.gn android_default_version_name
git am --whitespace=nowarn --keep-non-patch ../patches/*.patch
gclient sync -D --with_branch_heads --with_tags --jobs 32
gn args out/Default   # paste ../args.gn + trichrome_certdigest
chrt -b 0 autoninja -C out/Default \
  trichrome_webview_64_32_apk trichrome_chrome_64_32_apk \
  trichrome_library_64_32_apk vanadium_config_apk
```

Requirements (from GrapheneOS): Linux x86_64, **32GiB+ RAM**, large disk; Vanadium links with LTO+CFI.

## Update pins

```bash
python3 tools/fetch_whitelist_spki.py example.com
# merge printed JSON into whitelist_pins/policy.json
python3 tools/gen_whitelist_pins_inc.py \
  whitelist_pins/policy.json \
  > patches_src/vanadium/chromium_src/net/downstream/whitelist_pins/whitelist_pins_data.inc
# then regenerate patch 0289 if you keep a chromium checkout, or edit the .inc inside the patch
```

## Note on the earlier Electron `pin-browser`

`../pin-browser` is only a desktop prototype. **This Vanadium patch is the real GrapheneOS-browser-based approach.**
