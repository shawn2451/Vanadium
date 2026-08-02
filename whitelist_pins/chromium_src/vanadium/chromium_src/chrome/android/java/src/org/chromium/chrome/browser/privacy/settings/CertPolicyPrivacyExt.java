// Copyright 2026
// Use of this source code is governed by a GPL-2.0-only style license that can
// be found in the LICENSE file.

package org.chromium.chrome.browser.privacy.settings;

import androidx.preference.Preference;
import androidx.preference.PreferenceFragmentCompat;

import org.chromium.base.ThreadUtils;
import org.chromium.base.settings.SettingsExtUtils;
import org.chromium.build.annotations.NullMarked;
import org.chromium.chrome.R;
import org.chromium.chrome.browser.profiles.Profile;
import org.chromium.components.browser_ui.settings.ChromeSwitchPreference;
import org.chromium.components.browser_ui.settings.SettingsUtils;

/** Wires cert-policy global toggles into the Privacy settings screen. */
@NullMarked
final class CertPolicyPrivacyExt {
    private static final String PREF_DENYLIST = "cert_policy_denylist";
    private static final String PREF_WHITELIST_PINS = "cert_policy_whitelist_pins";
    private static final String PREF_CERT_MONITOR = "cert_policy_cert_monitor";
    private static final String PREF_BLOCK_ON_CHANGE = "cert_policy_block_on_change";

    private CertPolicyPrivacyExt() {}

    static void initializePreferences(PreferenceFragmentCompat prefFragment, Profile profile) {
        ThreadUtils.checkUiThread();
        SettingsUtils.addPreferencesFromResource(prefFragment, R.xml.cert_policy_preferences);

        // Place after other privacy_preferences_ext entries.
        final int order = 20;
        wire(prefFragment, PREF_DENYLIST, CertPolicyFlagsStore.KEY_DENYLIST, true, order);
        wire(prefFragment, PREF_WHITELIST_PINS, CertPolicyFlagsStore.KEY_WHITELIST_PINS, true,
                order + 1);
        wire(prefFragment, PREF_CERT_MONITOR, CertPolicyFlagsStore.KEY_CERT_MONITOR, true,
                order + 2);
        wire(prefFragment, PREF_BLOCK_ON_CHANGE, CertPolicyFlagsStore.KEY_BLOCK_ON_CHANGE, true,
                order + 3);
        updateBlockEnabled(prefFragment);
    }

    static void updatePreferences(PreferenceFragmentCompat prefFragment, Profile profile) {
        ThreadUtils.checkUiThread();
        updateSwitch(prefFragment, PREF_DENYLIST, CertPolicyFlagsStore.KEY_DENYLIST, true);
        updateSwitch(prefFragment, PREF_WHITELIST_PINS, CertPolicyFlagsStore.KEY_WHITELIST_PINS,
                true);
        updateSwitch(prefFragment, PREF_CERT_MONITOR, CertPolicyFlagsStore.KEY_CERT_MONITOR, true);
        updateSwitch(prefFragment, PREF_BLOCK_ON_CHANGE, CertPolicyFlagsStore.KEY_BLOCK_ON_CHANGE,
                true);
        updateBlockEnabled(prefFragment);
    }

    private static void wire(PreferenceFragmentCompat frag, String prefKey, String storeKey,
            boolean defaultValue, int order) {
        ChromeSwitchPreference pref = frag.findPreference(prefKey);
        if (pref == null) return;
        pref.setOrder(order);
        pref.setChecked(CertPolicyFlagsStore.get(storeKey, defaultValue));
        pref.setOnPreferenceChangeListener(
                (Preference p, Object newValue) -> {
                    CertPolicyFlagsStore.set(storeKey, (Boolean) newValue);
                    if (PREF_CERT_MONITOR.equals(prefKey)) {
                        updateBlockEnabled(frag);
                    }
                    return true;
                });
    }

    private static void updateSwitch(PreferenceFragmentCompat frag, String prefKey,
            String storeKey, boolean defaultValue) {
        ChromeSwitchPreference pref = frag.findPreference(prefKey);
        SettingsExtUtils.safelyUpdateSwitchPreference(
                /* switchPref= */ pref,
                /* newSummary= */ null,
                /* newCheckedValue= */ CertPolicyFlagsStore.get(storeKey, defaultValue));
    }

    private static void updateBlockEnabled(PreferenceFragmentCompat frag) {
        ChromeSwitchPreference block = frag.findPreference(PREF_BLOCK_ON_CHANGE);
        if (block == null) return;
        boolean monitorOn =
                CertPolicyFlagsStore.get(CertPolicyFlagsStore.KEY_CERT_MONITOR, true);
        block.setEnabled(monitorOn);
    }
}
