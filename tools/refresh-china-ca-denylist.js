#!/usr/bin/env node
'use strict';

/**
 * Rescan Mozilla CCADB Included CA list and regenerate
 * config/denylist/china-related-cas.json
 */

const fs = require('fs');
const path = require('path');
const https = require('https');
const crypto = require('crypto');
const { execFileSync } = require('child_process');
const { tmpdir } = require('os');

const URL =
  'https://ccadb.my.salesforce-sites.com/mozilla/IncludedCACertificateReportPEMCSV';
const OUT = path.join(__dirname, '..', 'denylist', 'china-related-cas.json');

const OWNER_RE =
  /BJCA|BEIJING CERTIFICATE|CFCA|China Financial|GDCA|Global Digital Cybersecurity|SHECA|Shanghai Electronic|iTrusChina|vTrus|TrustAsia|Hongkong Post|Certizen|CNNIC|WoSign|StartCom/i;

function fetchText(url) {
  return new Promise((resolve, reject) => {
    https
      .get(url, (res) => {
        if (res.statusCode && res.statusCode >= 300 && res.statusCode < 400 && res.headers.location) {
          fetchText(res.headers.location).then(resolve, reject);
          return;
        }
        if (res.statusCode !== 200) {
          reject(new Error(`HTTP ${res.statusCode}`));
          return;
        }
        const chunks = [];
        res.on('data', (c) => chunks.push(c));
        res.on('end', () => resolve(Buffer.concat(chunks).toString('utf8')));
      })
      .on('error', reject);
  });
}

function parseCsv(text) {
  const rows = [];
  let i = 0;
  let field = '';
  let row = [];
  let inQuotes = false;
  while (i < text.length) {
    const ch = text[i];
    if (inQuotes) {
      if (ch === '"') {
        if (text[i + 1] === '"') {
          field += '"';
          i += 2;
          continue;
        }
        inQuotes = false;
        i++;
        continue;
      }
      field += ch;
      i++;
      continue;
    }
    if (ch === '"') {
      inQuotes = true;
      i++;
      continue;
    }
    if (ch === ',') {
      row.push(field);
      field = '';
      i++;
      continue;
    }
    if (ch === '\n') {
      row.push(field);
      rows.push(row);
      row = [];
      field = '';
      i++;
      continue;
    }
    if (ch === '\r') {
      i++;
      continue;
    }
    field += ch;
    i++;
  }
  if (field.length || row.length) {
    row.push(field);
    rows.push(row);
  }
  const header = rows[0];
  return rows.slice(1).map((r) => {
    const obj = {};
    header.forEach((h, idx) => {
      obj[h] = r[idx] ?? '';
    });
    return obj;
  });
}

function normalizeFp(s) {
  return String(s || '')
    .toLowerCase()
    .replace(/[^0-9a-f]/g, '');
}

function spkiFromPemInfo(pemInfo) {
  if (!pemInfo) return null;
  const text = pemInfo.trim().replace(/^'|'$/g, '').replace(/''/g, "'");
  const m = text.match(
    /-----BEGIN CERTIFICATE-----([\s\S]*?)-----END CERTIFICATE-----/
  );
  if (!m) return null;
  const pem =
    '-----BEGIN CERTIFICATE-----\n' +
    m[1].trim() +
    '\n-----END CERTIFICATE-----\n';
  const tmp = path.join(
    tmpdir(),
    `ca-${crypto.randomBytes(8).toString('hex')}.pem`
  );
  fs.writeFileSync(tmp, pem);
  try {
    const pub = execFileSync('openssl', ['x509', '-in', tmp, '-pubkey', '-noout'], {
      encoding: 'utf8',
    });
    const body = pub
      .split('\n')
      .filter((l) => !l.includes('PUBLIC KEY'))
      .join('');
    const der = Buffer.from(body, 'base64');
    return crypto.createHash('sha256').update(der).digest('hex');
  } finally {
    fs.unlinkSync(tmp);
  }
}

async function main() {
  console.log('Fetching', URL);
  const csvText = await fetchText(URL);
  const rows = parseCsv(csvText);
  const roots = [];
  const seen = new Set();

  for (const row of rows) {
    const geo = row['Geographic Focus'] || '';
    const owner = row.Owner || '';
    const org = row['Certificate Issuer Organization'] || '';
    const name = row['Common Name or Certificate Name'] || '';
    const china =
      /china/i.test(geo) || OWNER_RE.test(`${owner} ${org} ${name}`);
    if (!china) continue;
    if (/^\s*USA\s*$/i.test(geo) && !OWNER_RE.test(`${owner} ${org} ${name}`)) {
      continue;
    }
    const certSha = normalizeFp(row['SHA-256 Fingerprint']);
    if (!certSha || seen.has(certSha)) continue;
    seen.add(certSha);
    roots.push({
      common_name: name,
      owner,
      organization: org,
      geographic_focus: geo,
      trust_bits: row['Trust Bits'] || '',
      cert_sha256: certSha,
      spki_sha256: spkiFromPemInfo(row['PEM Info']),
      company_website: row['Company Website'] || '',
      test_website_valid: row['Test Website - Valid'] || '',
    });
  }

  const out = {
    id: 'china-related-cas',
    title: 'China-related CAs (Mozilla Included roots)',
    generated_at: new Date().toISOString(),
    source: URL,
    selection_rule:
      'Geographic Focus contains China OR owner/org matches known PRC/HK CA operators in Mozilla Included list',
    notes: [
      'Scanned from Mozilla CCADB Included CA Certificate Report — currently trusted roots, not C=XX heuristics.',
      'Hongkong Post included because Geographic Focus is Hong Kong (SAR), China.',
      'Match any chain cert SHA-256 or SPKI SHA-256 against this list to deny.',
    ],
    enabled_by_default: true,
    match: ['cert_sha256', 'spki_sha256'],
    roots,
  };

  fs.mkdirSync(path.dirname(OUT), { recursive: true });
  fs.writeFileSync(OUT, JSON.stringify(out, null, 2) + '\n');
  console.log(`Wrote ${roots.length} roots -> ${OUT}`);
  for (const r of roots) {
    console.log(`- ${r.common_name} (${r.geographic_focus})`);
  }
}

main().catch((e) => {
  console.error(e);
  process.exit(1);
});
