
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <gmp.h>
typedef uint64_t u64; typedef __uint128_t u128;
static u64 mulmod(u64 a,u64 b,u64 m){ return (u64)(((u128)a*b)%m); }
static u64 powmod(u64 a,u64 d,u64 m){ u64 r=1; a%=m; while(d){ if(d&1) r=mulmod(r,a,m); a=mulmod(a,a,m); d>>=1; } return r; }
#define NPR 40
static u64 pr[NPR]; static int npr=0;
static void init_pr(void){ for(u64 c=3; c<=179 && npr<NPR; c+=2){ int ok=1; for(u64 d=3; d*d<=c; d+=2) if(c%d==0){ok=0;break;} if(ok) pr[npr++]=c; } }
int main(int argc, char** argv){
    FILE* f = fopen(argv[1],"r");
    long start = atol(argv[2]), cnt = atol(argv[3]);
    char line[64]; long idx=-1; u64 n;
    init_pr(); mpz_t t; mpz_init(t);
    const int KHI = 500000;
    while(fgets(line,sizeof line,f)){
        idx++; if(idx<start) continue; if(idx>=start+cnt) break;
        n = strtoull(line,0,10);
        int k0 = (n%2==0)?2501:2502, found=0, k;
        u64 r[NPR], md[NPR];
        for(int i=0;i<npr;i++){ u64 p=pr[i]; r[i]=powmod(2,k0,p); md[i]=(n-(u64)k0)%p; }
        for(k=k0;k<=KHI;k+=2){
            int comp=0;
            for(int i=0;i<npr;i++){ u64 p=pr[i]; if((r[i]+md[i])%p==0){comp=1;break;} }
            if(!comp){
                mpz_set_ui(t, n-(u64)k); mpz_setbit(t,k);
                if(mpz_probab_prime_p(t,1)>0 && mpz_probab_prime_p(t,25)>0){ found=1; break; }
            }
            for(int i=0;i<npr;i++){ u64 p=pr[i]; r[i]=(4*r[i])%p; md[i]=(md[i]+p-2)%p; }
        }
        if(found) printf("RESOLVED %llu %d %d\n", n, k, (int)mpz_sizeinbase(t,2));
        else printf("OPEN %llu (no k<=%d)\n", n, KHI);
        fflush(stdout);
    }
    return 0;
}
