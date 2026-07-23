#!/usr/bin/env python3
"""Independently verify the published Riesel base-71 certificates."""

from __future__ import annotations

import argparse
import json
import math
from pathlib import Path
from typing import Any

import gmpy2
from sympy import isprime


def exact_small_prime(value: int) -> bool:
    """Deterministically test the small factors used by these certificates."""
    if value < 2:
        return False
    if value % 2 == 0:
        return value == 2
    for divisor in range(3, math.isqrt(value) + 1, 2):
        if value % divisor == 0:
            return False
    return True


def require_int(record: dict[str, Any], field: str) -> int:
    value = record.get(field)
    if type(value) is not int:
        raise ValueError(f"{field} must be an integer")
    return value


def complete_factorization(value: gmpy2.mpz, qs: list[int]) -> dict[int, int]:
    """Divide value completely by the distinct certified prime factors."""
    if not qs or qs != sorted(set(qs)):
        raise ValueError("qs must be a nonempty sorted list of distinct factors")

    remainder = gmpy2.mpz(value)
    multiplicities: dict[int, int] = {}
    for q in qs:
        if type(q) is not int or not exact_small_prime(q):
            raise ValueError(f"qs contains a non-prime factor: {q!r}")
        exponent = 0
        while remainder % q == 0:
            remainder //= q
            exponent += 1
        if exponent == 0:
            raise ValueError(f"listed factor {q} does not divide N+1")
        multiplicities[q] = exponent

    if remainder != 1:
        raise ValueError(f"qs do not completely factor N+1; cofactor={remainder}")
    return multiplicities


def lucas_u_mod(P: int, Q: int, index: gmpy2.mpz, modulus: gmpy2.mpz) -> gmpy2.mpz:
    """Return U_index(P, Q) modulo modulus by binary doubling."""
    if index == 0:
        return gmpy2.mpz(0)

    D = P * P - 4 * Q
    inv2 = gmpy2.invert(2, modulus)
    if inv2 == 0:
        raise ValueError("2 is not invertible modulo N")

    U = gmpy2.mpz(1) % modulus
    V = gmpy2.mpz(P) % modulus
    Qk = gmpy2.mpz(Q) % modulus

    for bit in index.digits(2)[1:]:
        U, V, Qk = (
            (U * V) % modulus,
            (V * V - 2 * Qk) % modulus,
            (Qk * Qk) % modulus,
        )
        if bit == "1":
            U, V = (
                ((P * U + V) * inv2) % modulus,
                ((D * U + P * V) * inv2) % modulus,
            )
            Qk = (Qk * Q) % modulus
    return U


def verify_record(record: dict[str, Any]) -> tuple[int, int, int, int]:
    k = require_int(record, "k")
    n = require_int(record, "n")
    digits = require_int(record, "digits")
    D = require_int(record, "lucas_D")
    P = require_int(record, "P")
    Q = require_int(record, "Q")
    qs = record.get("qs")

    if k <= 0 or n < 1 or digits < 1:
        raise ValueError("k, n, and digits must be positive")
    if not isinstance(qs, list):
        raise ValueError("qs must be a list")
    if record.get("sympy") is not True:
        raise ValueError("stored sympy cross-check flag is not true")
    if D != P * P - 4 * Q:
        raise ValueError("lucas_D does not equal P^2 - 4Q")

    N = gmpy2.mpz(k) * gmpy2.mpz(71) ** n - 1
    if N <= 2 or N % 2 == 0:
        raise ValueError("N is not an odd integer greater than 2")
    if len(N.digits(10)) != digits:
        raise ValueError("recorded digit count does not match N")

    factors = complete_factorization(N + 1, qs)
    if set(factors) != set(qs):
        raise ValueError("N+1 factor set does not match qs")

    if gmpy2.gcd(N, 2 * Q * D) != 1:
        raise ValueError("gcd(N, 2QD) is not 1")
    if gmpy2.jacobi(D, N) != -1:
        raise ValueError("Jacobi(D, N) is not -1")
    if lucas_u_mod(P, Q, N + 1, N) != 0:
        raise ValueError("U_(N+1) is not 0 modulo N")

    for q in qs:
        witness = lucas_u_mod(P, Q, (N + 1) // q, N)
        if gmpy2.gcd(witness, N) != 1:
            raise ValueError(f"Lucas gcd condition failed for q={q}")

    if gmpy2.is_prime(N, 25) == 0:
        raise ValueError("gmpy2 rejected N")
    if not bool(isprime(int(N))):
        raise ValueError("sympy.isprime rejected N")

    return k, n, digits, D


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Verify every Riesel base-71 certificate in a JSONL file."
    )
    parser.add_argument(
        "certificates",
        nargs="?",
        type=Path,
        default=Path("data/riesel-71-certificates.jsonl"),
        help="certificate JSONL path (default: data/riesel-71-certificates.jsonl)",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    failed = False
    seen = 0
    identities: set[tuple[int, int]] = set()

    with args.certificates.open("r", encoding="utf-8") as handle:
        for line_number, line in enumerate(handle, start=1):
            if not line.strip():
                continue
            seen += 1
            record: dict[str, Any] | None = None
            try:
                record = json.loads(line)
                if not isinstance(record, dict):
                    raise ValueError("certificate is not a JSON object")
                k, n, digits, D = verify_record(record)
                if (k, n) in identities:
                    raise ValueError("duplicate certificate")
                identities.add((k, n))
                print(f"PASS k={k} n={n} digits={digits} lucas_D={D}")
            except Exception as exc:
                failed = True
                k = record.get("k", "?") if isinstance(record, dict) else "?"
                n = record.get("n", "?") if isinstance(record, dict) else "?"
                print(f"FAIL line={line_number} k={k} n={n} error={exc}")

    if seen != 11:
        print(f"FAIL expected 11 certificates, found {seen}")
        return 1
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
