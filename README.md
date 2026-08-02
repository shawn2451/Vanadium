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
DigiCert TLS CN intermediates): patches `0290`–`0293`, data in [`denylist/`](denylist/).
