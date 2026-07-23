---
layout: default
title: Public evidence
description: Public evidence for 11 certified least-n results in the Riesel base-71 conjecture.
---

<div class="evidence-page">
  <section class="hero">
    <div class="hero__inner">
      <p class="eyebrow">Computational number theory · public evidence</p>
      <h1>Riesel base 71<br>11 certified settlements</h1>
      <p class="hero__summary">This is a computational result about the Riesel base-71 conjecture. For each of 11 previously unproven k-values on the CRUS "Riesel base 71 remain" list, the least n such that k·71^n − 1 is prime was found and certified. Every smaller n is provably composite, and every prime is proven three independent ways (GMP Miller–Rabin, sympy BPSW, and a Brillhart–Lehmer–Selfridge Lucas N+1 proof).</p>
      <div class="action-row">
        <a class="button" href="{{ '/report.html' | relative_url }}">Read the full report</a>
        <a class="button button--quiet" href="{{ '/verification.html' | relative_url }}">Run the verification</a>
      </div>
    </div>
  </section>

  <section class="content-section content-section--surface">
    <div class="content-section__inner">
      <p class="section-kicker">Proof path</p>
      <h2>Four gates, one checkable endpoint</h2>
      <p class="lead">The exclusion stages establish compositeness below each least n. The final Lucas N+1 stage supplies a deterministic primality proof for the reported endpoint.</p>
      <div class="proof-rail">
        <div class="proof-stage">
          <span class="proof-stage__number">01</span>
          <h3>Sieve</h3>
          <p>An explicit small factor proves an eliminated n composite.</p>
        </div>
        <div class="proof-stage">
          <span class="proof-stage__number">02</span>
          <h3>Fermat</h3>
          <p>A failed base-2 congruence is a compositeness witness.</p>
        </div>
        <div class="proof-stage">
          <span class="proof-stage__number">03</span>
          <h3>BPSW</h3>
          <p>GMP and sympy independently screen the rare passers.</p>
        </div>
        <div class="proof-stage">
          <span class="proof-stage__number">04</span>
          <h3>Lucas N+1</h3>
          <p>The fully factored N+1 and supplied witness prove primality.</p>
        </div>
      </div>
    </div>
  </section>

  <section class="content-section">
    <div class="content-section__inner">
      <p class="section-kicker">Certified results</p>
      <h2>The 11 least-n endpoints</h2>
      <div class="table-wrap">
        <table>
          <thead>
            <tr><th>k</th><th>least n</th><th>digits</th><th>Lucas D</th></tr>
          </thead>
          <tbody>
            <tr><td>1000004</td><td>1266</td><td>2350</td><td>5</td></tr>
            <tr><td>1000034</td><td>1176</td><td>2184</td><td>−19</td></tr>
            <tr><td>1000174</td><td>2101</td><td>3896</td><td>−19</td></tr>
            <tr><td>1000222</td><td>1679</td><td>3115</td><td>53</td></tr>
            <tr><td>1000834</td><td>3571</td><td>6617</td><td>61</td></tr>
            <tr><td>1000892</td><td>2678</td><td>4964</td><td>21</td></tr>
            <tr><td>1001020</td><td>2963</td><td>5492</td><td>21</td></tr>
            <tr><td>1001072</td><td>1014</td><td>1884</td><td>65</td></tr>
            <tr><td>1001248</td><td>1421</td><td>2637</td><td>5</td></tr>
            <tr><td>1001332</td><td>2731</td><td>5062</td><td>37</td></tr>
            <tr><td>1001464</td><td>1405</td><td>2608</td><td>5</td></tr>
          </tbody>
        </table>
      </div>
      <p class="scope-note">The base-71 conjecture itself is NOT solved — 19,068,114 k's remain open.</p>
    </div>
  </section>

  <section class="content-section content-section--surface">
    <div class="content-section__inner">
      <p class="section-kicker">Visual record</p>
      <h2>The problem and the answers</h2>
      <div class="visual-grid">
        <figure class="evidence-figure">
          <img src="{{ '/assets/images/riesel-problem-infographic.png' | relative_url }}" width="1019" height="1320" alt="Infographic explaining the Riesel problem in base 71">
          <figcaption>The open-question setting and base-71 search context.</figcaption>
        </figure>
        <figure class="evidence-figure">
          <img src="{{ '/assets/images/riesel-answers-infographic.png' | relative_url }}" width="1019" height="1370" alt="Infographic listing the 11 settled results and their proof path">
          <figcaption>The 11 results, digit counts, and four-stage proof path.</figcaption>
        </figure>
      </div>
      <div class="action-row">
        <a class="button" href="{{ '/assets/documents/riesel-base-71-infographics.pdf' | relative_url }}">Open the two-page PDF</a>
        <a class="button button--outline" href="{{ '/assets/images/x-header-3x1.png' | relative_url }}">Open the social header</a>
      </div>
    </div>
  </section>

  <section class="content-section">
    <div class="content-section__inner">
      <p class="section-kicker">Evidence package</p>
      <h2>Inspect the artifacts directly</h2>
      <div class="artifact-grid">
        <a class="artifact-card" href="{{ '/report.html' | relative_url }}"><strong>Full report</strong><span>Method, results, scope, and reproducibility.</span></a>
        <a class="artifact-card" href="{{ '/data/riesel-71-certificates.jsonl' | relative_url }}"><strong>Certificates</strong><span>11 machine-readable Lucas N+1 witnesses.</span></a>
        <a class="artifact-card" href="{{ '/data/riesel-71-raw-results.jsonl' | relative_url }}"><strong>Raw results</strong><span>11 SETTLED and 12 OPEN batch records.</span></a>
        <a class="artifact-card" href="{{ '/verification.html' | relative_url }}"><strong>Verification record</strong><span>Independent checks and the exact console output.</span></a>
      </div>
    </div>
  </section>

  <section class="content-section content-section--surface">
    <div class="content-section__inner">
      <p class="section-kicker">Provenance</p>
      <h2>Source and coordination</h2>
      <p>The source k-list is Gary Barnes's CRUS <code>remain-riesel-base71.zip</code>, file dated Nov 2 2025, from the <a href="https://www.noprimeleftbehind.net/crus/Riesel-conjectures.htm">CRUS Riesel conjectures page</a>. Status is stated only for the Oct 23 2025 base-page window and the Jul 22 2026 <a href="https://www.noprimeleftbehind.net/crus/Riesel-conjecture-reserves.htm">reservations snapshot</a>. CRUS coordination is via <a href="https://www.mersenneforum.org/node/6722">Mersenneforum thread 6722</a>.</p>
    </div>
  </section>
</div>
