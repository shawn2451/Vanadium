# Hardened side-by-side Vanadium (旁路包)

Install **alongside** official GrapheneOS Vanadium. Does not replace system
WebView or the stock browser, so official apps keep receiving GrapheneOS updates.

## Packages (`args.gn`)

| Component | Package |
|-----------|---------|
| Browser | `app.vanadium.hardened.browser` |
| Library | `app.vanadium.hardened.trichromelibrary` |
| Config | `app.vanadium.hardened.config` |
| WebView | `app.vanadium.hardened.webview` (optional; do **not** set as system WebView) |

Set `trichrome_certdigest` / `config_apk_certdigest` from your keystore:

```bash
keytool -export-cert -alias vanadium -keystore vanadium.keystore | sha256sum
```

## Runtime settings (no rebuild)

After install, open **Hardened Vanadium → Settings → Privacy and security**.

Global toggles (default ON):

1. **CA denylist** — all country packs + DigiCert TLS CN  
2. **Whitelist certificate pins**  
3. **Certificate baseline monitor**  
4. **Block on baseline change**

Stored in app data as `cert_policy_flags.json` (same directory Chromium uses for
`DIR_ANDROID_APP_DATA`). Native TLS hooks re-read on mtime change → next HTTPS
handshake picks up the new policy.

## Build

Same as upstream Vanadium, using this repo’s `args.gn` and patches through `0297`:

```bash
git am --whitespace=nowarn --keep-non-patch ../patches/*.patch
# …
chrt -b 0 autoninja -C out/Default \
  trichrome_chrome_64_32_apk \
  trichrome_library_64_32_apk \
  vanadium_config_apk
# optional: trichrome_webview_64_32_apk — do not replace system WebView
../generate-release out
```

## Install (device already has official Vanadium)

```bash
adb install -r out/Default/apks/release/TrichromeLibrary.apk
adb install -r out/Default/apks/release/VanadiumConfig.apk
adb install -r out/Default/apks/release/TrichromeChrome.apk
```

You should see a **second** browser app. Use it when you want denylist/pin/monitor;
use stock Vanadium for everyday browsing + OS updates.

## Updates

Official Vanadium continues via GrapheneOS. This旁路包 is updated only when you
rebuild against a newer Chromium/Vanadium tag and reinstall with the **same**
keystore.
