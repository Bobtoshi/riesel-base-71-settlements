
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
typedef unsigned long long u64;
typedef __uint128_t u128;

static int cmpu64(const void*a,const void*b){u64 x=*(const u64*)a,y=*(const u64*)b;return (x>y)-(x<y);}

/* ---- Bloom filter over the C^z table (2^25 bits = 4MB, cache-friendly) ---- */
#define LB 25
#define BBITS (1ULL<<LB)
static unsigned char bloom[BBITS/8];
static inline void badd(u64 x){
    u64 h1=(x*0x9E3779B97F4A7C15ULL)>>(64-LB), h2=(x*0xBF58476D1CE4E5B9ULL)>>(64-LB);
    bloom[h1>>3]|=1<<(h1&7); bloom[h2>>3]|=1<<(h2&7);
}
static inline int bmaybe(u64 x){
    u64 h1=(x*0x9E3779B97F4A7C15ULL)>>(64-LB), h2=(x*0xBF58476D1CE4E5B9ULL)>>(64-LB);
    return (bloom[h1>>3]>>(h1&7)&1) & (bloom[h2>>3]>>(h2&7)&1);
}

static u64* P; static size_t nP;      /* sorted C^z table for confirmations */
static int in_table(u64 s){
    size_t lo=0, hi=nP;
    while(lo<hi){ size_t mid=(lo+hi)>>1; if(P[mid]<s) lo=mid+1; else hi=mid; }
    return lo<nP && P[lo]==s;
}

static int* spf;
static void build_spf(u64 n){ spf=malloc((n+1)*sizeof(int)); for(u64 i=0;i<=n;i++) spf[i]=(int)i;
    for(u64 p=2;p*p<=n;p++) if(spf[p]==(int)p) for(u64 m=p*p;m<=n;m+=p) if(spf[m]==(int)m) spf[m]=(int)p; }
static inline int coprime(u64 a, u64 b){ while(a>1){ int p=spf[a]; if(b%(u64)p==0) return 0; while(a%(u64)p==0) a/=(u64)p; } return 1; }

typedef struct { u64 val; int exp; } Pow;

int main(int argc, char** argv){
    const u64 LIM_A = 4000000000000000000ULL;
    const u64 LIM_C = 8000000000000000000ULL;
    u64 AMAX = argc>1? strtoull(argv[1],0,10) : 100000ULL;
    u64 IA0  = argc>2? strtoull(argv[2],0,10) : 0;
    u64 IA1  = argc>3? strtoull(argv[3],0,10) : ~0ULL;

    /* C-side perfect powers */
    size_t cap=1<<22; P=malloc(cap*sizeof(u64)); nP=0;
    for(u64 c=2;;c++){ u128 v=(u128)c*c*c; if(v>LIM_C) break; u128 p=v;
        for(int z=3;;z++){ if(p>LIM_C) break; if(nP>=cap){cap<<=1;P=realloc(P,cap*sizeof(u64));} P[nP++]=(u64)p; p*=c; } }
    qsort(P,nP,sizeof(u64),cmpu64);
    size_t n=0; for(size_t i=0;i<nP;i++) if(i==0||P[i]!=P[i-1]) P[n++]=P[i]; nP=n;
    for(size_t i=0;i<nP;i++) badd(P[i]);
    fprintf(stderr,"C-table: %zu powers, bloom %d MB\n", nP, 1<<(LB-20));

    /* A/B powers grouped by base */
    build_spf(AMAX);
    u64 nB = AMAX;                       /* bases 1..AMAX, index a-1 */
    u64* start=malloc((nB+2)*sizeof(u64)); u64* len=malloc((nB+1)*sizeof(u64));
    size_t capW=1<<22, nW=0; Pow* W=malloc(capW*sizeof(Pow));
    for(u64 a=1;a<=AMAX;a++){
        start[a-1]=nW; u128 p=(u128)a*a*a;
        if(a==1){ if(nW>=capW){capW<<=1;W=realloc(W,capW*sizeof(Pow));} W[nW].val=1; W[nW].exp=3; nW++; len[0]=1; continue; }
        for(int x=3;;x++){ if(p>LIM_A) break; if(nW>=capW){capW<<=1;W=realloc(W,capW*sizeof(Pow));}
            W[nW].val=(u64)p; W[nW].exp=x; nW++; p*=a; }
        len[a-1]=nW-start[a-1];
    }
    start[nB]=nW;
    fprintf(stderr,"A/B powers: %zu across %llu bases\n", nW, nB);

    if(IA1>nB) IA1=nB;
    unsigned long long checked=0, nears=0;
    for(u64 ia=IA0; ia<IA1; ia++){
        if(((ia-IA0)&0x3FF)==0) fprintf(stderr,"  ia=%llu/%llu (%.1f%%)\r", ia, IA1, 100.0*(ia-IA0)/(IA1-IA0));
        u64 a=ia+1;
        for(u64 ib=ia; ib<nB; ib++){
            u64 b=ib+1;
            if(!coprime(a,b)) continue;
            u64 sa=start[ia], ea=sa+len[ia], sb=start[ib], eb=sb+len[ib];
            for(u64 i=sa;i<ea;i++){
                u64 av=W[i].val;
                u64 j0 = (ia==ib)? i : sb;
                for(u64 j=j0;j<eb;j++){
                    u64 s=av+W[j].val; checked++;
                    if(bmaybe(s)){
                        if(in_table(s)){
                            int z; u64 c=0;
                            for(z=63;z>=3;z--){ u64 r=(u64)powl((long double)s,1.0L/z); if(r<2) continue;
                                for(u64 cc=r>1?r-1:1;;cc++){ u128 pw=1; int zz; for(zz=0;zz<z;zz++){ pw*=cc; if(pw>s) break; }
                                    if(pw==s){ c=cc; break; } if(pw>=s||zz<z) break; }
                                if(c) break; }
                            printf("*** BEAL COUNTEREXAMPLE: %llu^%d + %llu^%d = %llu = %llu^%d (gcd 1)\n",
                                   a,W[i].exp,b,W[j].exp,s,c,z); fflush(stdout);
                        }
                    } else if(nears<200 && (s&63)==0 && (bmaybe(s+1)||bmaybe(s-1)) && (in_table(s+1)||in_table(s-1))){
                        nears++; printf("near-miss: %llu^%d + %llu^%d = %llu (1 away from perfect power)\n",
                                        a,W[i].exp,b,W[j].exp,s);
                    }
                }
            }
        }
    }
    printf("DONE bases [%llu,%llu) of %llu Amax=%llu: %llu coprime pairs checked, %llu near-misses\n",
           IA0,IA1,nB,AMAX,checked,nears);
    return 0;
}
