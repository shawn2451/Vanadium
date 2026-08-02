# Cert baseline monitor (Vanadium downstream)

Replaces the former exclusive MITM CA mode. Patch `0295`.

## Behaviour

On each successful TLS verification path (same hook as denylist / whitelist pins):

1. **First sighting** of a host → establish process-local baseline  
   (leaf SPKI + issuing intermediate SPKI)
2. **Later sightings** → compare against baseline  
   - leaf SPKI change → `LOG(WARNING)` alert  
   - intermediate / issuer SPKI change → `LOG(WARNING)` alert  
3. When `kBlockOnChange` is true (default) → also reject the handshake

Compile-time toggles in `cert_monitor_data.inc`:

| Flag | Default | Meaning |
|------|---------|---------|
| `kEnabled` | `true` | Master switch |
| `kBlockOnChange` | `true` | Alert **and** disconnect |
| `kAlertOnLeafChange` | `true` | Watch leaf SPKI |
| `kAlertOnCaChange` | `true` | Watch intermediate SPKI |

## CT

- **Chromium built-in CT** still applies to publicly trusted certs (unchanged).
- **crt.sh-style historical CT issuer scans** are implemented in the desktop
  `pin-browser` prototype (`ctLogAlertEnabled`). Doing live HTTP CT queries from
  `net/socket` is intentionally out of scope for this native hook.

## Browser + WebView

Same binary path: Trichrome library is used by both `trichrome_chrome_*` and
`trichrome_webview_*`. Cert monitor runs for in-app WebView HTTPS the same way
as the Vanadium browser. Baselines are still process-local (isolated per app
WebView process).

## Limits

- Baselines are **process-local** (cleared on browser / WebView process restart
  → first visit after restart re-baselines).
- Same-key leaf renewals are not blocked (leaf SPKI unchanged).
- No in-app popup for WebView hosts — only `LOG` + TLS reject when blocking.
