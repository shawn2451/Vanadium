Vanadium is a privacy and security hardened variant of Chromium providing the WebView (used by
other apps to render web content) and standard browser for [GrapheneOS](https://grapheneos.org).
It depends on hardening and compatibility fixes in GrapheneOS rather than reinventing the wheel
inside Vanadium. For example, GrapheneOS already provides a hardened malloc implementation so
there's no need for Vanadium to replace it. Similarly, it can deploy security features causing
breakage on other operating systems due to the ability to fix compatibility problems in the OS.

See [the official build documentation](https://grapheneos.org/build#browser-and-webview) for build
instructions. See the [Vanadium section of our GrapheneOS features overview](https://grapheneos.org/features#vanadium) and the [web browsing section of 
our usage guide](https://grapheneos.org/usage#web-browsing) for more information about Vanadium.


## Downstream: whitelist certificate pinning

Branch `feat/whitelist-cert-pinning` adds optional **leaf + intermediate SPKI pinning (AND)** for configured hosts.

See [`whitelist_pins/README.md`](whitelist_pins/README.md) and patch `patches/0289-Add-whitelist-leaf-and-intermediate-certificate-pinning.patch`.
Also includes a **CA denylist** (China / Malaysia / Singapore / Spain / Russia / Iran +
DigiCert TLS CN intermediates): patches `0290`–`0294`, data in [`denylist/`](denylist/).

**Cert baseline monitor** (alert + block, default ON): patch `0295`,
[`cert_monitor/`](cert_monitor/) — first visit learns leaf + intermediate SPKI; later
CA/leaf swaps LOG a warning and reject TLS. Chromium built-in CT stays on; crt.sh-style
historical CT scans are in the desktop `pin-browser` prototype.

### Browser **and** WebView (already shared)

Vanadium uses **Trichrome**: browser (`app.vanadium.browser`) and WebView
(`app.vanadium.webview`) load the same native library
(`app.vanadium.trichromelibrary`). All of the above hooks live in
`net/socket/ssl_client_socket_impl.cc`, so they apply to **both** products —
there is no separate WebView port.

Build / install both APKs (official GrapheneOS targets):

```bash
chrt -b 0 autoninja -C out/Default \
  trichrome_webview_64_32_apk trichrome_chrome_64_32_apk \
  trichrome_library_64_32_apk vanadium_config_apk
```

Then install library + WebView + browser (order matters; see GrapheneOS build docs).
Set the device WebView provider to Vanadium WebView if you want other apps to use it.

**WebView caveats**

| Topic | Behaviour |
|-------|-----------|
| Denylist / whitelist pins / cert baseline block | Same as browser (shared `net/`) |
| Cert baseline storage | **Per WebView process** (each host app’s renderer process) |
| “Alert” UX | `LOG` only — host apps do not get a Vanadium popup |
| crt.sh CT scan | Not in native WebView (pin-browser only) |
