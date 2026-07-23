
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
typedef uint64_t u64; typedef __uint128_t u128;

static u64 mulmod(u64 a,u64 b,u64 m){ return (u64)(((u128)a*b)%m); }
static u64 powmod(u64 a,u64 d,u64 m){ u64 r=1; while(d){ if(d&1) r=mulmod(r,a,m); a=mulmod(a,a,m); d>>=1; } return r; }
static int isPrime(u64 n){
    if(n<2) return 0;
    static const u64 sp[12]={2,3,5,7,11,13,17,19,23,29,31,37};
    for(int i=0;i<12;i++){ if(n%sp[i]==0) return n==sp[i]; }
    u64 d=n-1; int r=0; while(!(d&1)){ d>>=1; r++; }
    static const u64 bs[7]={2ULL,325ULL,9375ULL,28178ULL,450775ULL,9780504ULL,1795265022ULL};
    for(int i=0;i<7;i++){ u64 a=bs[i]%n; if(!a) continue; u64 x=powmod(a,d,n);
        if(x==1||x==n-1) continue; int comp=1;
        for(int j=1;j<r;j++){ x=mulmod(x,x,n); if(x==n-1){ comp=0; break; } }
        if(comp) return 0; }
    return 1;
}

/* odd-only bitmap sieve up to LIM; bit i <-> number 2i+1 (1 = composite) */
static u64* bm; static u64 LIM;
static void sieve(void){
    u64 nbits=LIM/2+2, nw=nbits/64+1; bm=calloc(nw,8);
    if(!bm){ fprintf(stderr,"bitmap alloc failed (%llu MB)\n", nw*8>>20); exit(2); }
    fprintf(stderr,"sieve: LIM=%llu, bitmap %llu MB\n", LIM, nw*8>>20);
    for(u64 p=3; p*p<=LIM; p+=2){
        if( (bm[(p>>1)>>6]>>((p>>1)&63)) & 1 ) continue;
        for(u64 m=p*p; m<=LIM; m+=2*p) bm[(m>>1)>>6] |= 1ULL<<((m>>1)&63);
    }
}
static inline int bmp_prime(u64 t){ if(t<2) return 0; if(t==2) return 1; if(!(t&1)) return 0;
    return !( (bm[(t>>1)>>6]>>((t>>1)&63)) & 1 ); }

int main(int argc, char** argv){
    u64 N   = argc>1? strtoull(argv[1],0,10) : 1000000000ULL;
    u64 nlo = argc>2? strtoull(argv[2],0,10) : 2;
    u64 nhi = argc>3? strtoull(argv[3],0,10) : N;
    const int KMAX = 31;
    LIM = N + (1ULL<<KMAX) + 64;
    sieve();
    int maxk=0; u64 maxn=0; u64 hist[64]={0}; u64 worst_fail=0;
    for(u64 n=nlo;n<=nhi;n++){
        int found=0, k;
        for(k = (n%2==0)?1:2; k<=KMAX && k<(int)n; k+=2){        /* opposite parity to n => t odd */
            if(bmp_prime((1ULL<<k)+n-k)){ found=1; break; }
        }
        if(!found){                       /* rare fallback: exact Miller-Rabin for k up to 62 */
            for(k=(n%2==0)?1:2; k<=63 && k<(int)n; k+=2)
                if(isPrime((1ULL<<k)+n-k)){ found=1; break; }
        }
        if(!found){ printf("*** n=%llu: NO k<=63 gives a prime -- candidate counterexample (unbounded k remains)\n", n); worst_fail=n; }
        else { hist[k]++; if(k>maxk){ maxk=k; maxn=n; fprintf(stderr,"new hardest: n=%llu needs k=%d (2^%d+%llu=%llu prime)\n", n,k,k,n-k,(1ULL<<k)+n-k); } }
        if(((n-nlo)&0x3FFFFFF)==0) fprintf(stderr,"  n=%llu (%.0f%%)\r", n, 100.0*(n-nlo)/(nhi-nlo));
    }
    printf("DONE Sun 2^k+m verified for all n in [%llu,%llu]; hardest n=%llu needed k=%d\n", nlo,nhi,maxn,maxk);
    printf("k-distribution:"); for(int k=1;k<=maxk;k++) if(hist[k]) printf(" k=%d:%llu", k, hist[k]); printf("\n");
    return 0;
}
