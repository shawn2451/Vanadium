# China-related CA denylist (Vanadium downstream)

Compiled from Mozilla CCADB **Included** roots whose Geographic Focus / operator is China-related (BJCA, CFCA, GDCA, SHECA, iTrusChina/vTrus, TrustAsia, Hongkong Post, …).

At TLS verify time, if any SPKI in the chain (leaf→root) matches this list, the connection is rejected.

## Refresh from CCADB

```bash
# requires node + openssl
node tools/refresh-china-ca-denylist.js
python3 tools/gen_china_denylist_inc.py denylist/china-related-cas.json \
  > whitelist_pins/chromium_src/vanadium/chromium_src/net/downstream/china_denylist/china_denylist_data.inc
# then regenerate patches/0290-*.patch (or rebuild chromium_src into the patch)
```

## Disable

Set `"enabled_by_default": false` in `china-related-cas.json` and regenerate the `.inc`.
