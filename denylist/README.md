# Country-related CA denylist (Vanadium downstream)

Reject TLS chains whose SPKI set intersects compiled denylists for:

| Country | File | Public-trust status (2026-08) |
|---------|------|-------------------------------|
| China | `china-related-cas.json` | Mozilla Included (BJCA/CFCA/…) |
| Malaysia | `malaysia-related-cas.json` | Microsoft CCADB (PosDigicert, TM Applied; Disabled) |
| Singapore | `singapore-related-cas.json` | Microsoft Included (Netrust Root CA 2) |
| Spain | `spain-related-cas.json` | Mozilla Included (FNMT / ACCV / ANF / Firmaprofesional / Izenpe) |
| Russia | `russia-related-cas.json` | empty (no Mozilla/MS Included roots) |
| Iran | `iran-related-cas.json` | empty (no Mozilla/MS Included roots) |

Enforcement lives in patch `0290` (hook) + `0291` (multi-country data / checker).

## Regenerate compiled hashes

```bash
# optional: refresh China list from Mozilla CSV
node tools/refresh-china-ca-denylist.js

python3 tools/gen_country_denylist_inc.py denylist \
  > whitelist_pins/chromium_src/vanadium/chromium_src/net/downstream/china_denylist/china_denylist_data.inc
# then regenerate patches/0291-*.patch (or rebuild chromium_src into the patch)
```

## Toggle a country off

Set `"enabled_by_default": false` in that country's JSON and regenerate the `.inc`.

## Empty lists

Russia / Iran remain empty until a curated national/operator root is added with both
`cert_sha256` and `spki_sha256`. Empty packs are no-ops at runtime.
