# Kako da iz ovoga stvarno nešto naučiš

> Vremena je malo, pa učenje mora da bude jeftino po minutu.
> Ukupna cena svega ispod: **oko 90 minuta** raspoređenih kroz dva dana.

---

## 1. Razdvoj vodovod od materije

**Prekopiraj bez griže savesti:** harness, CMake, CLI, plot skripte, README šablon. Tu nema šta da se nauči osim CMake sintakse koju ionako guglaš svaki put.

**Napiši sam, bez izuzetka:** `reorder.cpp`, `tiled.cpp`, `parallel.cpp`, `transposed.cpp`. Ukupno oko 60 linija koda.

Ako tih 60 linija napišeš sam, projekat je odradio svoj posao — bez obzira što je sve ostalo prepisano.

---

## 2. Piši kernel iz obrasca pristupa, ne iz zapamćenog koda

Ovo je jedina tehnika koja stvarno razdvaja učenje od prekucavanja.

Pre nego što otvoriš editor: **papir**. Nacrtaj matricu, prstom prati koje elemente unutrašnja petlja dodiruje i kojim redom. Zadaci su na početku svakog kernel fajla i u `docs/teorija.md`.

Tek kad možeš da izgovoriš naglas rečenicu tipa *„za fiksno `i` i `k`, unutrašnja petlja prelazi jedan red bloka `B` i jedan red bloka `C`, oba sekvencijalno"* — tek tada piši kod.

Kod koji izađe iz te rečenice ostaje ti u glavi. Kod koji izađe iz gledanja u tuđi kod ne ostaje.

---

## 3. Predviđanje pre merenja je mehanizam učenja, ne ukras za README

Zato postoji sekcija 1 u `docs/notes.md`.

Kad zapišeš „očekujem 4×" pa dobiješ 6.8×, mozak registruje iznenađenje i to se pamti. Kad prvo vidiš broj, sve deluje očekivano unazad i ništa se ne zalepi.

Predviđanje mora da bude konkretno i sa obrazloženjem u jednoj rečenici. Ne „biće brže".

---

## 4. Pet mikro-eksperimenata

Svi rade na gotovom kodu — samo menjaš parametre. Automatizovani su u `scripts/run_experiments.sh`.

| # | Eksperiment | Vreme | Šta naučiš |
|---|---|---|---|
| 1 | `tiled` sa `bs = N` | 2 min | mora da bude ≈ `ikj`; ako nije, tvoj tiled ima bug |
| 2 | `tiled` sa `bs = 4` | 2 min | režija petlji nadjača lokalnost — postoji donja granica |
| 3 | N=1024 naspram N=1023 | 5 min | konfliktni promašaji kod stepena dvojke, uživo |
| 4 | transponuj `B` pa pusti `ijk` | 15 min | **poenta nije redosled petlji nego korak kroz memoriju** |
| 5 | `float` umesto `double` | 20 min | 16 elemenata po liniji → drugi optimalni blok |

**Prva tri su besplatna — uradi ih obavezno**, sva tri staju u sedam minuta i sva tri daju rečenicu za README.

**Četvrti je najvredniji.** Kernel `ijkT` je već ožičen u projekat (`src/kernels/transposed.cpp`), samo ga napiši. Ako posle njega umeš da objasniš zašto `ijk`-nad-`Bᵀ` i `ikj` daju sličan rezultat iz različitih razloga, razumeo si celu teoriju.

**Peti radi samo ako ti ostane vremena u utorak** — `experiments/float_vs_double.cpp`, samostalan fajl, jedna `g++` komanda.

Rezultate upiši u `docs/notes.md`, sekcija 3.

---

## 5. Kad zapneš: ne gledaj rešenje, spusti N

Pravilo: **20 minuta borbe**, pa:

```bash
./build/matmul 8 tiled 4 --verify
```

Greška u indeksima se na 8×8 vidi za minut, na 1024×1024 nikad.

Kernel fajlovi imaju **progresivne hintove** u komentarima — HINT 1 posle 10–20 min, HINT 2 posle 20–35 min. Koristi ih tim redom.

Traženje tuđeg koda je poslednji korak, ne prvi. Ne zato što je varanje, nego zato što ti oduzme upravo onaj deo koji bi zapamtio.

---

## 6. Test je usmeni, ne pisani

U utorak uveče, **bez gledanja u repo**, odgovori naglas:

- [ ] Zašto je `ikj` brže od `ijk`?
- [ ] Šta tačno tiling postiže?
- [ ] Kako si izabrao block size i šta bi bilo da si pogrešio?
- [ ] Zašto skaliranje na 4 niti nije 4×?
- [ ] Zašto gubiš od OpenBLAS-a?
- [ ] Šta je aritmetički intenzitet i gde ti je kernel na roofline dijagramu?
- [ ] Šta se menja za `float`?
- [ ] Šta su konfliktni promašaji i zašto stepen dvojke ume da škodi?
- [ ] Zašto `-O3 -march=native` menja odnose?
- [ ] Kako se ovo prenosi na GPU? A na Tenstorrent?

Ako negde zamucaš — vrati se u kod i pogledaj baš to mesto. Petnaest minuta, i to je jedina provera koja odgovara onome što će ti se desiti na intervjuu.

---

## 7. Šta NE pokušavaj da naučiš sada

SIMD intrinsike, operand packing, mikro-kerneli. To je novembar. Pokušaj da to stigneš u v1 pojede vreme grafika, i završiš i bez učenja i bez repoa.

---

## 8. Pravilo za README

**Nijedna rečenica koju ne umeš da odbraniš na intervjuu ne ide u README.**

Ako ne razumeš zašto je neki broj takav kakav jeste — ili istraži dok ne razumeš, ili napiši pošteno „nisam siguran zašto, pretpostavka je X". Drugo je potpuno prihvatljivo i zvuči bolje od lažne sigurnosti.
