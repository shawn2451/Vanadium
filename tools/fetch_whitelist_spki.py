#!/usr/bin/env python3
"""Fetch leaf + first-intermediate SPKI pins for whitelist_pins/policy.json."""

from __future__ import annotations

import argparse
import base64
import hashlib
import json
import subprocess
import sys


def pem_certs_from_sclient(host: str, port: int) -> list[str]:
    proc = subprocess.run(
        [
            "openssl",
            "s_client",
            "-showcerts",
            "-connect",
            f"{host}:{port}",
            "-servername",
            host,
        ],
        input=b"",
        capture_output=True,
        timeout=30,
        check=False,
    )
    text = proc.stdout.decode("utf-8", "ignore")
    certs: list[str] = []
    cur: list[str] = []
    for line in text.splitlines():
        if "-----BEGIN CERTIFICATE-----" in line:
            cur = [line]
        elif cur:
            cur.append(line)
            if "-----END CERTIFICATE-----" in line:
                certs.append("\n".join(cur) + "\n")
                cur = []
    return certs


def spki_sha256_b64(pem: str) -> str:
    pub = subprocess.run(
        ["openssl", "x509", "-pubkey", "-noout"],
        input=pem.encode(),
        capture_output=True,
        check=True,
    ).stdout.decode()
    body = "".join(pub.splitlines()[1:-1])
    der = base64.b64decode(body)
    digest = hashlib.sha256(der).digest()
    return "sha256/" + base64.b64encode(digest).decode()


def subject(pem: str) -> str:
    return subprocess.run(
        ["openssl", "x509", "-noout", "-subject"],
        input=pem,
        capture_output=True,
        check=True,
        text=True,
    ).stdout.strip()


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("host")
    parser.add_argument("--port", type=int, default=443)
    parser.add_argument("--include-subdomains", action="store_true", default=True)
    args = parser.parse_args()

    certs = pem_certs_from_sclient(args.host, args.port)
    if len(certs) < 2:
        print(f"Need leaf+intermediate, got {len(certs)} certs", file=sys.stderr)
        return 2

    leaf = spki_sha256_b64(certs[0])
    intermediate = spki_sha256_b64(certs[1])
    print(f"leaf: {subject(certs[0])}", file=sys.stderr)
    print(f"intermediate: {subject(certs[1])}", file=sys.stderr)
    print(
        json.dumps(
            {
                "name": args.host,
                "include_subdomains": bool(args.include_subdomains),
                "leafSpkiSha256": [leaf],
                "intermediateSpkiSha256": [intermediate],
            },
            indent=2,
        )
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
