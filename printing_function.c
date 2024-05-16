//|------------------------------------|//
//| DICHIARAZIONE DELLE LIBRERIE USATE |//
//|------------------------------------|//

// Header
#include "header.h"

//|--------------------------------|//
//| IMPLEMENTAZIONI DELLE FUNZIONI |//
//|--------------------------------|//

// Stampa le righe di intestazione
void print_intestazione(struct indent_s indent){
    printf("%-*s %-*s %-*s %-5s %-12s %-10s %s\n", indent.ind_log + 2, "Login", indent.ind_name + 1, "Name", indent.ind_tty + 4, "Tty", "Idle", "Login Time", "Office", "Office Phone");
}

// Stampa l'idle time secondo il formato -s
char* check_idle_time(struct IdleTime idle, char* idle_print){
    // Controlla se l'idle time e' maggiore di un'ora 
    if(idle.hours >= 1){
        // Se i minuti sono minori di 10 stampa h:0x dove x = minuti e h = ore
        if(idle.minutes < 10){
            sprintf(idle_print, "%ld:0%ld", idle.hours, idle.minutes);
            return idle_print;  // Restituisce l'idle time sottoforma di stringa
        }
        // Altrimenti stampa h:x dove h = ore e x = minuti
        sprintf(idle_print, "%ld:%ld", idle.hours, idle.minutes);
        return idle_print;
    }
    // Controlla se l'idle time e' maggiore di 1 minuto, allora stampa solo i minuti
    else if(idle.hours < 1 && idle.minutes >= 1){
        sprintf(idle_print, "%ld", idle.minutes);
        return idle_print;
    }
    // Altrimenti non stampa niente
    else return "";
}

// Formattazione del comando -s
void prints(t_user user, struct indent_s indent){
    // Idle time da printare
    char idle_print[4] = "";
    int idle_ind;
    check_idle_time(user.idle_time, idle_print);            // Chiama la funzione che restituisce l'idle time sottoforma di stringa secondo il formato che serve in base al valore dell'idle time
    idle_ind = indent.ind_tty + (9 - strlen(idle_print));
    // Salva in una stringa temporanea il login time nel formato Mese Giorno ORA:MINUTI
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%B %d %H:%M", user.login_time);
    // User name
    printf("%-*s", indent.ind_log + 3, user.user_name);
    // Real name
    printf("%-*s", indent.ind_name + 2, user.real_name);
    // TTY
    printf("%-*s", idle_ind, user.tty);
    // Idle
    printf("%s", idle_print);
     // Login Time
    printf("%14s (%s)", buffer, user.tty);
    // Spazio
    printf("\n");
}

void prints_disc(
    struct indent_s indent,     // Struct in cui sono salvati i valori di indentazione
    t_user user                 // Struct contenente le informaizoni dell'utente di cui bisogna stampare le informazioni 
){
    // User name
    printf("%-*s", indent.ind_log + 3, user.user_name);
    // Real name
    printf("%-*s", indent.ind_name + 2, user.real_name);
    // TTY
    printf("%-*s", indent.ind_tty + 5, "*");
    // Idle
    printf("%-6s", "*");
    // Login time
    printf("%-13s", "No Logins");
    // Room Number
    if(user.room_number != NULL){printf("%-11s", user.room_number);}
    else{printf("%-11s", "");}
    // Office Phone
    if(user.office != NULL){printf("%s", user.office);}
    // Spazio
    printf("\n");
}

// Funzione per settare l'indentazione secondo le informazioni di ogni utente
struct indent_s set_indent(t_user* users, struct indent_s indent){
    // Itera sugli elementi dell'array in cui sono salvati le informaizoni di ogni utente loggato o cercato
    for(int i = 0; i < sizeof(users) ; i++){
        if(users[i].exist == true){
            if(indent.ind_log < strlen(users[i].user_name)){indent.ind_log = strlen(users[i].user_name);}
            if(indent.ind_name < strlen(users[i].real_name)){indent.ind_name = strlen(users[i].real_name);}
            // Controlla se e' stato impostato il tty dell'utente, se non e' stato impostato non modifica il valore di indentazione
            if(strcmp(users[i].tty, "") != 0) if(indent.ind_tty < strlen(users[i].tty)){indent.ind_tty = strlen(users[i].tty); 
        }
    }
    // Restituisce la struct contenente le indentazioni sulle parti dell'utente
    return indent;
}

// Formattazione del comando -l

// Funzione che dato un file stampa le righe del file
void print_file(FILE* file){
    char line[256]; // Buffer per leggere ogni linea del file
    while (fgets(line, sizeof(line), file) != NULL) { // Legge una linea alla volta finché non raggiunge la fine del file
        printf("%s", line); // Stampa la linea
    }
}

// Funzione che data la directory dell'utente controlla e nel caso stampa le righe dei file .plan .project .pgpkey 
void take_file(char* dir){
    // Buffer in cui salvo le directory dei file
    char buffer_pr[80];
    char buffer_pl[80];
    char buffer_pgp[80];
    // Aggiungo alla directory dell'utente il path per poter aprire i file .plan .project e .pgpkey
    sprintf(buffer_pl, "%s/%s", dir, ".plan");
    sprintf(buffer_pr, "%s/%s", dir, ".project");
    sprintf(buffer_pgp, "%s/%s", dir, ".pgpkey");
    // Apre i file in lettura
    FILE *file_pl = fopen(buffer_pl, "r"); // Apre il file in modalità di sola lettura
    FILE *file_pr = fopen(buffer_pr, "r"); // Apre il file in modalità di sola lettura
    FILE *file_pgp = fopen(buffer_pgp, "r"); // Apre il file in modalità di sola lettura
    // Controlla se i file sono stati aperti correttamente (se non sono stati aperti correttamente allora il valore della variabile di tipo FILE e' NULL)
    if(file_pgp != NULL) {
        printf("PGP key:\n");
        print_file(file_pgp);
    }
    if(file_pr != NULL){
        printf("Project:\n");
        print_file(file_pr);
    }
    // Se il file .plan non esiste allora stampa la stringa "No Plan."
    if(file_pl == NULL) printf("No Plan.\n");
    else{
        printf("Plan:\n");
        print_file(file_pl);
    }
    // Fine funzione
    return;
}

// Funzione che prende i GECOS dell'utente e li stampa in base all'esistenza delle informazioni GECOS
void print_gecos(t_user users, int ind){
    // Controlla se esiste il room number e l'office phone
    if(users.room_number != NULL && users.office != NULL){
        // Se esiste stampa entrambi nella stessa stringa separati da una ,
        printf("Office: %s, %s", users.room_number, users.office);
        // Controlla l'esistenza dell'home phone 
        if(users.home != NULL){
            // Se esiste l'home phone allora lo stampa distanziandolo dall'office phone e dal room number
            int spazio = ind - (strlen(users.room_number) + strlen(users.office) + 15);
            printf("%*sHome Phone: %s", spazio, "", users.home);
        }
        printf("\n");   // Va a capo
    }
    // Se non esiste l'office phone controlla se esiste il room number
    else if(users.room_number != NULL){
        // Stampa solo il room number
        printf("Office: %s", users.room_number);
        // Controlla l'esistenza dell'home phone
        if(users.home != NULL){
            int spazio = ind - (strlen(users.room_number) + 13);
            printf("%*sHome Phone: %s", spazio, "", users.home);
        }
        printf("\n");   // Va a capo
    }
    // Controlla se esiste solo l'office phone
    else if(users.office != NULL){
        // Stampa l'office phone
        printf("Office: %s", users.office);
        // Controlla se esiste l'home phone
          if(users.home != NULL){
            int spazio = ind - (strlen(users.office) + 13);
            printf("%*sHome Phone: %s", spazio, "", users.home);
        }
        printf("\n");   // Va a capo
    }
    // Controlla se esiste solo l'home phone
    else if(users.home != NULL){
        // Stampa l'home phone all'inizio della riga
        printf("Home Phone: %s", users.home);
        printf("\n");   // Va a capo
    }
    return;   // Fine funzione
}

// Funzione che stampa le informaizoni della mail dell'utente passato in input
void print_mail(t_user user){
    // Buffer in cui viene salvato la formattazione dell'orario in cui ha ricevuto l'ultima mail o l'ultima volta che ha letto la mail 
    char buffer[80];
    // Controlla se l'ultima mail che ha ricevuto e' stato dopo l'ultima volta che ha letto le mail 
    if(user.last_mail > user.last_mail_read){
        // Stampa a schermo l'ultima mail ricevuto
        struct tm* time = localtime(&user.last_mail);
        strftime(buffer, sizeof(buffer), "%a %B %d %H:%M %Y", time);
        printf("New mail received %s (%s)\n", buffer, time->tm_zone);   // tm_zone e' il fuso orario
        // Stampa anche l'ultima volta che ha letto le mail
        time = localtime(&user.last_mail_read);
        strftime(buffer, sizeof(buffer), "%a %B %d %H:%M %Y", time);
        printf("%5sUnread since %s (%s)\n", "", buffer, time->tm_zone); // tm_zone e' il fuso orario
    }
    // Altrimenti stampa solo l'ultima volta che ha letto le mail
    else{
        struct tm* time = localtime(&user.last_mail_read);
        strftime(buffer, sizeof(buffer), "%a %B %d %H:%M %Y", time);
        printf("Mail last read %s (%s)\n", buffer, time->tm_zone);
    }
    return; // Fine funzione 
}

// Funzione che prende le informazioni riguardante le mail dell'utente passato in input e restituisce l'utente stesso salvando le infromazioni nella struct relativa all'utente 
t_user get_mail_inf(t_user user){
    // Struttura utilizzata per prendere le informazioni riguardanti le proprieta' del file
    struct stat file_stat;
    // Buffer in cui salvo il percorso della mail dell'utente
    char buffer[80];
    sprintf(buffer, "%s/%s", _PATH_MAILDIR, user.user_name);
    // Controlla se le informazioni del file sono state prese correttamente allora salva l'ultima volta che l'utente ha letto le mail e l'ultima mail che ha ricevuto
    if(stat(buffer, &file_stat) != -1){
        user.last_mail = file_stat.st_mtime;
        user.last_mail_read = file_stat.st_atime;
        print_mail(user);   // Stampa le informaizoni della mail
    }
    else printf("No mail\n");   // Se il file non e' stato aperto correttamente allora stampa No mail. il che indica che e' stato trovato la mailbox dell'utente
    return user;    // Restituisce l'utente con le informazioni modificate nel caso in cui esiste la mailbox dell'utente
}

// Funzione che stampa a schermo l'idle time se il comando finger viene avviato con il comando -l
void print_idle_time(struct IdleTime idle){
    // Controlla l'idle e lo stampa in base al valore
    if(idle.hours >= 1) printf("%5ld hours %ld minutes idle\n", idle.hours, idle.minutes);    // Controlla se l'idle e' maggiore di un ora allora stampa l'ora e i minuti
    else if(idle.hours < 1 && idle.minutes >= 1) printf("%5ld minutes %ld seconds idle\n", idle.minutes, idle.seconds); // Controlla se l'idle e' maggiore di 1 minuto e minore di 1 ora allora stampa solo i minuti 
    else printf("%5ld seconds idle\n", idle.seconds);   // Altrimenti stampa solo i secondi
}

// Funzione principale in cui stampa le informazioni dell'utente loggato preso in input secondo il formato -l
void printl(t_user users){
    // Stampa l'user name e il real name
    printf("Login: %-33s", users.user_name);
    printf("Real Name: %s\n", users.real_name);
    // Stampa le directory dell'utente e della shell
    printf("Directory: %-29s", users.usr_dir);
    printf("Shell: %s\n", users.shell_dir);
    // Stampa le GECOS
    print_gecos(users, 45);
    // Stampa quando l'utente ha fatto il login
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a %B %d %H:%M", users.login_time);
    // Stampa quando l'utente ha fatto il login con la timezone, il tty e l'host
    printf("On since %s (%s) on %s from %s\n", buffer, users.timezone, users.tty, users.host);
    print_idle_time(users.idle_time);
    users = get_mail_inf(users);    // Funzione che prende le informazioni della mailbox e ne stampa le informaizoni
    return;   // Fine funzione
}

// Funzione principale in cui stampa le infromazioni dell'utente non loggato preso in input secondo il formato -l
void printl_disc(t_user users){
    // Stampa l'user name e il real name
    printf("Login: %-33s", users.user_name);
    printf("Real Name: %s\n", users.real_name);
    // Stampa le directory dell'utente e della shell
    printf("Directory: %-29s", users.usr_dir);
    printf("Shell: %s\n", users.shell_dir);
    // Stampa le gecos
    print_gecos(users, 45);
    // Stampa che non l'utente non e' loggato
    printf("Never logged in.\n");
    // Stampa le informazioni della mail
    users = get_mail_inf(users);
}
