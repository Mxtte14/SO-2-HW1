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

    while(users_logged[cont_log].exist != false){
        // Se il nome è uguale a quello specificato in argv
        if(strcmp(users_logged[cont_log].user_name, nome) == 0 || (opts.m == false && str_in_str(users_logged[cont_log].real_name, nome) == 0)){
            cont_found++;
            users_logged[cont_log].print = true;
            return cont_found;
        } else if(users_logged[cont_log].print != true) users_logged[cont_log].print = false;
        cont_log ++;
    }

    int temp = cont_log;
    cont_log = get_pwdinf(cont_log, nome);
    while (cont_log > temp){
        cont_log -= 1;
        users_logged[cont_log].print = true;
    }

    if(users_logged[cont_log].exist == false){
        printf("%s: no such user\n", nome);
    }
    else{
        users_logged[cont_log].logged = false;
        users_logged[cont_log].print = true;
        cont_log ++;
        cont_found++;
    }

    // Fine funzione
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

    int cont_found = 0;

    // In base a quale combinazioni di opzioni sceglie la formattazione in cui mostrare le informazioni degli utenti
    if(
        (opts.s == true && opts.l == false) ||
        (opts.s == true && opts.m == true && opts.l == false && opts.p == true) ||
        (opts.s == true && opts.m == true && opts.l == false) ||
        (opts.s == true && opts.p == true && opts.l == false) ||
        (opts.s == false && opts.m == false && opts.l == false && opts.p == false && cont == 0)
    ){
        if(cont == 0){
            // Stampa gli utenti loggati
            indent = set_indent(users_logged, indent);
            print_intestazione(indent);
            for(int i = 0; users_logged[i].exist == true; i ++){
                users_logged[i] = get_idle(users_logged[i]);
                prints(users_logged[i], indent);
            }
            return 0;
        }

        // Controlla se gli utenti presi in input esistono
        for(int c = 0; c < cont; c ++) cont_found = search_utent(nomi[c], cont_found);
        if(cont_found==0) return 0;

        // Stampa gli utenti salvati in users_to_print
        indent = set_indent(users_logged, indent);
        print_intestazione(indent);
        for(int i = 0; users_logged[i].exist == true; i ++){
            if(users_logged[i].logged == true && users_logged[i].print == true){
                users_logged[i] = get_idle(users_logged[i]);
                prints(users_logged[i], indent);
            }else if(users_logged[i].print == true) prints_disc(indent, users_logged[i]);
        }
    }
    else{
        if(cont == 0){
            for(int i = 0; users_logged[i].exist == true; i ++){
                users_logged[i] = get_idle(users_logged[i]);
                printl(users_logged[i]);
                if(opts.p == false) take_file(users_logged[i].usr_dir);
                // Riga vuota
                printf("\n");
            }
            return 0;
        }
        for(int c = 0; c < cont; c ++) cont_found = search_utent(nomi[c], cont_found);
        if(cont_found==0) return 0;

        for(int i = 0; users_logged[i].exist == true; i ++){
            if(users_logged[i].logged == true && users_logged[i].print == true){
                users_logged[i] = get_idle(users_logged[i]);
                printl(users_logged[i]);
                if(opts.p == false) take_file(users_logged[i].usr_dir);
                // Riga vuota
                printf("\n");
            }else if(users_logged[i].print == true){
                printl_disc(users_logged[i]); // Riga vuota
                if(opts.p == false) take_file(users_logged[i].usr_dir);
                printf("\n");
            }
        }
    }

    return 0;
}