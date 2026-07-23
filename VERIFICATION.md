---
layout: default
title: Verification
permalink: /verification.html
---

# Verification

The repository includes two complementary verification paths:

- [`verify.py`](verify.py) independently re-verifies every published prime certificate with only `gmpy2` and `sympy`.
- The supplied [`riesel-71-certify.py`](code/riesel-71-certify.py) reproduces the original independent certification pass used to generate the certificate records.

## Run the independent verifier

Python 3 is required. The only direct dependencies are listed in [`requirements.txt`](requirements.txt).

```bash
python3 -m venv .venv
source .venv/bin/activate
python -m pip install -r requirements.txt
python verify.py data/riesel-71-certificates.jsonl
```

For every nonempty JSONL record, `verify.py`:

1. Computes N = k·71^n − 1 and checks the recorded decimal digit count.
2. Proves each value in `qs` prime by deterministic trial division, divides N+1 by those factors with full multiplicity, and requires the remaining cofactor to be exactly 1.
3. Checks D = P² − 4Q, gcd(N, 2QD) = 1, and Jacobi(D, N) = −1.
4. Runs the BLS Lucas N+1 conditions with the supplied `(D, P, Q)` witness: U_(N+1) ≡ 0 (mod N), and gcd(U_((N+1)/q), N) = 1 for every prime q dividing N+1.
5. Cross-checks the result with both `gmpy2.is_prime(N, 25)` and `sympy.isprime(N)`.

Any failed condition prints `FAIL` for that certificate and makes the process exit nonzero. The verifier also requires 11 distinct certificate records.

## Recorded verification run

This output is from the repository's actual certificate file, run on Jul 23 2026 with Python 3.12.13, gmpy2 2.3.1, and sympy 1.14.0. The process exited with code 0.

```text
PASS k=1000004 n=1266 digits=2350 lucas_D=5
PASS k=1000034 n=1176 digits=2184 lucas_D=-19
PASS k=1000174 n=2101 digits=3896 lucas_D=-19
PASS k=1000222 n=1679 digits=3115 lucas_D=53
PASS k=1000834 n=3571 digits=6617 lucas_D=61
PASS k=1000892 n=2678 digits=4964 lucas_D=21
PASS k=1001020 n=2963 digits=5492 lucas_D=21
PASS k=1001072 n=1014 digits=1884 lucas_D=65
PASS k=1001248 n=1421 digits=2637 lucas_D=5
PASS k=1001332 n=2731 digits=5062 lucas_D=37
PASS k=1001464 n=1405 digits=2608 lucas_D=5
```

## Why the least-n claim is proof-grade

The endpoint proof and the least-n proof answer different questions. The Lucas certificate proves that the number at the reported n is prime. Least-n additionally requires every earlier n to be composite.

The settlement engine exhaustively processed every smaller n through two proof-producing exclusion stages:

- **Sieve witness:** when a prime q ≤ 300000 divides k·71^n − 1, q is an explicit factor. That is a direct proof of compositeness.
- **Fermat witness:** every sieve survivor was checked at base 2. A prime N must satisfy 2^(N−1) ≡ 1 (mod N). When that congruence fails, the nonzero remainder is a direct proof that N is composite.

Every smaller n is eliminated by one of those two witnesses. Probable-prime screening is not used to justify the least-n exclusions. The final n then receives the independent GMP, sympy, and deterministic Lucas N+1 checks described above.

The compact certificate JSONL records the prime endpoints. The smaller-n witnesses are reproducible from the exhaustive loop in [`riesel-71-batch.py`](code/riesel-71-batch.py); they are not encoded as additional rows in the certificate file.

## Spot-check with the supplied certification engine

The original certification engine uses NumPy in addition to `gmpy2` and `sympy`, and its input/output paths are the original hard-coded `/tmp/settled71.jsonl` and `/tmp/settled71_certified.jsonl`. In a clean temporary environment:

```bash
python3 -m venv .certify-venv
source .certify-venv/bin/activate
python -m pip install numpy gmpy2 sympy
cp data/riesel-71-raw-results.jsonl /tmp/settled71.jsonl
python code/riesel-71-certify.py
diff -u data/riesel-71-certificates.jsonl /tmp/settled71_certified.jsonl
```

The script recomputes N, rebuilds the complete prime-factor set of N+1, searches for a valid Lucas witness, and reruns the GMP and sympy primality paths. It independently spot-checks the certified endpoint. The exhaustive smaller-n traversal itself is in `riesel-71-batch.py`, as described above.
