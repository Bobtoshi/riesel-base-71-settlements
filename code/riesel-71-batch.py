
import numpy as np, time, json, sys
from gmpy2 import mpz, is_prime, powmod, gcd

L = 300000
_s = np.ones(L+1, bool); _s[:2] = False
for i in range(2, int(L**0.5)+1):
    if _s[i]: _s[i*i::i] = False
P = np.nonzero(_s)[0].astype(np.uint64)
_CACHE = {}
def init_residues(nlo):
    if nlo not in _CACHE:
        _CACHE[nlo] = np.array([pow(71, nlo, int(p)) for p in P], dtype=np.uint64)
    return _CACHE[nlo]
def survivors(k, nlo, nhi):
    r = ((k % P) * init_residues(nlo)) % P
    out = []
    for nn in range(nlo, nhi+1):
        if not (r == 1).any(): out.append(nn)
        r = (r * 71) % P
    return out
def prp_pass(t):
    return powmod(2, t-1, t) == 1
def factor_small(m):
    fs = []
    for p in P:
        p = int(p)
        if p*p > m: break
        while m % p == 0: fs.append(p); m //= p
    if m > 1: fs.append(int(m)); assert is_prime(mpz(m))
    return fs
def pocklington(k, n):
    N = mpz(k)*mpz(71)**n - 1
    qs = sorted(set(factor_small(k)) | {71})
    F1 = N - 1
    for a0 in [2,3,5,7,11,13,17,19,23,29,31,37,41,43,47]:
        a = mpz(a0)
        if powmod(a, F1, N) != 1: continue
        if all(gcd(powmod(a, F1//q, N) - 1, N) == 1 for q in qs):
            return int(a0), qs
    return None, None
def settle_k(k, ncap=4000):
    hits = []
    for nn in survivors(k, 1001, ncap):
        t = mpz(k)*mpz(71)**nn - 1
        if prp_pass(t) and is_prime(t):
            hits.append(nn)
    if not hits: return {'k':k,'status':'OPEN','searched_to':ncap}
    nn = hits[0]
    for m in survivors(k, 1, 1000):              # certify least-n: no prime below 1001
        tm = mpz(k)*mpz(71)**m - 1
        if prp_pass(tm) and is_prime(tm):
            return {'k':k,'status':'ANOMALY','n':m}
    t = mpz(k)*mpz(71)**nn - 1
    a, qs = pocklington(k, nn)
    return {'k':k,'status':'SETTLED','n':nn,'digits':len(str(t)),'pock_a':a,'pock_qs':qs}

count = int(sys.argv[1]); skip = int(sys.argv[2])
out = open('/tmp/settled71.jsonl','a'); log = open('/tmp/settled71.log','a')
done = 0; idx = -1
with open('/tmp/remain-riesel-base71.txt') as f:
    for line in f:
        k = int(line)
        if k <= 1000000: continue
        idx += 1
        if idx < skip: continue
        if done >= count: break
        r = settle_k(k); done += 1
        out.write(json.dumps(r)+'\n'); out.flush()
        log.write(f"{time.strftime('%H:%M:%S')} i={idx} k={k} {r['status']}\n"); log.flush()
log.write(f"BATCH DONE skip={skip} count={count}\n"); log.close(); out.close()
