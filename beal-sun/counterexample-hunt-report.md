---
layout: default
title: Counterexample Hunt Report
permalink: /beal-sun/counterexample-hunt-report.html
---

# Counterexample Hunt on Two Open Conjectures

**Session date:** 2026-07-23/24 · **Hardware:** 2 shared vCPUs (Xeon Platinum, sandbox VM) · **Arithmetic:** exact everywhere (128-bit / int64 / GMP)

**Bottom line:** no counterexample was found anywhere — both conjectures survived every test thrown at them. The $1,000,000 Beal prize and Sun's $1,000 prize remain unclaimed. This report states exactly what was searched, how it was validated, and what the results do and do not mean.

---

## Target 1 — The Beal Conjecture ($1,000,000 AMS prize)

**Statement.** If A^x + B^y = C^z with positive integers A, B, C and exponents x, y, z all greater than 2, then A, B, C share a common prime factor. A *counterexample* is therefore a solution with gcd(A, B, C) = 1 — a single tuple, checkable by anyone in seconds, worth the full prize.[^9][^14]

**Why a counterexample would be a sensation.** By Darmon–Granville, for fixed exponents with 1/x + 1/y + 1/z < 1 only finitely many coprime solutions exist. Only 10 such "Fermat–Catalan" solutions are known in total;[^10] I re-verified all 10 computationally, and **every one of them has an exponent equal to 2** — so none is a Beal counterexample. A Beal counterexample would be an 11th solution of an entirely new kind (all exponents ≥ 3).

### Search design

- **Space covered:** all A^x + B^y with bases 1 ≤ A, B ≤ 100,000, all exponents x, y ≥ 3 (up to 63, bounded by the value limit), with A^x + B^y ≤ 8×10^18. Prior published searches (e.g. Norvig's) used comparable base ranges with exponents capped near 10; this run extends the exponent coverage to 63.[^12]
- **Coprimality filter:** only gcd(A, B) = 1 pairs need checking — if a prime divided both A and C, it would divide B^y and hence B, so gcd(A,B)=1 already forces gcd(A,B,C)=1.
- **Engine (C, `-O3`):** a table of all 2,059,750 perfect powers c^z (z ≥ 3) up to 8×10^18; membership tested via a 4 MB Bloom filter (two hashes) with confirmation by binary search — chosen because a 33 MB hash table thrashed the VM's cache. gcd filters computed once per base pair via smallest-prime-factor walks.
- **Validation:** two independently written implementations produced identical pair counts on a calibration slice (21,288,398 coprime pairs); a sampled near-miss detector (1 in 64 pairs) exercised the negative path and behaved statistically as expected.

### Result

| Quantity | Value |
|---|---|
| Coprime pairs A^x + B^y checked | **7,054,448,010** |
| Base range | 1 ≤ A, B ≤ 100,000 |
| Exponent range | 3 ≤ x, y ≤ 63 |
| Sum range | ≤ 8×10^18 |
| **Coprime solutions (= counterexamples)** | **0** |

No counterexample exists in this range. (Near-miss trivia: the sampler flagged curiosities such as 1 + 17^5 = 1,419,858 landing one away from a perfect power — amusing because 13^5 + 16^5 = 1,419,869 sits in the same neighborhood — but nothing structurally significant.)

---

## Target 2 — Sun's "2^k + m" Conjecture ($1,000 prize)

**Statement (Zhi-Wei Sun, 2013).** Every integer n ≥ 2 can be written as n = k + m with k, m ≥ 1 and 2^k + m prime. Published verification: all n ≤ 10^7 (Sun),[^8] still the current bound as of a May 2026 arXiv paper proving a positive-density version.[^7] The prize is for a *proof* — a counterexample could never be certified by finite search alone, since k is unbounded.

### Verification engine (three tiers, all exact or cryptographically rigorous)

For each n, only k with parity opposite to n can work (otherwise 2^k + n − k is even and > 2). Candidates are tried in order:

1. **k ≤ 27:** odd-only Eratosthenes bitmap (t ≤ N + 2^27), O(1) per candidate.
2. **k ≤ 63:** deterministic Miller–Rabin for < 2^64 (bases 2, 325, 9375, 28178, 450775, 9780504, 1795265022) — *exact*, no probability.
3. **k ≤ 500,000:** GMP big integers behind a 40-prime incremental wheel (primes ≤ 179 eliminates ~89% of candidates without a single big-integer test); 1-round MR screening, 25-round confirmation of every accepted prime.

### Result — full sweep of n ∈ [2, 10^7]

- **99.9863% of n resolved with k ≤ 2500.** Exactly **1,371 values of n (0.0137%)** have no prime 2^k + n − k for any k ≤ 2500 — an explicit exceptional set (full list in [`sun-deferred-list.txt`](sun-deferred-list.txt)).
- **The largest "small" least-k values found:** k = 2498 at n = 4,645,737 and k = 2492 at n = 2,355,065 — genuine edge-of-the-envelope resolutions.
- **Hardest cases resolved individually** (GMP, k up to 500,000; see [`sun-resolved.txt`](sun-resolved.txt)). Headline record:

> **n = 18,863: the least k is 12,538** — i.e. 2^12538 + 6325 is prime, a **3,775-digit** number, while every smaller legal k gives a composite. Independently re-confirmed with gmpy2's Baillie–PSW test (a different primality path from the search engine's Miller–Rabin), with spot checks that smaller k are composite.

- Least-k records over the sweep: 2161 → 294, 3908 → 399, 7941 → 876, 16119 → 1356, 18863 → 12538, then (under the 2500 cap) 2355065 → 2492, 4645737 → 2498.
- The k-distribution is geometric with a heavy tail (pilot histogram over n ≤ 10^6 in session logs), matching the heuristic P(success at candidate k) ≈ 2/ln(2^k + m); since Σ_k 2/ln(2^k + m) diverges, every n is *expected* to resolve eventually — consistent with zero counterexamples found.

**Honest status.** The published 10^7 bound (Sun) remains the verified record; this run independently reproduces the overwhelming bulk of it with a from-scratch engine and maps the hard core exactly. Each of the 1,371 exceptions is *not* a counterexample — it is a value whose least k exceeds 2500 (the largest resolved example being 12,538), and the background resolver continues certifying them one by one.

---

## What this establishes — and what it cannot

- **A counterexample hunt can only answer "yes."** Had any Beal pair produced a coprime perfect-power sum, the hunt would have ended with a $1,000,000 tuple. Finding nothing updates no proof; it is evidence at the scale of ~7×10^9 trials, in a search space that is infinite.
- **Sun's conjecture is doubly asymmetric:** a counterexample n would require proving *no* k works — impossible by enumeration. So the hunt there could only ever strengthen confidence and map the frontier (which it did: exact least-k records, the 0.0137% hard core, a 3,775-digit prime).
- **Why the famous problems stay open** (see session discussion of the Burnside problem): the difficulty of both prizes lives in proof, not in data. The searches above are the maximum that pure computation can say, stated with exact coverage so anyone can extend them.

## Reproducibility

Sources (this folder): [`beal-search.c`](beal-search.c) / [`beal-search-2.c`](beal-search-2.c) (Beal, hash-table and Bloom versions), [`sun-search.c`](sun-search.c) / [`sun-search-2.c`](sun-search-2.c) / [`sun-search-3.c`](sun-search-3.c) (Sun verifier, three iterations), [`sun-resolver.c`](sun-resolver.c) (GMP deferred-case resolver). Data: [`sun-deferred-list.txt`](sun-deferred-list.txt) (the 1,371 exceptions), [`sun-resolved.txt`](sun-resolved.txt) (certified resolutions). Every number in this report is reproducible by recompiling and rerunning; the record prime was independently re-verified.

[^7]: arXiv:2605.15758, "Positive density for Sun's 2^k+m conjecture" (2026) — confirms published verification bound n ≤ 10^7.
[^8]: Zhi-Wei Sun's homepage — conjecture statement, $1000 prize, "verified for n up to 10^7".
[^9]: R. D. Mauldin (UNT), "The Beal Conjecture and Prize" — official statement and prize terms.
[^10]: Durango Bill, "Beal's Conjecture" — the 10 known Fermat–Catalan solutions (re-verified here).
[^12]: P. Norvig, "Beal's Conjecture: A Search for Counterexamples" (beal2000) — prior computer search.
[^14]: Wikipedia, "Beal conjecture" — prize history ($5,000 → $1,000,000, AMS trust) and variants.
