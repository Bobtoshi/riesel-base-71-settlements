
import json, math, sys
import numpy as np
from gmpy2 import mpz, is_prime
from sympy import jacobi_symbol, isprime

L = 300000
_s = np.ones(L+1, bool); _s[:2] = False
for i in range(2, int(L**0.5)+1):
    if _s[i]: _s[i*i::i] = False
P = np.nonzero(_s)[0].astype(np.uint64)
def factor_small(m):
    fs = []
    for p in P:
        p = int(p)
        if p*p > m: break
        while m % p == 0: fs.append(p); m //= p
    if m > 1: fs.append(int(m)); assert is_prime(mpz(m))
    return fs
def lucas_U(P, Q, m, N):
    D = P*P - 4*Q
    inv2 = pow(2, -1, N)
    U, V, Qk = 1 % N, P % N, Q % N
    for bit in bin(m)[3:]:
        U, V, Qk = (U*V) % N, (V*V - 2*Qk) % N, (Qk*Qk) % N
        if bit == '1':
            U, V = ((P*U + V) * inv2) % N, ((D*U + P*V) * inv2) % N
            Qk = (Qk * Q) % N
    return U
def lucas_Nplus1_proof(N, qs):
    for D in [5, -7, 9, 13, -15, 17, -19, 21, -23, 25, 29, -31, 33, 37, -39, 41]:
        if D % 4 != 1: continue
        if jacobi_symbol(D, N) != -1: continue
        P, Q = 1, (1 - D)//4
        if lucas_U(P, Q, N+1, N) != 0: continue
        if all(math.gcd(lucas_U(P, Q, (N+1)//q, N), N) == 1 for q in qs):
            return D, P, Q
    return None

done_ks = set()
try:
    for line in open('/tmp/settled71_certified.jsonl'):
        done_ks.add(json.loads(line)['k'])
except FileNotFoundError: pass
out = open('/tmp/settled71_certified.jsonl','a')
for line in open('/tmp/settled71.jsonl'):
    rec = json.loads(line)
    if rec['status'] != 'SETTLED' or rec['k'] in done_ks: continue
    k, n = rec['k'], rec['n']
    N = k * 71**n - 1
    qs = sorted(set(factor_small(k)) | {71})
    assert is_prime(mpz(N)), f"k={k}: gmpy2 BPSW FAIL"
    wit = lucas_Nplus1_proof(N, qs)
    assert wit is not None, f"k={k}: Lucas N+1 FAILED"
    rec2 = {'k':k,'n':n,'digits':rec['digits'],'lucas_D':wit[0],'P':wit[1],'Q':wit[2],'qs':qs,'sympy':bool(isprime(N))}
    out.write(json.dumps(rec2)+'\n'); out.flush()
    print(f"PROVEN k={k} n={n} ({rec['digits']} digits) D={wit[0]}", flush=True)
out.close()
print("CERTIFICATION PASS COMPLETE", flush=True)
