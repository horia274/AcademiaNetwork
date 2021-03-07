# NetworkAcademy

## Enunt

Acest proiect reprezinta un foarte bun exercitiu de la cursul de **Structuri
de date**. Este vorba despre un sistem ce agregă informații despre articole
științifice și răspunde la diferite query-uri. Mai multe detalii se pot gasi
[aici](https://ocw.cs.pub.ro/courses/sd-ca/teme/tema3-2020).

## Continut repo

Implementarea propriu-zisa a acestei aplicatii se gaseste in fisierul **publications.c**.
Celelalte fisiere sursa folosite reprezinta propriile mele implementari pentru
diferitele structuri de date folosite.

Directoarele din repo contin urmatoarele informatii:

* parsed_papers - contine fisierele cu informatii despre lucrarile stiintifice
* checker - sursele care testeaza corectitudinea implementarii
* parson - contine fisierele sursa ce au rolul de a parsa inputul din fisierele
din *parsed_papers*
* tests_in - testele de input
* tests_ref - ce ar trebui sa obtin in urma rularii

## Modalitate de rulare

Pentru a compila, folosesc urmatoarele comenzi:

* make - pentru a obtine fisierul obiect *publications.o*
* make -f Makefile_Academia - pentru a obtine executabilul AcademiaNetwork

Pentru a rula aplicatia folosesc o comanda de tipul:

* ./AcademiaNetwork tests.in/*nume-test* tests.ref/*nume-test*

Daca nu se obtine niciun output, inseamna ca rezultatul obtinut este identic
cu cel din fisierul tests.ref/*nume-test*, ceea ce arata corectitudinea
implementarii.

## Continut structura PublData

Structurile de date folosite sunt urmatoarele:

* ht_papers -> dictionar pe id-uri de articole care da informatii despre un
articol(id, titlu, an, numarul de referinte etc (struct paper)).

* ht_citations -> dictionar pe id-uri de articole care da o lista cu id-urile
articolelor care citeaza id-ul cheie.

* ht_venues -> dictionar pe jurnale care da o lista cu id-urile articolelor
care au fost publicate in acel jurnal.

* ht_authors -> dictionar pe id-uri de autori care da informatii despre un
autor (o lista cu id-urile articolelor publicate etc (struct author)).

* ht_collaborations -> dictionar pe id-uri de autori care da o lista cu id-
urile autorilor cu care acesta a colaborat (are macar un articol comun).

* ht_fields -> dictionar pe domenii care da o lista cu id-urile articolelor
din acel domeniu.

* ht_institution -> dictionar pe institutii care da o lista cu id-urile auto-
rilor din acea institutie.

* years -> vector de frecventa: years\[i\] = numarul de articole publicate in
anul i.

## Explicatii task-uri

### get_oldest_influence:

BFS pe referinte -> ma duc pe toate articolele care au influentat articolul
dat ca parametru, intr-o coada bag informatiile despre aceste articole pentru a
avea anul publicarii, numarul de citari reprezinta lungimea listei din dictiona-
rul ht_citations, iar id-ul, la fel ca anul il gasesc din dictionarul ht_papers.
Verific cele 3 situatii din enunt si actualizez titlul cerut la fiecare pas.

La parcurgerea BFS am marcat campul visit si am retinut indicii din dictio-
narul ht_papers care au fost vizitati, intr-o coada. Folosesc functia
reset_visit_papers pentru a demarca acei indici.


### get_venue_impact_factor:

Obtin lista articolelor care au aparul in jurnalul dat ca parametru, cu aju
torul lui ht_venues si pentru fiecare iau numarul de articole care il citeaza,
din dictionarul ht_citations, uitandu-ma la dimensiunea listei intoarse. In fi-
nal, fac aceasta medie a numarului de citari.


### get_number_of_influenced_papers:

BFS pe citari -> bag intr-o coada toate articolele care au citat articolul
dat ca parametru (pe care le obtin din lista intoarsa de ht_citations), calcu-
lez distanta fata de referinta (id-ul dat ca parametru), ca fiind cu 1 mai mare
decat distanta articolului scos din coada la momentul respectiv. Daca aceasta
distanta este <= distance, atunci numarul de articole care au fost influentate
creste, iar cand distanta devine > distance, opresc algoritmul. La fel demar-
chez indicii vizitati cu reset_visit_papers.


### get_erdos_distance:

BFS pe autori -> folosesc ht_collaborations si bag intr-o coada autorii cu
care cel dat ca parametru a colaborat si continui apoi cu colaboratii acestora,
si tot asa. La fel ca la task-ul precedent, calculez distanta fiecarui autor,
fata de referinta si daca gasesc id-ul celui de-al doilea autor dat ca parametru
in coada, opresc BFS-ul si intorc distanta. Daca coada devine vida, inseamna ca
nu am gasit drum intre cei doi autori si intorc -1.


### get_most_cited_papers_by_field:

MAX_HEAP -> folosesc ht_fields, pentru a obtine toate articolele din dome-
niul dat ca parametru; num_papers va fi dimensiunea acestei liste. Functia de
comparat folosita de heap, va fi compare_papers, care va compara 2 articole date
ca parametru in functie de cerintele din enunt. Bag toate articolele obtinute,
pe rand, in heap si apoi fac heap_top si heap_pop pentru a obtine fix ordinea
ceruta.


### get_number_of_papers_between_dates:

Vector de frecventa -> for de la early_date la late_date si adun intr-o va-
riabila years\[i\], adica numarul de articole aparute in anul i.


### get_number_of_authors_with_field:

Folosesc 3 dictionare:
* din ht_institutions obtin o lista cu id-urile autorilor de la acea insti
tutie
* din ht_authors iau lista de id-uri ale articolelor scrise de fiecare autor
* din ht_papers, iau domeniile fiecarui articol si le compar cu domeniul
dat ca parametru; daca sunt identici cresc numarul de autori


### get_histogram_of_citations:

Iau lista de articole publicate de autorul dat ca parametru. Merg pe id-uri-
le acestor articole si iau anul lor. daca gasesc un articol cu an mai mic decat
al ultimului articol, actualizez num_years, care va fi 2020 - anul minim al u-
unui articol al acelui autor.

Creez vectorul de frecventa histogram si iau din nou articolele scrise de
acel autor, iar la pozitia 2020 - anul articolului, din histogram, adaug numarul
de articole care il citeaza.


### get_reading_order:

Pornesc un BFS pe referinte ale articolului dat ca parametru si cand ajung
in nodurile terminale le bag intr-o lista. In aceasta lista pot sa am noduri
care sunt frunze in tot graful, sau care au distanta fata de referinta egala cu
parametrul distance. Intre acele noduri terminale care au distanta, distance,
pot exista legaturi, sa depinde unele de altele, adica sa nu fie frunze in
"minigraful considerat".

Verific care sunt frunze si le bag intr-un heap. Acum articolele din heap au
aceeasi prioritate de a fi citite (functia de comparat doar verifica criteriul
cu an si id). Fac heap_top si heap_pop si ma duc pe lista celor care citeaza
articolul obtinut. Recunosc articolele cautate dupa faptul ca au visit-ul 1,
din BFS. Le demarchez, punand visit-ul 0 si daca acestea nu mai au nicio
referinta care sa fie in graf atunci inseamna ca nu mai trebuie sa fie citit
niciun alt articol inainte si le bag in heap.

De asemenea verific si daca aceste articole care citeaza, nu cumva depind de
alte articole care si ele citeaza acel articol, inainte de a baga in heap.
Continui algoritmul de num_papers ori.


### find_best_coordinator:



		

