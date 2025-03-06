# Load-Balancer

Harea Teodor-Adrian
313CA

Structuri de date
Tema2

  Tema isi propune sa utilizeze notiunile de hashtable
pentru a realiza o implementare a unui load balancer, asa
cum a fost descris in cadrul enuntului. Am utilizat functiile
folosite la laborator pentru a gestiona hashtable-urile din spatele
serverelor.

  Se pot identifica urmatoarele fisiere:

-"main.c": reprezinta baza programului, modul in care putem interactiona
cu acesta. Se identifica un loop ce citeste comenzile prezente in fisierul
primit ca parametru. De asemenea, se remarca functiile "get_key_value" si
"get_key" ce parseaza liniile citite din fisier si scoate cheile si valorile
ce vor fi introduse in load balancer, care le va distribui in memoriile
serverelor.

-"server.c": aici se regasesc functiile de baza pentru listele inlantuite,
pentru hashtable-uri si pentru lucrul cu structura "server_memory";
	Functii:
-"init_server_memory": aloca memoria necesara unui server si returneaza
acest server;
-"server_store": functia de hashtable "ht_put";
-"server_retrieve": functia de hashtable "ht_get";
-"server_remove_entry": functia de hashtable "ht_remove_entry";
-"free_server_memory": functia de hashtable "ht_free";
-"return_server_key_value": functia care returneaza toate perechile
de tip "key si value" dintr-un server.

-"load_balancer.c": contine functiile ce gestioneaza load balancer-ul;
	Functii:
-"hash_function_servers": returneaza hash-ul mastii sau id-ului unui
server;
-"hash_function_key"; returneaza hash-ul unei chei;
-"init_load_balancer": aloca memoria si initializeaza o structura de
tip "load_balancer";
-"loader_add_server": adauga un nou server in load balancer. Se realoca
memoria pentru array-ul de hashring si array-ul de servere. Se calculeaza
ethichetele serverului nou adaugat si se insereaza in hashring, tinand
cont ca acest vector trebuie sa ramana ordonat crescator. Dupa acestea,
se realizeaza procesul de redistribuire a datelor prezente in memoriile
serverelor din load balancer. Se itereaza prin vectorul de hash-uri si,
pentru fiecare dintre cele 3 noi hash-uri adaugate in hashring, se verifica
care este hash-ul imediat urmator in sens crescator. Se cauta id-ul serverului
reprezentat de hash-ul gasit. Se scot, pe rand, datele din serverul cu id-ul
gasit si se adauga din nou in load balancer, prin intermediul functiei de
"loader store";
-"loader_remove_server": sterge un server din load balancer. Se realoca
memoria pentru array-urile de servere si de hash-uri si se sterge serverul
respectiv din array-ul de servere, precum si hash-urile ce ii corespund
acestuia din array-ul de hashring. Se scot toate perechile de tip
key si value din serverul pe care vrem sa il stergem si se insereaza din
nou in load balancer, acest lucru fiind realizat dupa ce am sters
datele din load balancer legate de acest server. Se elibereaza memoria
serverului de sters; 
-"loader_store": adauga in serverele din load balancer o pereche de
tip key si value. Se calculeaza hash-ul key-ului si se gaseste hash-ul
imediat urmator in sens crescator, din hashring. Se identifica id-ul
serverului caruia ii corespunde hash-ul gasit. In acest server se
vor introduce datele;
-"loader_retrieve": cauta valoarea "value" asociata cheii "key" primita.
Se calculeaza hash-ul cheii si se cauta hash-ul imediat urmator in sens
crescator din hashring. Se identifica id-ul serverului caruia ii corespunde
hash-ul. In aceste server se cauta perechile de tip key si value si se
returneaza "value"-ul asociat cheii "key";
-"free_load_balancer": elibereaza memoria load balancer-ului;

-"load_balancer.h": contine structura de load_balancer, in care se gasesc
hashring-ul, size-ul hashring-ului(numarul de hashuri prezente in acesta),
array-ul de pointeri catre servere(servers) si numarul de servere(size);

-"server.h": contine structura de server_memory, care este o structura
de tip hashtable, ce are in aceasta array-ul de liste simplu inlantuite
(buckets), id-ul serverului(server_id) si numarul de bucket-uri(size);
