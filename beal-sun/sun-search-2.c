
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <gmp.h>
typedef uint64_t u64; typedef __uint128_t u128;

static u64 mulmod(u64 a,u64 b,u64 m){ return (u64)(((u128)a*b)%m); }
static u64 powmod(u64 a,u64 d,u64 m){ u64 r=1; a%=m; while(d){ if(d&1) r=mulmod(r,a,m); a=mulmod(a,a,m); d>>=1; } return r; }
static int isPrime64(u64 n){
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

static u64* bm; static u64 LIM;
static void sieve(void){
    u64 nbits=LIM/2+2, nw=nbits/64+1; bm=calloc(nw,8);
    if(!bm){ fprintf(stderr,"bitmap alloc failed\n"); exit(2); }
    for(u64 p=3; p*p<=LIM; p+=2){
        if((bm[(p>>1)>>6]>>((p>>1)&63))&1) continue;
        for(u64 m=p*p; m<=LIM; m+=2*p) bm[(m>>1)>>6] |= 1ULL<<((m>>1)&63);
    }
}
static inline int bmp_prime(u64 t){ if(t<2) return 0; if(t==2) return 1; if(!(t&1)) return 0;
    return !((bm[(t>>1)>>6]>>((t>>1)&63))&1); }

#define NPR 40
static u64 pr[NPR]; static int npr=0;
static void init_pr(void){ for(u64 c=3; c<=179 && npr<NPR; c+=2){ int ok=1; for(u64 d=3; d*d<=c; d+=2) if(c%d==0){ok=0;break;} if(ok) pr[npr++]=c; } }

int main(int argc, char** argv){
    u64 N   = argc>1? strtoull(argv[1],0,10) : 100000000ULL;
    u64 nlo = argc>2? strtoull(argv[2],0,10) : 2;
    u64 nhi = argc>3? strtoull(argv[3],0,10) : N;
    const int KMAX = 31, KHI = 200000;
    LIM = N + (1ULL<<KMAX) + 64;
    sieve(); init_pr();
    mpz_t t; mpz_init(t);
    int maxk=0; u64 maxn=0, unresolved=0, hard=0;
    for(u64 n=nlo;n<=nhi;n++){
        int found=0, k;
        for(k=(n%2==0)?1:2; k<=KMAX && k<(int)n; k+=2)
            if(bmp_prime((1ULL<<k)+n-k)){ found=1; break; }
        if(!found)
            for(k=(n%2==0)?1:2; k<=63 && k<(int)n; k+=2)
                if(isPrime64((1ULL<<k)+n-k)){ found=1; break; }
        if(!found){
            hard++;
            int k0 = (n%2==0)? 65 : 64;
            if(k0<(int)n){
                u64 r[NPR], md[NPR];
                for(int i=0;i<npr;i++){ u64 p=pr[i]; r[i]=powmod(2,k0,p); md[i]=(n-(u64)k0)%p; }
                for(k=k0; k<=KHI && k<(int)n; k+=2){
                    int comp=0;
                    for(int i=0;i<npr;i++){ u64 p=pr[i]; if((r[i]+md[i])%p==0){ comp=1; break; } }
                    if(!comp){
                        mpz_set_ui(t, n-(u64)k); mpz_setbit(t, k);
                        if(mpz_probab_prime_p(t, 8)){ found=1; break; }
                    }
                    for(int i=0;i<npr;i++){ u64 p=pr[i]; r[i]=(4*r[i])%p; md[i]=(md[i]+p-2)%p; }
                }
            }
            if(!found){ unresolved++; printf("*** UNRESOLVED n=%llu (no legal k<=%d)\n", n, KHI); }
        }
        if(found && k>maxk){ maxk=k; maxn=n; fprintf(stderr,"new hardest n=%llu k=%d\n", n, k); }
        if(((n-nlo)&0x3FFFFFF)==0) fprintf(stderr,"  n=%llu\r", n);
    }
    printf("DONE n in [%llu,%llu]: %s; hardest n=%llu needed k=%d; hard(k>63)=%llu; unresolved=%llu\n",
           nlo,nhi, unresolved?"INCOMPLETE":"ALL VERIFIED", maxn,maxk, hard, unresolved);
    return 0;
}
