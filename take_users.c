//|------------------------------------|//
//| DICHIARAZIONE DELLE LIBRERIE USATE |//
//|------------------------------------|//

// Header
#include "header.h"

//|--------------------------------------|//
//| DICHIRAZIONI DELLE VARIABILI GLOBALI |//
//|--------------------------------------|//

// Struct per controllare quali opzioni sono stati specificati negli argomenti in input
struct options opts = {false, false, false, false};

// Struct per salvare i valori per impostare l'indentazione secondo il comando finger -s
struct indent_s indent = {0, 0, 0};

// Contiene gli utenti loggati con le loro relative informazioni
t_user users_logged[100];

//|--------------------------------|//
//| IMPLEMENTAZIONI DELLE FUNZIONI |//
//|--------------------------------|//

// Funzione che restituisce il tempo di idle del sistema
struct IdleTime get_idle_time(struct IdleTime idle){

    // Inizializzazione delle tre variabili
    time_t days = 0;
    time_t hours = 0;
    time_t minutes = 0;
    time_t seconds = idle.seconds;

    // Calcola da quanto tempo e' stato avviato il sistema
    while(seconds >= 60){
        minutes = seconds/60;
        seconds = seconds - (minutes * 60);
        if(minutes >= 60){
            hours = minutes/60;
            minutes = minutes - (hours * 60);
        }
        if(hours >= 24){
            days = hours/60;
            hours = hours - (days * 24);
        }
    }

    // Imposta i valori della struttura della variabile idle
    idle.days = days;
    idle.hours = hours;
    idle.minutes = minutes;
    idle.seconds = seconds;

    return idle;
}

// Funzione che prende come parametro una stringa e imposta tutti i caratteri della stringa in minuscolo
char* string_lower(char* string){
    // Per ogni carattere della stringa rende il carattere minuscolo e restitusice la stringa modificata
    for(int i = 0; string[i]; i ++) string[i] = tolower(string[i]);
    return string;
}

// Funzione che presi due stringhe controlla se la seconda e' contenuta nella prima senza distinzione tra minuscolo e maiuscolo
int str_in_str(char* string1, char* string2){
    // Salva in dei buffer la prima e la seconda stringa
    char* real_name = strdup(string1);
    char* name = strdup(string2);
    // Rende le due stringa con tutti i caratteri in minuscolo 
    real_name = string_lower(real_name);
    name = string_lower(name);
    // Prende la prima stringa, la divide in base al separatore " " e controlla se la seconda stringa e' uguale a una parte della prima stringa 
    char* token = strtok(real_name, " ");
    while(token != NULL){
       if(strcmp(token, name) == 0) return 0;   // Nel caso in cui la seconda stringa e' uguale a una parte della prima stringa restitusice 0
       token = strtok(NULL, " "); 
    } 
    // Se la seconda stringa non e' uguale a nessuna parte della prima stringa allora restitusice 1    
    return 1;
}

// Funzione che calcola l'idle time
t_user get_idle(t_user user){
    // Calcola l'idle time
    struct stat file_stat;
    char file_path[100];
    // Aggiunge a file_path il percorso per trovare il file tty
    if(strstr(user.tty, "tty") != NULL){
        sprintf(file_path, "/dev/tty");
    }else sprintf(file_path, "/dev/%s", user.tty);
    // Se il file e' stato preso correttemente allora calcola il tempo di idle
    if (stat(file_path, &file_stat) != -1) {
        // Ottenere e stampare l'ultimo accesso (atime)
        user.idle_time.seconds = time(NULL) - file_stat.st_ctime;
        user.idle_time = get_idle_time(user.idle_time);
    }
    // Fine funzione, restituisce l'utente con il tempo di idle impostato
    return user;
}

// Funzione che estrae le informaizoni del gecos
void take_gecos(char *gecos[4], struct passwd* pw, int cont){
    // Contatore utilizzato per salvare le informazioni del gecos nell'array preso in input chiamato gecos
    int cont_gecos = 0;
    char *temp;

    // Estrae le informaizoni del gecos da pw_gecos utilizzando strtok con separatore ","
    for (temp = strtok(pw->pw_gecos, ","); temp; temp = strtok(NULL, ",")) {
        if (*temp==0) temp = NULL;
        if (cont_gecos < 4) gecos[cont_gecos++] = temp;
    }
    // Se non sono esistono alcune informaiozni salva come valore in gecos il valore NULL
    while (cont_gecos<4) gecos[cont_gecos++] = NULL;

    // Assegna il nome reale, il room number, l'office phone e l'home phone alle informazioni dell'utente se esistono
    if(gecos[0] != NULL) users_logged[cont].real_name = strdup(gecos[0]);
    else users_logged[cont].real_name = NULL;
    // Controlla se il room number, l'office phone e l'home phone esistono
    if(gecos[1] != NULL) users_logged[cont].room_number = strdup(gecos[1]);
    else users_logged[cont].room_number = NULL;
    if(gecos[2] != NULL) users_logged[cont].office = strdup(gecos[2]);
    else users_logged[cont].office = NULL;
    if(gecos[3] != NULL) users_logged[cont].home = strdup(gecos[3]);
    else users_logged[cont].home = NULL;

    // Fine funzione
    return;
}

// Funzione che prense un contatore e un nome e controlla se esiste un utente in cui nel nome reale e' contenuto la variabile name
int get_pwdinf_by_realname(int cont, char* name){
    // Struct della librearia passwd utilizzata per prendere le informazioni in /etc/passwd 
    struct passwd *pw;
    // Inizilizza l'apertura del file
    setpwent();
    // Itera sul file e prende ogni riga come struct passwd in cui sono salvate le informazioni per ogni riga
    while((pw = getpwent()) != NULL){
        // Assegnazione dell'user name nel caso in cui l'utente non e' stato salvato precedentemente nella struct
        strncpy(users_logged[cont].user_name, pw->pw_name, UT_NAMESIZE);
        users_logged[cont].user_name[UT_NAMESIZE] = '\0';
        // Inizializza l'array in cui salvare le GECOS
        char* gecos[4];
        take_gecos(gecos, pw, cont);
        // Controlla se l'utente o processo preso ha il nome reale, e se name e' contenuto nel nome reale
        if(gecos[0] != NULL && str_in_str(gecos[0], name) == 0){
            // Assegna l'user directory e la shel directory nel caso in cui name e' contenuto nel nome reale
            users_logged[cont].usr_dir = strdup(pw->pw_dir);
            users_logged[cont].shell_dir = strdup(pw->pw_shell);
            users_logged[cont].exist = true;
            // Aumenta il contatore
            cont ++;
        }
    }
    // Chiude l'apertura del file
    endpwent();
    return cont;    // Restituisce il contatore
}

// Funzione che dato l'username in input prende le informazioni relative a quell'utente da /etc/passwd
int get_pwdinf(int cont, char* name){
    // Struct utilizzata per ottenere i dati in /etc/passwd di un utente o processo
    struct passwd* pw;
    // Prende le informazioni in /etc/passwd del nome utente passato come parametro della funzione
    pw = getpwnam(name);
    // Controlla se l'utente e' stato preso correttamente
    if(pw != NULL){
        // Assegnazione dell'user name nel caso in cui l'utente non e' stato salvato precedentemente nella struct
        if(users_logged[cont].exist == false){
            strncpy(users_logged[cont].user_name, pw->pw_name, UT_NAMESIZE);
            users_logged[cont].user_name[UT_NAMESIZE] = '\0';
        }
        // Divide i gecos e li salva in un array
        char* gecos[4];
        take_gecos(gecos, pw, cont);
        // Assegna l'user directory e la shel directory
        users_logged[cont].usr_dir = strdup(pw->pw_dir);
        users_logged[cont].shell_dir = strdup(pw->pw_shell);
        users_logged[cont].exist = true;
        return cont;    // Restitusice il contatore utilizzato per iterare nell'array users_logged
    }
    // Controlla se e' stato specificato l'opzione -m (se e' stato specificato allora non esegue il controllo sull'username)
    // Se non e' stato trovato con getpwnam allora potrebbe essere stato specificato un utente con il nome reale
    else if(opts.m == false){
        cont = get_pwdinf_by_realname(cont, name);    // Se non ha trovato nessun utente che ha come user_name il name passato in input controlla se esiste un utente che ha un nome reale in cui e' contenuto name 
        return cont;    // Restitusice il contatore
    }
    // Se non trova nessun utente imposta il parametro exist a false per indicare che non esiste
    users_logged[cont].exist = false;

    // Fine funzione
    return cont;  // Restituisce il contatore
}

// Funziona che prende tutti gli utenti con la libreria utmp (ovvero prende gli utenti non loggati)
void logged_user(){
    // Contatore per inserire gli utenti in users_logged
    int cont = 0;
    // Struct per prendere tutti gli utenti loggati
    struct utmp* ut;
 
    // Funzione della librerie utmp.h indica l'inizio della lettura del file utmp
    setutent();

    // Prende un utente e lo salva in ut, finche riesce a prendere gli utente continua il while, altrimenti se ut == NULL allora non ci sono piu' utenti loggati
    while((ut = getutent()) != NULL){
        // Controlla se e' un utente
        if(ut->ut_type == USER_PROCESS){
            // Assegnazione dell'user name
            strncpy(users_logged[cont].user_name, ut->ut_user, UT_NAMESIZE);
            users_logged[cont].user_name[UT_NAMESIZE] = '\0';
            // Assegnazione del tty e dell'host
            strncpy(users_logged[cont].tty, ut->ut_line, UT_LINESIZE);
            users_logged[cont].tty[UT_LINESIZE] = '\0';
            strncpy(users_logged[cont].host, ut->ut_host, UT_HOSTSIZE);
            users_logged[cont].host[UT_HOSTSIZE] = '\0';
            // Prende le informazioni del login time
            time_t temp = ut->ut_time;
            users_logged[cont].login_time = localtime(&temp);
            // Assegna la timezone
            users_logged[cont].timezone = users_logged[cont].login_time->tm_zone;
            // Prende le informazioni da pwd dell'utente
            cont = get_pwdinf(cont,  users_logged[cont].user_name);
            users_logged[cont].exist = true;
            users_logged[cont].logged = true;
            // Incrementa il contatore per aggiungere l'utente successivo
            cont ++;
        }
    }
    // Assegna NULL al nome utente corrente per indicare la fine dell'array
    users_logged[cont].exist = false;

    // Funzione della librerie utmp.h che chiude il file
    endutent();

    // Fine funzione
    return;
}
