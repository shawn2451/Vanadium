# Realtime cert policy (no TLS session cache)

Patch `0296`.

## What already was realtime

Chromium sets `SSL_CTX_set_reverify_on_resume(1)`. Even with TLS session
resumption, the server certificate is re-verified, and Vanadium’s hooks in
`ssl_client_socket_impl` (denylist / whitelist pins / cert_monitor) run again.

## What this patch adds

`SSLClientSessionCache::Config.max_entries = 0` in `HttpNetworkSession`.

- No cached TLS session tickets
- Each **new socket** does a full handshake → cert policy always runs
- Cost: more handshakes / slightly higher latency and CPU

## What can still skip a check

| Mechanism | Effect | Mitigation |
|-----------|--------|------------|
| **HTTP disk/memory cache** | Page served without network → no TLS | Hard refresh / disable HTTP cache when testing; not disabled OS-wide here (too blunt for daily browsing) |
| **Keep-alive socket reuse** | Same TCP+TLS connection reused → no new handshake | Expected; cert cannot silently swap mid-connection without a new handshake |
| **Process-local cert baselines** | First sighting wins until process restart / reset | By design for cert_monitor |

## Testing tip

After installing a new build, force-stop Vanadium / WebView host apps so the
old process (and its in-memory baselines / pooled sockets) is gone.
