# Plan rada: `matmul-tiling` v1

> **Sada:** nedelja 06.09.2026, 22:00 · **Push v1:** utorak 08.09 uveče · **Prijava:** 09.09
> **Raspoloživo:** ~2.5h večeras + ceo ponedeljak + ceo utorak ≈ 20 radnih sati

---

## 0. Šta se menja u odnosu na `projekat_matmul-tiling.md`

Dokument pretpostavlja da rad počinje u nedelju u 09:00. Izgubio si ceo taj dan, ali **nisi izgubio projekat** — samo teoretski blok od 3.5 sata. Tri izmene:

1. **Teorija se ne čita odvojeno.** Prepakovana je u `docs/teorija.md` unutar repoa, u komadima od 3–10 minuta, svaki sa zadatkom na papiru. Čitaš komad tačno pred fazu kojoj pripada. CS:APP ostaje za MUPS.
2. **Register blocking (Faza 4) se briše iz v1.** Bio je opcion; sada je isključen. Nema pregovora.
3. **Večeras se radi samo infrastruktura + naive + commit.** Cilj večeras nije performansa nego da sutra u 09:00 imaš repo koji se builda i meri, i da GitHub istorija počinje 06.09.

Infrastrukturu ne pišeš — priložena je gotova i testirana u `matmul-tiling-skeleton.zip`. **Kernele pišeš sam**, u fajlovima gde stoji `// TODO`.

### Šta je novo u skeletonu (u odnosu na prvu verziju)

| Fajl | Čemu služi |
|---|---|
| `docs/teorija.md` | cela teorija u komadima, sa zadacima na papiru |
| `docs/ucenje.md` | protokol učenja: šta prepisati, šta pisati sam, kako se testirati |
| `docs/notes.md` | prošireno: slotovi za predviđanja, mikro-eksperimente, usmeni test |
| kernel fajlovi | zadatak na papiru pre koda + **progresivni hintovi** (HINT 1 posle 10 min, HINT 2 posle 20) |
| `src/kernels/transposed.cpp` | eksperiment `ijkT` — najvredniji u projektu, ožičen i spreman |
| `scripts/run_experiments.sh` | pet mikro-eksperimenata, ukupno 7 minuta |
| `experiments/float_vs_double.cpp` | samostalan program, opciono za utorak |

Program te sada i čuva od najglupljeg gubitka vremena: ako kernel vrati same nule (prazan TODO), ispisuje upozorenje umesto da ti prikaže 4000 GFLOPS i pusti te da se raduješ.

---

## 1. Cut-line: šta se žrtvuje ako kasniš

Ovim redom, odozdo:

| Prioritet | Stavka | Ako nemaš vremena |
|---|---|---|
| 1 (nikad) | naive + ikj + tiled, verifikovani | — |
| 2 (nikad) | README sa brojevima i objašnjenjem | — |
| 3 (nikad) | Predviđanje BS-a upisano pre merenja | — |
| 4 (nikad) | Mikro-eksperimenti E1–E3 (7 min ukupno) | — |
| 5 | GFLOPS-vs-N i sweep grafik | — |
| 6 | Eksperiment E4 (`ijkT`) | preskoči, ali je šteta |
| 7 | OpenMP | preskoči, napiši u Limitations |
| 8 | `perf` brojači | 20 min i dalje, pa plan B |
| 9 | Roofline | zameni pasusom teksta o AI |
| 10 | OpenBLAS poređenje | preskoči |
| 11 | `-O3 -march=native` posebno | preskoči |
| 12 | Eksperiment E5 (`float`) | preskoči |
| ~~13~~ | ~~register blocking~~ | **već obrisano** |

**Repo bez OpenMP-a i bez `perf`-a, ali sa čistim merenjem tri verzije i README-om koji objašnjava zašto — i dalje prolazi screening.** Repo sa šest verzija i bez analize — ne prolazi.

---

## 2. Protokol učenja (pun tekst u `docs/ucenje.md`)

Pet pravila, ukupna cena oko 90 minuta kroz dva dana. Bez njih ovo je prekucavanje.

**2.1. Razdvoj vodovod od materije.** Harness, CMake, CLI, plot skripte — prekopiraj bez griže savesti. Sve učenje sedi u četiri fajla i oko 60 linija koda: `reorder.cpp`, `tiled.cpp`, `parallel.cpp`, `transposed.cpp`. Ako tih 60 linija napišeš sam, projekat je odradio posao.

**2.2. Papir pre editora.** Svaki kernel fajl počinje zadatkom: nacrtaj matricu, prstom prati šta unutrašnja petlja dodiruje. Piši kod tek kad umeš naglas da izgovoriš rečenicu tipa *„za fiksno `i` i `k`, unutrašnja petlja prelazi jedan red bloka `B` i jedan red bloka `C`, oba sekvencijalno"*. Kod koji izađe iz te rečenice ostaje. Kod koji izađe iz gledanja u tuđi kod ne ostaje.

**2.3. Predviđanje pre merenja** — mehanizam, ne ukras. Iznenađenje se pamti, potvrda unazad ne. Slotovi su u `docs/notes.md` sekcija 1.

**2.4. Kad zapneš: 20 minuta borbe, pa spusti N.** `./build/matmul 8 tiled 4 --verify` — greška u indeksima se na 8×8 vidi za minut, na 1024×1024 nikad. Zatim HINT 1, pa HINT 2 iz komentara u fajlu. Tuđi kod je poslednji korak, ne prvi.

**2.5. Test je usmeni.** U utorak uveče, bez gledanja u repo, deset pitanja iz `docs/notes.md` sekcija 5. Gde zamucaš — vrati se baš na to mesto u kodu.

---

## 3. NEDELJA 06.09, 22:00–00:30

### 22:00–22:15 · Paketi i provera okruženja

```bash
sudo apt update
sudo apt install -y build-essential cmake git linux-tools-common linux-tools-generic
sudo apt install -y libopenblas-dev python3-venv python3-pip

g++ --version && cmake --version && perf --version

cat /proc/sys/kernel/perf_event_paranoid
sudo sysctl -w kernel.perf_event_paranoid=1     # ako je > 1
```

Ako `perf` ne radi (WSL2, kontejner) — **ne diraj to večeras.** Zabeleži i idi dalje.

### 22:15–22:40 · Mašina, teorija sekcija 1, predviđanje

```bash
unzip matmul-tiling-skeleton.zip && cd matmul-tiling
chmod +x scripts/*.sh
./scripts/collect_machine.sh
cat docs/machine.md
```

Otvori `docs/teorija.md`, pročitaj **sekciju 1** (2 min) i popuni kolonu „Tvoja mašina" u tabeli. Zatim, **pre ijednog merenja**, popuni `docs/notes.md` sekcije 0 i 1:

```
3 · BS² · 8 ≤ L1_bytes    →    BS ≤ sqrt(L1_bytes / 24)
```
Za L1 = 32 KB: `BS ≤ 36.9` → **predviđanje BS = 32**. Za 48 KB: `BS ≤ 45` → 32 ili 40.

Upiši i peak GFLOPS (`takt × jezgra × 2 FLOP/ciklus` za skalarni double) i predviđanja za ikj (~3–5×), tiled (dodatnih ~1.5–2×), skaliranje (~0.6–0.8 × jezgara).

> Ovo je jedina stvar koju posle **ne možeš** da rekonstruišeš. Pet minuta sada vredi više od sata sutra.

### 22:40–22:55 · Git od prve minute

```bash
git config --global user.name  "Tvoje Ime"
git config --global user.email "tvoj@email"

git init -b main
git add .
git commit -m "Add project skeleton, CMake build, timing harness and CLI"

gh repo create matmul-tiling --public --source=. --push
# ručno: git remote add origin git@github.com:<user>/matmul-tiling.git && git push -u origin main

gh repo edit --description "Dense matmul in C++ from naive to cache-blocked and parallel, with cache-counter measurements and a roofline analysis" \
  --add-topic cpp --add-topic performance --add-topic optimization \
  --add-topic cache --add-topic memory-hierarchy --add-topic openmp \
  --add-topic hpc --add-topic benchmarking --add-topic roofline
```

Otvori `LICENSE` i zameni `<Tvoje Ime>`.

### 22:55–23:20 · Build i prvi brojevi

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```
U izlazu CMake-a proveri: `OpenMP: ON`, `BLAS: ...`, `Flags: -O2`.

```bash
./build/matmul 256 naive
./build/matmul 512 naive
./build/matmul 1024 naive        # ~2–8 s po prolazu
./build/matmul 50 bandwidth      # STREAM triad, ~400 MB po nizu
```

Zapiši naive GFLOPS i propusnost u `notes.md`. `ikj`, `tiled`, `parallel`, `ijkT` vraćaju nule dok ih ne napišeš — program će ti to i reći upozorenjem. Normalno.

```bash
git add -A && git commit -m "Add machine spec, baseline naive measurement and block-size prediction"
```

### 23:20–23:35 · Teorija sekcije 2–4 + papir

`docs/teorija.md`, sekcije 2, 3 i 4. Deset minuta čitanja, pet minuta crtanja. **Nacrtaj matricu 8×8 i prstom prati `ijk`, pa `ikj`.** Ne preskači crtanje da bi „uštedeo" pet minuta — to je ceo transfer u glavu.

### 23:35–00:20 · Faza 2: `ikj`, prvi kernel koji pišeš sam

Otvori `src/kernels/reorder.cpp`. Zadatak na papiru je već gore odrađen, specifikacija je u komentaru. Tri stvari koje se najčešće promaše:
1. `C` se nulira (već je u skeletonu) — sada akumuliraš `+=`, ne dodeljuješ
2. `A[i*N+k]` izvuci u `const double a` van unutrašnje petlje
3. `C[i*N + j]`, ne `C[i*N + k]` — **indeks izlaza nikad ne sadrži `k`**

```bash
cmake --build build -j
./build/matmul 512 ikj --verify        # mora da kaže OK
./build/matmul 1024 ikj
```
Ako FAIL: `./build/matmul 8 ikj --verify`. Ne debuguj na N=1024. Posle 10 minuta — HINT 1 u fajlu, posle 20 — HINT 2.

```bash
for N in 128 256 512 1024; do ./build/matmul $N ikj; done
git add -A && git commit -m "Add ikj loop reorder: <X>x over naive at N=1024"
git push
```

U `docs/notes.md` sekcija 2: očekivano vs izmereno vs zašto misliš da je razlika takva. **Pre nego što bilo šta guglaš.**

### 00:20–00:30 · Gasi

Pročitaj zadatak na papiru sa vrha `src/kernels/tiled.cpp` pre spavanja. Mozak radi noću na šest ugnježdenih petlji — ovo nije metafora, uradi to.

**Minimum večeras:** repo na GitHub-u, build radi, naive izmeren, predviđanje BS-a upisano. `ikj` je bonus — ako u 23:50 ne ide, spavaj i uradi ga ujutru za 40 min.

---

## 4. PONEDELJAK 07.09

Pre početka: **zatvori browser i sve ostalo, uključi laptop u struju, ugasi Docker/Slack/Spotify.** Termalno prigušivanje pojede razliku koju meriš.

### 09:00–09:20 · Teorija sekcija 5 + papir (10 min čitanja, 10 crtanja)

`docs/teorija.md` sekcija 5. Zadatak: matrica 8×8 podeljena na blokove 4×4, fiksiraj blok `C[ii][jj]`, odredi koji blokovi `A` i `B` su potrebni. Odgovor mora da bude *„cela blok-vrsta A i cela blok-kolona B"* — otuda petlja po `kk`.

### 09:20–10:00 · Dovrši `ikj` ako nije gotov

### 10:00–12:30 · Faza 3: tiling — najteži deo koda

Šest ugnježdenih petlji. Ide ujutru zato što se indeksi lako pomešaju.

**Ne prekucavaj — napiši iz strukture koju si nacrtao, pa se bori sa granicama.** Cilj je da te indeksi zabole, jer ćeš zato umeti da odgovoriš na svako pitanje o njima. Hintovi su u fajlu i koriste se tim redom, posle 20 odnosno 35 minuta.

Debug ritual posle svake izmene:
```bash
cmake --build build -j && ./build/matmul 64 tiled 16 --verify
./build/matmul 500 tiled 32 --verify      # N NIJE deljivo sa bs -- hvata min() greške
./build/matmul   8 tiled  4 --verify      # gresku vidiš rukom
```

Tri najčešće greške: `A[ii*N+k]` umesto `A[i*N+k]`; unutrašnja petlja `j < bs` umesto `j < min(jj+bs, N)`; zaboravljeno nuliranje `C`.

Bonus koji košta dva minuta: probaj i redosled blokovskih petlji `ii, jj, kk` umesto `ii, kk, jj`. Radi, ali je sporije — zapiši razliku, to je besplatan nalaz za README.

```bash
git add -A && git commit -m "Add cache blocking with runtime block size; verified against naive"
```

### 12:30–13:00 · Block-size sweep

```bash
./scripts/run_sweep.sh 1024
```

Uporedi izmereni optimum sa predviđanjem. **Oba ishoda su dobra:**
- poklopilo se → „predvideo iz veličine L1, potvrdio merenjem"
- nije → veći optimum znači da si blokirao za L2; manji znači konfliktne promašaje (stepen dvojke + ograničena asocijativnost); ili je kompajler nešto sam uradio

Upiši u `notes.md` odmah.

```bash
git add -A && git commit -m "Add block-size sweep: predicted BS=32 from L1, measured optimum BS=<X>"
```

### 13:00–13:15 · **Mikro-eksperimenti E1–E3** (7 minuta mašinskog vremena)

```bash
BS=<tvoj_optimum> N=1024 ./scripts/run_experiments.sh
```

Tri nalaza, svaki po jedna rečenica u `notes.md` sekcija 3:

- **E1 — `tiled` sa `bs = N`** mora da bude ≈ `ikj`. Ako nije, tvoj tiled ima bug koji `verify` nije uhvatio. Ovo je najjeftiniji sanity test u projektu.
- **E2 — `tiled` sa `bs = 4`**: blok savršeno staje u L1, a sporo je. Zašto? Režija petlji i gubitak prefetch-a nadjačaju lokalnost. Postoji **donja** granica, ne samo gornja.
- **E3 — N = 1023 vs 1024 vs 1025**: konfliktni promašaji uživo. Kod stepena dvojke se redovi mapiraju na iste setove keša i izbacuju jedan drugog. Efekat je često jači kod `ikj` nego kod `tiled` — blokiranje ga delimično sakriva.

### 13:15–14:00 · Pauza. Stvarno.

### 14:00–14:20 · **Eksperiment E4: `ijkT`** — najvredniji u projektu

`src/kernels/transposed.cpp`, petnaest minuta. Transponuješ `B` unapred pa pustiš **originalni** `ijk` redosled.

Rezultat: `ijk` odjednom postaje brz. Zaključak koji nosiš na intervju: **poenta nije redosled petlji, poenta je korak kroz memoriju.** `ikj` menja petlje, `ijkT` menja podatke, oba postižu isto — sekvencijalni tok umesto skokovitog.

Dva pitanja na koja moraš da odgovoriš naglas i da ih upišeš u `notes.md`:
- Zašto ova dva pristupa daju sličan rezultat iz različitih razloga?
- Šta transponovanje košta i zašto se ipak isplati? *(O(N²) rada da bi se popravilo O(N³) pristupa.)*

```bash
./build/matmul 512 ijkT --verify
./build/matmul 1024 ijkT
git add -A && git commit -m "Add transposed-B experiment: stride, not loop order, is what matters"
```

### 14:20–15:00 · OpenBLAS referenca

```bash
OPENBLAS_NUM_THREADS=1 ./build/matmul 1024 blas    # fer poređenje sa tvojom 1 niti
./build/matmul 1024 blas                            # svi threadovi
```
Poređenje tvoje jedne niti sa BLAS-ovih osam nije poređenje. Zapiši oba.

### 15:00–16:00 · Faza 5: OpenMP

Pročitaj `docs/teorija.md` sekcija 7 (3 min). Pitanje sa vrha `parallel.cpp` odgovori **pre** koda: zašto je bezbedno po `ii`/`jj`, a katastrofalno po `kk`.

```bash
cmake --build build -j
./build/matmul 512 parallel 32 --verify
export OMP_PROC_BIND=close OMP_PLACES=cores
./scripts/run_threads.sh 1024 <tvoj_bs>
git add -A && git commit -m "Add OpenMP parallelisation over output blocks: <X>x on <N> cores"
```

### 16:00–16:45 · Pun run svih merenja

```bash
./scripts/run_all.sh          # ~10–20 min; NE diraj mašinu dok radi
```
Idi na kafu. Svaki klik dok ovo radi je šum u tvojim brojevima.

### 16:45–17:30 · Večera

### 17:30–19:00 · `perf` brojači

```bash
./scripts/run_perf.sh 1024 <tvoj_bs>
cat results/perf_raw.txt
```
Prepiši u `results/perf_counters.md`: L1-dcache-load-misses, miss rate, IPC. **Ovo je dokaz, ne ilustracija** — miss rate koji pada sa ~40% na ~3% dok vreme pada 20× ne traži da ti se veruje.

**Ako `perf` ne radi — 20 minuta maksimalno, pa plan B:** efektivna propusnost po verziji (`bajtovi/sekunda`, za naive ≈ `N³·64`, za tiled ≈ `N³·24/BS`), upoređena sa STREAM merenjem. U README napiši zašto brojači nedostaju.

### 19:00–19:30 · Propusnost + roofline brojevi

```bash
./build/matmul 50 bandwidth
```
Nizovi moraju biti bar 4× veći od L3, inače meriš keš a ne DRAM. Pročitaj `docs/teorija.md` sekcija 6 i izračunaj *ridge point* svoje mašine.

### 19:30–20:30 · Grafici

```bash
python3 -m venv .venv && . .venv/bin/activate
pip install pandas matplotlib
# u scripts/plot.py postavi PREDICTED_BS na svoju vrednost
python scripts/plot.py
python scripts/plot_roofline.py --peak <peak> --bw <bandwidth>
git add -A && git commit -m "Add perf counters, STREAM bandwidth and plots" && git push
```

### 20:30–21:00 · `docs/notes.md`

Popuni sekcije 2, 3 i 4 do kraja. Ovo je sirovina za README i za intervju; sutra ujutru se nećeš setiti.

---

## 5. UTORAK 08.09

### 09:00–09:45 · `-O3 -march=native` kao poseban eksperiment

```bash
cmake -B build-fast -DCMAKE_BUILD_TYPE=Release -DAGGRESSIVE=ON
cmake --build build-fast -j
for v in naive ikj ijkT tiled parallel; do ./build-fast/matmul 1024 $v <bs>; done
```
Može auto-vektorizovati naivnu petlju i **promeniti odnose**. To je nalaz, ne smetnja — pokazuje da razumeš da kompajler učestvuje u eksperimentu. Glavna tabela ostaje `-O2`.

### 09:45–11:30 · README

Šablon je u repou, sa `_` na mestima koja popunjavaš. Redosled: **prvo tabela sa brojevima, pa „What the numbers say", pa ostalo.** Neko ko skroluje daje ti 40 sekundi.

Ubaci i E1–E4 kao kratke rečenice — mikro-eksperimenti su ono što tvoju verziju razlikuje od pedeset drugih matmul repoa.

**Pravilo: nijedna rečenica koju ne umeš da odbraniš ne ide u README.** Ako ne razumeš zašto je broj takav, ili istraži dok ne razumeš, ili napiši pošteno „pretpostavka je X". Drugo zvuči bolje od lažne sigurnosti.

### 11:30–12:00 · `results/perf_counters.md` u finalnu formu

### 12:00–13:00 · Pauza

### 13:00–14:15 · **Finalni čist run** — ovi brojevi idu u README

Sve zatvoreno, na struji:
```bash
./scripts/run_all.sh
./scripts/run_sweep.sh 1024
./scripts/run_threads.sh 1024 <bs>
python scripts/plot.py
python scripts/plot_roofline.py --peak <..> --bw <..>
```

### 14:15–15:15 · Popuni sve `_` u README-u

Pretraži na `_` i na `<` — nijedan placeholder ne sme da ostane. Placeholder je gori nego da sekcije nema.

### 15:15–16:00 · Higijena repoa

```bash
git status && git add -A
git commit -m "Write up findings: 20x from data movement alone, roofline analysis"
git push
git ls-files results/            # jesu li PNG-ovi commitovani?
```
Na GitHub-u u browseru proveri: slike se stvarno prikazuju, Description i Topics postavljeni, LICENSE ima tvoje ime, repo pinovan prvi, `docs/notes.md` i `docs/teorija.md` su u repou (dnevnik sa predviđanjima je deo priče, ne sramota).

### 16:00–16:45 · Čitanje tuđim očima

Pročitaj README kao neko ko te ne poznaje i ima 40 sekundi.

### 16:45–17:45 · Rezerva — ili eksperiment E5 ako ti se radi

```bash
g++ -std=c++17 -O2 -o /tmp/fvd experiments/float_vs_double.cpp && /tmp/fvd 1024
```
Predviđanje pre pokretanja: `BS_float / BS_double = sqrt(2) ≈ 1.41`, jer u istu liniju staje 16 `float`-ova umesto 8 `double`-ova.

**Ne dodaj šestu optimizaciju umesto ovog sata.** Nedovršena analiza je gora od jedne optimizacije manje.

### 18:00–18:30 · Usmeni test

Bez gledanja u repo, naglas, deset pitanja iz `docs/notes.md` sekcija 5. Gde zamucaš — vrati se u kod i pogledaj baš to mesto. Ovo je jedina provera koja odgovara onome što će ti se desiti na intervjuu.

**60 sekundi za screening:**
> Isti algoritam, pet implementacija, oko 20× razlike. Poenta je da FLOP-ovi nisu ono što košta — kretanje podataka jeste. Izmerio sam to i cache brojačima, ne samo štopericom, a optimalni block size sam predvideo iz veličine L1 pre nego što sam ga izmerio.

### 18:30–19:00 · Transfer — tri pasusa svojim rečima

`docs/teorija.md` sekcija 8, zadatak na kraju. Napiši bez gledanja igde: kako se ovo prenosi na GPU, kako na Tenstorrent, i zašto su izabrali eksplicitni model umesto koherentnog keša.

**To je razlog zašto ovaj projekat postoji.** Ako to izgovoriš mirno i tačno, tehnički deo prijave je prošao bez obzira na ostalo.

---

## 6. Poruke commita — pun redosled

```
Add project skeleton, CMake build, timing harness and CLI
Add machine spec, baseline naive measurement and block-size prediction
Add ikj loop reorder: 4.1x over naive at N=1024
Add cache blocking with runtime block size; verified against naive
Add block-size sweep: predicted BS=32 from L1, measured optimum BS=32
Add micro-experiments: degenerate block size, tiny blocks, power-of-two conflict misses
Add transposed-B experiment: stride, not loop order, is what matters
Add OpenBLAS reference kernel for a single-threaded upper bound
Add OpenMP parallelisation over output blocks: 3.2x on 4 cores
Add perf counters: L1 miss rate 38% -> 3.1%
Add STREAM-triad bandwidth measurement and roofline plot
Measure -O3 -march=native separately: ranking changes for naive
Write up findings: 20x from data movement alone, roofline analysis
```

Na engleskom, konkretne, sa brojevima. Ne `update`, ne `fix`, ne `wip`. **Commituj tokom dana** — istorija se vidi i pokazuje tempo.

---

## 7. Ako zapne

**`--verify` javlja FAIL** → spusti na N=64, bs=16. Ako i tamo pada, greška je u indeksima. Ako N=64 prolazi a N=500 pada, greška je u `std::min(ii+bs, N)`.

**Apsurdno visok GFLOPS** → program ispisuje upozorenje: checksum je 0, kernel je prazan TODO.

**Brojevi skaču između runova** → nešto radi u pozadini, ili si na bateriji, ili se CPU prigušuje. Ako ostane, prijavi raspon pošteno — to je iskrenije od skrivanja.

**`perf` ne radi** → `sudo sysctl -w kernel.perf_event_paranoid=1`, `sudo apt install linux-tools-$(uname -r)`. Na WSL2 često nema šanse. 20 minuta pa plan B.

**`cblas.h: No such file`** → `sudo apt install libopenblas-dev`, obriši `build/`, ponovi `cmake -B build`. Ako i dalje ne nalazi: `target_include_directories(matmul PRIVATE /usr/include/x86_64-linux-gnu)`.

**OpenMP sporiji od tiled** → paralelizovao si po `kk` (netačno), ili je `bs` toliko veliko da ima manje blokova nego niti, ili nuliraš `C` unutar paralelnog regiona. `--verify` prvo.

**`naive` na N=2048 traje večno** → ~30 s po prolazu je očekivano; `run_all.sh` mu već daje `--reps 1 --warmup 1`.

---

## 8. Šta NE raditi

- Ne piši sopstveni BLAS. Cilj je razumevanje, ne konkurencija.
- Ne ulazi u SIMD intrinsike. Jama bez dna, i nije v1.
- **Ne prekucavaj kernele** — ni iz dokumenta, ni sa interneta, ni iz ovog plana. Skeleton fajlovi imaju specifikaciju i hintove, ne rešenje, namerno.
- Ne preskači zadatke na papiru da bi „uštedeo" pet minuta.
- Ne meri na bateriji.
- Ne tvrdi nigde da si pobedio BLAS.

---

## 9. Checklist pre pusha v1

**Merenje**
- [ ] `./scripts/run_all.sh` prolazi bez grešaka na čistoj mašini
- [ ] Sve verzije prolaze `--verify` (max abs diff < 1e-9)
- [ ] Sva tri grafika se prikazuju na GitHub-u
- [ ] Kompajlerske zastavice prijavljene (`-O2` glavno, `-O3 -march=native` posebno)

**Učenje**
- [ ] Kernele si napisao sam, nijedan nije prekucan
- [ ] `docs/notes.md` sekcija 1 sadrži predviđanja upisana **pre** merenja
- [ ] E1–E3 urađeni i objašnjeni u `notes.md` sekcija 3
- [ ] Usmeni test prošao bez mucanja na više od dva pitanja
- [ ] Tri pasusa o transferu na GPU/Tenstorrent napisana svojim rečima

**Repo**
- [ ] README nema nijedan `_` ili `<placeholder>`
- [ ] Description + 9 topics + MIT LICENSE sa tvojim imenom
- [ ] `docs/machine.md` sadrži pun `lscpu` izlaz
- [ ] Sekcija Limitations postoji i iskrena je
- [ ] Cross-link sa `inference-bench` u oba README-a
- [ ] Repo pinovan na profilu, prvi
