// Compile-time toggles for vanadium::cert_monitor.
#include <stddef.h>
#include <stdint.h>

namespace vanadium {
namespace cert_monitor {
namespace data {

// Master switch. When false, CheckCertBaseline always returns true.
constexpr bool kEnabled = true;

// When true, baseline mismatches reject the TLS handshake (in addition to LOG).
constexpr bool kBlockOnChange = true;

// Alert (and optionally block) when leaf SPKI changes.
constexpr bool kAlertOnLeafChange = true;

// Alert (and optionally block) when intermediate / issuer SPKI changes.
constexpr bool kAlertOnCaChange = true;

}  // namespace data
}  // namespace cert_monitor
}  // namespace vanadium
