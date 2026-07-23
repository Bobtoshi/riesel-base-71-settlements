
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
typedef unsigned long long u64;
typedef __uint128_t u128;

static int cmpu64(const void*a,const void*b){u64 x=*(const u64*)a,y=*(const u64*)b;return (x>y)-(x<y);}

/* ---- open-addressing hash set for the C^z table ---- */
static u64* htab; static u64 hmask;
static inline void hput(u64 x){ u64 h=(x*0x9E3779B97F4A7C15ULL)&hmask; while(htab[h]&&htab[h]!=x) h=(h+1)&hmask; if(!htab[h]) htab[h]=x; }
static inline int hhas(u64 x){ u64 h=(x*0x9E3779B97F4A7C15ULL)&hmask; while(htab[h]){ if(htab[h]==x) return 1; h=(h+1)&hmask; } return 0; }

typedef struct { u64 val; u64 base; int exp; } PP;
static int cmpPP(const void*a,const void*b){u64 x=((const PP*)a)->val,y=((const PP*)b)->val;return (x>y)-(x<y);}

static int* spf;
static void build_spf(u64 n){ spf=malloc((n+1)*sizeof(int)); for(u64 i=0;i<=n;i++) spf[i]=(int)i;
    for(u64 p=2;p*p<=n;p++) if(spf[p]==(int)p) for(u64 m=p*p;m<=n;m+=p) if(spf[m]==(int)m) spf[m]=(int)p; }
static inline int coprime(u64 a, u64 b){ while(a>1){ int p=spf[a]; if(b%(u64)p==0) return 0; while(a%(u64)p==0) a/=(u64)p; } return 1; }

int main(int argc, char** argv){
    const u64 LIM_A = 4000000000000000000ULL;   /* A^x, B^y <= 4e18  */
    const u64 LIM_C = 8000000000000000000ULL;   /* C^z   <= 8e18     */
    u64 AMAX = argc>1? strtoull(argv[1],0,10) : 100000ULL;
    u64 I0   = argc>2? strtoull(argv[2],0,10) : 0;
    u64 I1   = argc>3? strtoull(argv[3],0,10) : ~0ULL;

    /* ---- C-side table: all perfect powers v=c^z (z>=3) <= 8e18 ---- */
    size_t cap=1<<22, nP=0; u64* P=malloc(cap*sizeof(u64));
    for(u64 c=2;;c++){ u128 v=(u128)c*c*c; if(v>LIM_C) break; u128 p=v;
        for(int z=3;;z++){ if(p>LIM_C) break; if(nP>=cap){cap<<=1;P=realloc(P,cap*sizeof(u64));} P[nP++]=(u64)p; p*=c; } }
    qsort(P,nP,sizeof(u64),cmpu64);
    size_t n=0; for(size_t i=0;i<nP;i++) if(i==0||P[i]!=P[i-1]) P[n++]=P[i];
    size_t hcap=1; while(hcap < n + n/2) hcap<<=1; hmask=hcap-1; htab=calloc(hcap,sizeof(u64));
    for(size_t i=0;i<n;i++) hput(P[i]);
    fprintf(stderr,"C-table: %zu distinct perfect powers (z>=3) <= 8e18\n", n);

    /* ---- A/B list: (a^x, a, x), a<=AMAX, x>=3, a^x<=4e18 ---- */
    build_spf(AMAX);
    size_t capA=1<<20, nA=0; PP* A=malloc(capA*sizeof(PP));
    A[nA].val=1; A[nA].base=1; A[nA].exp=3; nA++;
    for(u64 a=2;a<=AMAX;a++){ u128 p=(u128)a*a*a;
        for(int x=3;;x++){ if(p>LIM_A) break; if(nA>=capA){capA<<=1;A=realloc(A,capA*sizeof(PP));}
            A[nA].val=(u64)p; A[nA].base=a; A[nA].exp=x; nA++; p*=a; } }
    qsort(A,nA,sizeof(PP),cmpPP);
    fprintf(stderr,"A/B list: %zu powers (bases <= %llu, all exponents >=3)\n", nA, AMAX);

    /* ---- pair search ---- */
    if(I1>nA) I1=nA;
    unsigned long long checked=0, nears=0;
    for(u64 i=I0;i<I1;i++){
        if(((i-I0) & 0x3FFFFF)==0) fprintf(stderr,"  progress i=%llu/%llu (%.1f%%)\r", i, I1, 100.0*(i-I0)/(I1-I0));
        u64 av=A[i].val, ab=A[i].base;
        for(u64 j=i;j<nA;j++){
            if(!coprime(ab,A[j].base)) continue;          /* gcd(A,B)=1 <=> pairwise coprime */
            u64 s=av+A[j].val; checked++;
            if(hhas(s)){
                /* recover C,z */
                int z; u64 c=0;
                for(z=63;z>=3;z--){ u64 r=(u64)powl((long double)s,1.0L/z); if(r<2) continue;
                    for(u64 cc=r>1?r-1:1;;cc++){ u128 pw=1; int zz; for(zz=0;zz<z;zz++){ pw*=cc; if(pw>s) break; }
                        if(pw==s){ c=cc; break; } if(pw>=s || zz<z) break; }
                    if(c) break; }
                printf("*** BEAL COUNTEREXAMPLE: %llu^%d + %llu^%d = %llu = %llu^%d  (gcd(%llu,%llu)=1)\n",
                       A[i].base,A[i].exp,A[j].base,A[j].exp,s,c,z,A[i].base,A[j].base); fflush(stdout);
            } else if(nears<200 && (s & 63)==0 && (hhas(s+1)||hhas(s-1))){
                nears++; printf("near-miss: %llu^%d + %llu^%d = %llu (1 away from a perfect power)\n",
                                A[i].base,A[i].exp,A[j].base,A[j].exp,s);
            }
        }
    }
    fprintf(stderr,"\nchecked %llu coprime pairs; near-misses printed %llu\n", checked, nears);
    printf("DONE chunk [%llu,%llu) Amax=%llu : %llu coprime pairs checked\n", I0, I1, AMAX, checked);
    return 0;
}
