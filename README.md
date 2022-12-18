# Loader executabile

### so_seg_t *FindSegment(char *addres)

- Returneaza segmentul din care face parte adresa din parametrul char* address

- Compara adresa data in parametru cu adresa de inceput si adresa de final a unui segment (>=inceput && <=final)

### static void segv_handler(int signum, siginfo_t *info, void *context)

- Verificare daca tratam semnlaul bun (signum != SIGSEGV)

- Verificare daca nu accesam o pagina deja mapata la care nu avem permisiune de acces (info->si\_code == SEGV\_ACCER), caz in care continuam cu handler-ul default (signal(SIGSEGV,SIG_DFL))

- Gasim segmentul in care a avut loc intreruperea cu functia FindSegment((char *)info->si_addr)

- Daca segmentul este null inseamna ca suntem intr-un segment necunoscut si continuam cu handler-ul default (signal(SIGSEGV,SIG_DFL))

- Calcul pageNumber, pageStartAddress, pageAddressInSegment, nextPageAddressInSegment

- Daca pageAddressInSegment < segment->file\_size mapez pagina in memorie. Daca urmatoarea pagina depaseste file\_size iar mem\_size e diferit de file\_size (daca sunt egale nextPageAddress va fi inafara segmentului cu o pagina) zeroizam de la fileEnd=segment->vaddr+segment->file\_size; (unde se termina fisierul) pana la nextPageAddress; adica restul paginii

- Altfel doar alocam memorie

### Instructiuni compilare

- Din folder skel-lin trebuie executata comanda make 
- Din folder checker-lin trebuie executata comanda make -f Makefile.checker (biblioteca e copiata din make-ul de la skel)

### Link git

- https://github.com/marcu26/tema2_pso
