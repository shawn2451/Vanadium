// Copyright 2026
// Use of this source code is governed by a GPL-2.0-only style license that can
// be found in the LICENSE file.

package org.chromium.chrome.browser.privacy.settings;

import org.json.JSONException;
import org.json.JSONObject;

import org.chromium.base.Log;
import org.chromium.base.PathUtils;
import org.chromium.build.annotations.NullMarked;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;

/**
 * Persists cert-policy runtime toggles for the hardened side-by-side Vanadium
 * build. Native net/ code reads the same JSON via DIR_ANDROID_APP_DATA.
 */
@NullMarked
final class CertPolicyFlagsStore {
    private static final String TAG = "CertPolicyFlags";
    private static final String FILE_NAME = "cert_policy_flags.json";

    static final String KEY_DENYLIST = "denylistEnabled";
    static final String KEY_WHITELIST_PINS = "whitelistPinsEnabled";
    static final String KEY_CERT_MONITOR = "certMonitorEnabled";
    static final String KEY_BLOCK_ON_CHANGE = "certMonitorBlockOnChange";

    private CertPolicyFlagsStore() {}

    static File flagsFile() {
        return new File(PathUtils.getDataDirectory(), FILE_NAME);
    }

    static JSONObject load() {
        File f = flagsFile();
        if (!f.exists()) {
            return defaults();
        }
        try (FileInputStream in = new FileInputStream(f);
                ByteArrayOutputStream out = new ByteArrayOutputStream()) {
            byte[] buf = new byte[4096];
            int n;
            while ((n = in.read(buf)) >= 0) {
                out.write(buf, 0, n);
            }
            return new JSONObject(out.toString(StandardCharsets.UTF_8.name()));
        } catch (IOException | JSONException e) {
            Log.w(TAG, "load failed, using defaults", e);
            return defaults();
        }
    }

    static void save(JSONObject json) {
        File f = flagsFile();
        File parent = f.getParentFile();
        if (parent != null && !parent.exists()) {
            parent.mkdirs();
        }
        try (FileOutputStream out = new FileOutputStream(f)) {
            out.write(json.toString(2).getBytes(StandardCharsets.UTF_8));
            out.write('\n');
        } catch (IOException | JSONException e) {
            Log.e(TAG, "save failed", e);
        }
    }

    static boolean get(String key, boolean defaultValue) {
        return load().optBoolean(key, defaultValue);
    }

    static void set(String key, boolean value) {
        JSONObject o = load();
        try {
            o.put(key, value);
            save(o);
        } catch (JSONException e) {
            Log.e(TAG, "set failed", e);
        }
    }

    private static JSONObject defaults() {
        JSONObject o = new JSONObject();
        try {
            o.put(KEY_DENYLIST, true);
            o.put(KEY_WHITELIST_PINS, true);
            o.put(KEY_CERT_MONITOR, true);
            o.put(KEY_BLOCK_ON_CHANGE, true);
        } catch (JSONException e) {
            // ignore
        }
        return o;
    }
}
