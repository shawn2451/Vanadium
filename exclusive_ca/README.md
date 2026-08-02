# Exclusive MITM CA allowlist (Vanadium downstream)

When enabled, TLS is accepted only if the verified chain contains an allowlisted
CA SPKI (your mitmproxy / Charles / personal root). Public DigiCert/Let’s Encrypt
chains fail.

**Default: OFF** (`enabled_by_default: false` in `allowlist.json`).

## Workflow

1. Run mitmproxy / Charles; export its CA PEM.
2. Replace `mitm-root.pem` (do **not** commit private keys).
3. Regenerate hashes into `allowlist.json` / `exclusive_ca_data.inc`.
4. Set `"enabled_by_default": true` (or flip the compile-time `kEnabled`) and rebuild.
5. Point the phone Wi‑Fi proxy at the MITM host; browse with this Vanadium build.

## Regenerate

```bash
python3 tools/gen_exclusive_ca_inc.py exclusive_ca/allowlist.json \
  > whitelist_pins/chromium_src/vanadium/chromium_src/net/downstream/exclusive_ca/exclusive_ca_data.inc
```

Pin-browser has a matching runtime toggle: **仅信任我的 MITM CA（Exclusive）**.
