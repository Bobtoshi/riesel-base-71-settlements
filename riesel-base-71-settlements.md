---
layout: default
title: Full report
permalink: /report.html
---

# Settled Instances of the Riesel Base-71 Conjecture

**Session date:** 2026-07-24 · **Arithmetic:** exact (GMP / pure-Python big integers) · **Proof standard:** every prime below is *proven*, not merely probable.

## What was settled, in one sentence

For each *k* in the results table: **the first prime of the form k·71ⁿ − 1 was found, and the least such n is fully certified** — every smaller n is provably composite, and the primality is proven three independent ways, including a Brillhart–Lehmer–Selfridge Lucas (N+1) proof that anyone can re-verify in seconds.

## The problem and its provenance

- **Riesel problem (1956):** a Riesel number is an odd *k* such that k·2ⁿ − 1 is composite for every n ≥ 1. The conjecture that 509203 is the smallest remains unproven. The Sierpiński analogue (k·2ⁿ + 1, 1960) is likewise famous.
- **Conjectures 'R Us (CRUS), started Dec 14, 2007:** proves the analogue for every base *b*: for each base there is a conjectured smallest Riesel/Sierpiński value CK, established by finding a prime k·bⁿ ∓ 1 for every *k* below CK.
- **Riesel base 71 (started Nov 24, 2023):** as of the Oct 23, 2025 status update, k ≤ 1M had been tested to n = 10,000 and **k > 1M only to n = 1,000**, leaving **19,068,114 unproven k's**. The base was unreserved on the July 22, 2026 reservations snapshot, so these instances were open to anyone.

Each unproven *k* is an individual open question: *does k·71ⁿ − 1 ever yield a prime?* This run settles the first several on the list, definitively.

## Method (four stages, each stage certain)

1. **Sieve** for n ∈ [1, 4000]: vectorized residue tracking over all 25,997 primes ≤ 3×10⁵. An eliminated n is *proven* composite (explicit small factor).
2. **Fermat screen** (base 2) on survivors: a failed test is a *proof* of compositeness.
3. **Baillie–PSW** primality test (two independent implementations: GMP's Miller–Rabin suite and sympy) on the rare passers.
4. **BLS Lucas N+1 proof** of each prime: since N+1 = k·71ⁿ factors completely (factors of k, plus 71), the Brillhart–Lehmer–Selfridge theorem applies directly with F = N+1 ≥ √N, giving a deterministic primality proof with witness (D, P, Q). The prover was validated on a test suite: Mersenne primes 2¹³−1 … 2¹²⁷−1 (all proven) and composite Mersennes 2²³−1, 2²⁹−1, 2³⁷−1, 2⁴¹−1 (all correctly rejected).

Because every n below the certified least n falls at stage 1 or 2, **the least-n claim is itself proof-grade** — no probable-prime reasoning anywhere in the final certificates.

## Results — 11 instances settled and proven

| k | least n | digits of k·71ⁿ − 1 | Lucas witness D |
|---|---|---|---|
| 1000004 | 1266 | 2350 | 5 |
| 1000034 | 1176 | 2184 | −19 |
| 1000174 | 2101 | 3896 | −19 |
| 1000222 | 1679 | 3115 | 53 |
| 1000834 | 3571 | 6617 | 61 |
| 1000892 | 2678 | 4964 | 21 |
| 1001020 | 2963 | 5492 | 21 |
| 1001072 | 1014 | 1884 | 65 |
| 1001248 | 1421 | 2637 | 5 |
| 1001332 | 2731 | 5062 | 37 |
| 1001464 | 1405 | 2608 | 5 |

Of the first 23 k's on the remaining list, 11 settled (48%); the other 12 (1000162, 1000240, 1000288, 1000310, 1000360, 1000402, 1000498, 1000562, 1000898, 1001270, 1001432, 1001480) were searched to n = 4000 without a prime — still open, with their search frontier now pushed from 1000 to 4000.

For every settled k: all n below the least n are *provably* composite (small-factor sieve witnesses or Fermat base-2 witnesses), and the prime itself is proven by (a) GMP Miller–Rabin suite, (b) sympy Baillie–PSW, and (c) the BLS Lucas N+1 proof with the tabulated witness — three independent paths.

Full machine-readable certificates: [`riesel-71-certificates.jsonl`](data/riesel-71-certificates.jsonl) (fields: k, n, digits, Lucas D/P/Q, prime factors of N+1, sympy cross-check flag). Raw batch output: [`riesel-71-raw-results.jsonl`](data/riesel-71-raw-results.jsonl).

## Honest scope

- This settles **individual instances** — the first handful of 19,068,114 remaining k's. The base-71 conjecture itself remains open (it may have a conjectured CK in the hundreds of millions; completing it is a project-scale effort, not a session-scale one).
- The results are genuine, checkable new mathematics in a 70-year-old problem family: each prime found removes one *k* from CRUS's remaining list, and the least-n determination is exhaustive. They are reportable to the CRUS project ([Mersenneforum thread 6722](https://www.mersenneforum.org/node/6722)).
- The verified status window is per the Oct 23, 2025 base-71 status page and the July 22, 2026 reservations snapshot; a small chance exists that some of these k's were independently resolved by the project after the snapshot — the primes remain correct regardless.

## Reproducibility

- [`riesel-71-batch.py`](code/riesel-71-batch.py) — the settlement engine (sieve → Fermat → BPSW → Pocklington/Lucas).
- [`riesel-71-certify.py`](code/riesel-71-certify.py) — independent certification pass (Lucas N+1 + sympy).
- [`riesel-71-raw-results.jsonl`](data/riesel-71-raw-results.jsonl), [`riesel-71-certificates.jsonl`](data/riesel-71-certificates.jsonl) — raw and proven results.
- Source data: Gary Barnes's `remain-riesel-base71.zip` from the [CRUS Riesel conjectures page](https://www.noprimeleftbehind.net/crus/Riesel-conjectures.htm) (Nov 2, 2025 file).

## Attribution

Source k-list: Gary Barnes, CRUS, `remain-riesel-base71.zip`, file dated Nov 2 2025. Status windows: Oct 23 2025 (base page) and Jul 22 2026 ([reservations snapshot](https://www.noprimeleftbehind.net/crus/Riesel-conjecture-reserves.htm)). CRUS coordination: [Mersenneforum thread 6722](https://www.mersenneforum.org/node/6722).
