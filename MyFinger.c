//|------------------------------------|//
//| DICHIARAZIONE DELLE LIBRERIE USATE |//
//|------------------------------------|//

// File in cui sono definite funzioni che prendono le informaizoni con le librerie pwd e utmp
#include "take_users.c"

// File in cui sono definite funzioni per stampare le informazioni secondi i formati -s -l
#include "printing_function.c"

// Header
#include "header.h"

// Funzione che prende un nome specificato in input in argv e controlla se esiste in users_logged e in pwd_users
int search_utent(char* nome, int cont_found){
    // Contatore per iterare in users_logged
    int cont_log = 0;
    
    // Itera sugli utenti che ho salvato in users_logged (itera fino a trovare un utente che esiste)
    while(users_logged[cont_log].exist != false){
        // Controlla se il nome è uguale a quello specificato in argv o e' contenuto nel nome reale 
        if(strcmp(users_logged[cont_log].user_name, nome) == 0 || (opts.m == false && str_in_str(users_logged[cont_log].real_name, nome) == 0)){
            // Aumenta il contatore utilizzato per
            cont_found++;
            // Imposta il valore di print a true indicando che l'utente puo' essere stampato a schermo con le sue relative informazioni che sono salvare nella struct 
            users_logged[cont_log].print = true;
            // Restituisce il count_found aggiornato
            return cont_found;
        // Controlla se l'utente puo' essere stampato, se non puo' essere stampato allora imposta il valore di print a false
        } else if(users_logged[cont_log].print != true) users_logged[cont_log].print = false;
        cont_log ++;  // Aumenta il contatore per passare all'utente successivo
    }

    // Salva in una variabile temporanea il valore del contatore utilizzato per iterare in users_logged
    int temp = cont_log;
    // Chiama la funzione get_pwdinf che aggiunge tutti gli utenti che hanno o come nome utente il valore di nome oppure gli utenti in cui nome e' contenuto nel nome reale 
    cont_log = get_pwdinf(cont_log, nome);
    // Controlla se cont_log e' stato modificato (se modificato sono stati aggiunti degli utenti)
    while (cont_log > temp){
        cont_log -= 1;                          // Diminuisce il contatore di 1
        users_logged[cont_log].print = true;    // Setta il valore di print a true in quanto essendo stato aggiungo a users_logged allora deve essere stampato a schermo
    }
    
    // Controlla se l'ultimo utente aggiunto e' un utente che e' stato trovato, se non e' stato trovato stampa un messaggio di errore indicando che l'utente specificato in input non esiste 
    if(users_logged[cont_log].exist == false) printf("%s: no such user\n", nome);
    // Se l'utente esiste imposta il valore di print a true in quanto le infromazioni relative all'utente devono essere stampate e imposta il valore di logged a false in quanto dato che non sono stati trovati in users_logged a allora sono utenti che al momento non sono loggati
    else{
        users_logged[cont_log].logged = false;
        users_logged[cont_log].print = true;
        // Aumenta i contatori
        cont_log ++;
        cont_found++;
    }

    // Fine funzione e restituisce il contatore cont_found
    return cont_found;
}

// MAIN
int main(int argc, char** argv){
    // Funzione che prende tutti gli utenti loggati
    logged_user();

    // array di stringhe in cui sono contenute i nomi utenti 
    char* nomi[argc - 1];
    // contatore per aggiungiere i nomi specificati nell'array dei nomi
    int cont = 0;

    // Itera sugli arogmenti presi in input e controlla quali comandi sono stati specificati
    for(int opt = 1; opt < argc; opt ++){
        // Se l'argomento è -l allora l'opzione l è settata a true
        if(strcmp(argv[opt],"-l") == 0) {
            opts.l = true;
        }
        // Se l'argomento è -s allora l'opzzione s è settata a rue
        else if(strcmp(argv[opt],"-s") == 0){
            opts.s = true;
        }
        // Se l'argomento è -m allora l'opzione m è settata a true
        else if(strcmp(argv[opt],"-m") == 0){
            opts.m = true;
        }
        // Se l'argomento è -p allora l'opzione p è settata a true
        else if(strcmp(argv[opt], "-p") == 0){
            opts.p = true;
        }
        else{
            // Se l'argomento è un nome utente allora lo aggiunge all'array di nomi
            nomi[cont] = argv[opt];
            cont ++;
        }
    }

    // Contatore utilizzato per controllare se gli utenti specificati in input sono >= 1
    int cont_found = 0;

    // In base a quale combinazioni di opzioni sceglie la formattazione in cui mostrare le informazioni degli utenti
    // Formattazione secondo il comando -s
    if(
        (opts.s == true && opts.l == false) ||
        (opts.s == true && opts.m == true && opts.l == false && opts.p == true) ||
        (opts.s == true && opts.m == true && opts.l == false) ||
        (opts.s == true && opts.p == true && opts.l == false) ||
        (opts.s == false && opts.m == false && opts.l == false && opts.p == false && cont == 0)
    ){
        // Se il numero di utenti specificati in input e' 0 allora il comando stampa a schermo le informazioni degli utenti che sono al momento loggati
        if(cont == 0){
            // Stampa gli utenti loggati
            indent = set_indent(users_logged, indent);              // Imposta l'indentazione della riga di intestaizone
            print_intestazione(indent);                             // Stampa la riga di intestaizone
            // Itera nell'array users_logged fino a trovare un utente che non esiste, allora smette di stampare fli utenti
            for(int i = 0; users_logged[i].exist == true; i ++){
                users_logged[i] = get_idle(users_logged[i]);        // Prende l'idle time dell'utente 
                prints(users_logged[i], indent);                    // Stampa a schermo le infromazioni dell'utente seguene l'indentazione utilizzata per la riga di intestazione 
            }
            return 0;   // Fine funzione MAIN
        }

        // Controlla se gli utenti presi in input esistono
        for(int c = 0; c < cont; c ++) cont_found = search_utent(nomi[c], cont_found);
        if(cont_found==0) return 0;

        // Stampa gli utenti salvati in users_logged in cui sono stati anche aggiunti gli utenti che sono stati specificati in input e che non erano presenti in users_logged
        indent = set_indent(users_logged, indent);    // Imposta l'indentazione delle righe
        print_intestazione(indent);                   // Stampa la riga di intestazione 
        // Itera nell'array users_logged fino a trovare un utente che non esiste, allora si ferma il ciclo
        for(int i = 0; users_logged[i].exist == true; i ++){
            // Controlla se l'utente e' loggato e deve essere stampato
            if(users_logged[i].logged == true && users_logged[i].print == true){
                users_logged[i] = get_idle(users_logged[i]);    // Prende l'idle time dell'utente
                prints(users_logged[i], indent);                // Stampa a schermo le infromazioni dell'utente seguendo l'indentazione usata per la riga di intestazione 
            // Se l'utente non e' loggato ma deve essere stampato allora printa le infromazioni dell'utente escludendo alcune informazioni
            }else if(users_logged[i].print == true) prints_disc(indent, users_logged[i]);
        }
    }
    // Formattazione secondo il comando -l
    else{
        // Controlla se sono stati specificati degli utenti in input (se non sono stati specificati stampa a schermo tutti gli utenti al momento loggati)
        if(cont == 0){
            // Itera nell'array users_logged e fino a trovare un utente che non esiste (se non esiste allora tutte le informazioni sono vuote)
            for(int i = 0; users_logged[i].exist == true; i ++){
                users_logged[i] = get_idle(users_logged[i]);    // Prende l'idle time dell'utente
                printl(users_logged[i]);                        // Stampa a schermo le informazioni dell'utente
                // Controlla se e' stato specificato il comando -p (se specificato non stampa i file .plan .project .pgpkey)
                if(opts.p == false) take_file(users_logged[i].usr_dir);     // se p == false allora chiama la funzione che stampa a i file d'interesse passando come parametro la direcotry dell'utente
                // Riga vuota
                printf("\n");
            }
            return 0;   // Fine funzione MAIN 
        }

        // Controlla se gli utentei presi in input esistono (se non esistono allora ferma la funzione)
        for(int c = 0; c < cont; c ++) cont_found = search_utent(nomi[c], cont_found);
        if(cont_found==0) return 0;

        // Itera nell'array users_logged fino a quando trova un utente che non esiste
        for(int i = 0; users_logged[i].exist == true; i ++){
            // Controlla se l'utente e' loggato e deve essere stampato a schermo
            if(users_logged[i].logged == true && users_logged[i].print == true){
                users_logged[i] = get_idle(users_logged[i]);                // Prende l'idle time dell'utente 
                printl(users_logged[i]);                                    // Stampa le informazioni dell'utente 
                if(opts.p == false) take_file(users_logged[i].usr_dir);     // Controlla se e' stato specificato il comando -p
                // Riga vuota
                printf("\n");
            // Se l'utente deve essere stampato ma non e' loggato allora stampa le informazioni dell'utente escludendo qualche informaizone
            }else if(users_logged[i].print == true){
                printl_disc(users_logged[i]); // Riga vuota
                if(opts.p == false) take_file(users_logged[i].usr_dir);   // Controlla se e' stato specificato il comando -p
                // Riga vuota
                printf("\n");
            }
        }
    }
    return 0; // Fine funzione MAIN
}
