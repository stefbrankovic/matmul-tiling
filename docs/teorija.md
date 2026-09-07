# Teorija — sve što ti treba, na jednom mestu

> Ovo se ne čita u komadu. Čita se u delovima, tačno pred fazu kojoj pripada.
> Referenca za posle: CS:APP (Bryant & O'Hallaron), 6.2–6.6.
> **Svaka sekcija ima zadatak na papiru. Zadatak nije opcion — on je učenje.**

---

## 1. Memorijska hijerarhija · čitaj pre Faze 1

| Nivo | Veličina | Latencija | Tvoja mašina |
|---|---|---|---|
| Registri | ~1 KB | 0 ciklusa | ~1 KB |
| L1d | 32–48 KB/jezgru | ~4 ciklusa | 48 KB (49152 B) / jezgru |
| L2 | 256 KB–2 MB/jezgru | ~12–20 ciklusa | 512 KB (524288 B) / jezgru |
| L3 | 8–32 MB, deljen | ~40–70 ciklusa | 4 MB, deljen |
| DRAM | GB | ~200–400 ciklusa | DRAM |

Razlika L1 → DRAM je oko **100×**. Ako ti unutrašnja petlja ide u DRAM, ništa drugo ne pomaže — ni bolji algoritam, ni bolji kompajler, ni više niti.

```bash
lscpu | grep -i cache
getconf -a | grep CACHE      # veličina cache line-a i asocijativnost
```

**Zadatak (2 min):** popuni kolonu iznad iz izlaza gornjih komandi. Ovi brojevi ti trebaju za predviđanje i za README.

---

## 2. Cache line · čitaj pre Faze 2

Memorija se ne čita po bajtu nego po **cache line-u**, tipično 64 bajta = 8 `double` vrednosti.

- Sekvencijalno čitanje: jedan promašaj donese 8 korisnih vrednosti → 1 promašaj na 8 pristupa
- Korak veći od 64 bajta: svaki pristup je nov promašaj, i **7/8 dovučene propusnosti se baca**

Uz to, hardverski **prefetcher** prepoznaje sekvencijalne obrasce i dovlači unapred. Kod skokovitih pristupa ne radi ništa.

**Zadatak (3 min):** za `double` i liniju od 64 B, koliko cache linija dodirneš čitajući ceo red matrice 1024×1024? A celu kolonu? *(Odgovori: 128 linija i 1024 linija — osmostruko više saobraćaja za istu količinu podataka.)*

---

## 3. Row-major i zašto `ijk` propada · čitaj pre Faze 2

C i C++ čuvaju 2D nizove **po vrstama**. Element `A[i][j]` je na adresi `A + i*N + j`.

```
for i: for j: for k:  C[i][j] += A[i][k] * B[k][j]
```

Unutrašnja petlja ide po `k`:

| Pristup | Korak po `k` | Posledica |
|---|---|---|
| `A[i*N + k]` | 1 element (8 B) | sekvencijalno, prefetcher radi ✅ |
| `B[k*N + j]` | N elemenata (N·8 B) | nova cache linija svaki put ❌ |
| `C[i*N + j]` | 0 | ostaje u registru ✅ |

Za N=1024 i `double`: korak kroz `B` je **8192 bajta**. Svaki pristup dovuče 64 B, koristi 8, baci 56. I dok se vratiš na `B[k][j+1]`, ta linija je odavno izbačena.

**To je ceo problem.** Broj operacija je identičan u svim verzijama — `2N³`. Razlika je isključivo u tome gde su podaci kad zatrebaju.

**Zadatak na papiru (5 min):** matrica 8×8, nacrtaj je. Prstom prati šta unutrašnja petlja po `k` dodiruje u `A` i u `B`. Nacrtaj strelice.

---

## 4. Zašto `ikj` popravlja stvar · čitaj pre Faze 2

```
for i: for k: for j:  C[i][j] += A[i][k] * B[k][j]
```

| Pristup | Korak po `j` | Posledica |
|---|---|---|
| `A[i*N + k]` | 0 | konstanta, u registru ✅ |
| `B[k*N + j]` | 1 element | sekvencijalno ✅ |
| `C[i*N + j]` | 1 element | sekvencijalno ✅ |

Dva sekvencijalna toka umesto jednog skokovitog. **Cena:** `C` se sada čita i upisuje N puta umesto jednom. I dalje pobeđuje, jer je sekvencijalni pristup toliko jeftiniji.

Tipično ubrzanje: **3–5×**, bez ijedne promene u algoritmu.

**Zadatak (5 min):** ponovi crtanje iz sekcije 3, ali sa `ikj`. Zatim izgovori naglas, u jednoj rečenici, šta se promenilo. Tek posle toga otvaraj `src/kernels/reorder.cpp`.

---

## 5. Blocking i formula koju izvodiš sam · čitaj pre Faze 3

`ikj` je bolji, ali za veliko N i dalje ne staje: dok obiđeš celu vrstu `B` (za N=1024 to je 8 KB), početak je izbačen iz L1.

Rešenje: obrađuj **blokove** BS×BS. Tri bloka (A, B, C) treba istovremeno da stanu u L1:

```
3 · BS² · sizeof(double) ≤ L1_veličina
```

Za L1 = 32 KB i `double`:
```
3 · BS² · 8 ≤ 32768   →   BS² ≤ 1365   →   BS ≤ 36
```

**Predviđanje: optimum oko BS = 32.**

> **Upiši svoje predviđanje u `docs/notes.md` PRE nego što pustiš sweep.**
> Ovo nije formalnost. Kad zapišeš „očekujem 4×" pa dobiješ 6.8×, mozak
> registruje iznenađenje i to ostane. Kad prvo vidiš broj, sve deluje
> očekivano unazad i ništa se ne zalepi.

Dva ishoda, oba dobra:
- **Poklopilo se** → „predvideo iz veličine L1, potvrdio merenjem"
- **Nije se poklopilo** → objasni zašto:
  - veći izmereni optimum → zapravo si blokirao za L2, ne za L1
  - manji → **konfliktni promašaji**: kod stepena dvojke i ograničene asocijativnosti blokovi se mapiraju na iste setove keša i izbacuju jedan drugog
  - kompajler je nešto sam uradio (auto-vektorizacija, unrolling)

Nepoklapanje sa dobrim objašnjenjem je jače od poklapanja bez objašnjenja.

**Zadatak na papiru (10 min):** matrica 8×8 podeljena na blokove 4×4. Fiksiraj blok `C[ii][jj]`. Koji blokovi `A` i `B` su ti potrebni da ga izračunaš do kraja? *(Cela blok-vrsta A i cela blok-kolona B — otuda petlja po `kk`.)*

---

## 6. Aritmetički intenzitet i roofline · čitaj pre grafika

**AI = FLOP-ova po bajtu dovučenom iz memorije.**

Naivna verzija: po iteraciji unutrašnje petlje radiš 2 FLOP-a (množenje + sabiranje), a dovučeš celu liniju od 64 B zbog `B`:
```
AI_naive ≈ 2 / 64 ≈ 0.03 FLOP/bajt
```

Blokirana: blok od BS² elemenata se ponovo koristi BS puta:
```
FLOP-ova po bloku = 2 · BS³
bajtova dovučeno  = 3 · BS² · 8
AI_tiled ≈ 2·BS³ / (24·BS²) = BS / 12
```
Za BS=32: **AI ≈ 2.7 FLOP/bajt**, oko 90× više.

**Roofline.** Mašina ima dva plafona:
- **Računski:** maksimalni GFLOPS (horizontalna linija)
- **Memorijski:** propusnost × AI (kosa linija)

Postignuti GFLOPS = min(dva plafona). Levo od preseka si **memory-bound**, desno **compute-bound**.

Naivna verzija sedi duboko na memorijskom plafonu. Blocking diže AI i pomera te udesno. **To je koncept oko kojeg je izgrađena cela industrija AI akceleratora.**

Dva broja koja ti trebaju:
```
peak_GFLOPS = takt_GHz × fizička_jezgra × FLOP_po_ciklusu
   skalarni double, -O2       → ~2 FLOP/ciklus/jezgro
   AVX2 + FMA (-march=native) → 16 FLOP/ciklus/jezgro
bandwidth = ./build/matmul 50 bandwidth      (STREAM triad)
```
Prijavi kako si ih dobio. Procena sa objašnjenjem je poštenija od tuđeg broja.

**Zadatak (5 min):** izračunaj gde je *ridge point* tvoje mašine (`peak/bandwidth`). Da li je tvoj tiled kernel levo ili desno od njega? Šta to znači za sledeću optimizaciju koju bi radio?

---

## 7. OpenMP · čitaj pre Faze 5

```cpp
#pragma omp parallel for collapse(2) schedule(static)
```
- `parallel for` — deli iteracije po nitima
- `collapse(2)` — spaja dve spoljne petlje u jedan iteracioni prostor (više posla za deljenje)
- `schedule(static)` — jednaki blokovi, bez režije; dobro kad je posao uniforman

**Race condition:** paralelizuj po `ii`/`jj` (izlazni blokovi su nezavisni), **nikad po `kk`** (više niti bi akumuliralo u isti `C[i][j]`).

Skaliranje **neće** biti linearno. Dva razloga i oba moraš da umeš da izgovoriš:
1. Propusnost memorije je **deljeni resurs** — kad je kernel bandwidth-bound, dodatne niti se takmiče za istu magistralu umesto da dodaju protok
2. Hyperthread **nije** fizičko jezgro — dve niti na istom jezgru dele iste izvršne jedinice i isti L1

---

## 8. Transfer — zašto ovo uopšte radiš

Ovo je poslednja stavka i jedina zbog koje projekat postoji u tvom portfoliju.

**Na GPU:** deljena memorija umesto L1, koalescirani pristup umesto sekvencijalnog, occupancy umesto broja niti. **Tiling ostaje identična ideja.**

**Na Tenstorrent:** nema keš koherencije. Programer **eksplicitno** premešta tile-ove između DRAM-a i lokalne memorije jezgra. Tile je prvorazredni pojam jezika, ne trik optimizacije.

> Sve što si ovde radio nesvesno, oslanjajući se na keš, tamo pišeš rukom.
> Ako razumeš zašto blocking radi na CPU kešu, razumeš i zašto su izabrali
> eksplicitni model: programer radi svesno ono što je kod tebe keš radio
> implicitno — i zato deterministički, bez promašaja koje ne kontrolišeš.

**Zadatak (15 min, u nedelju kad sve bude gotovo):** napiši ta tri pasusa svojim rečima, bez gledanja igde. To je jedini pravi test da li si naučio.
