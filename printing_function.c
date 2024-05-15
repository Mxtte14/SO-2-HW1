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

// Stampa l'idle time
char* check_idle_time(struct IdleTime idle, char* idle_print){
    // Idle time da printare
    if(idle.hours >= 1){
        if(idle.minutes < 10){
            sprintf(idle_print, "%ld:0%ld", idle.hours, idle.minutes);
            return idle_print;
        }
        sprintf(idle_print, "%ld:%ld", idle.hours, idle.minutes);
        return idle_print;
    }
    else if(idle.hours < 1 && idle.minutes >= 1){
        sprintf(idle_print, "%ld", idle.minutes);
        return idle_print;
    }
    else{
        return "";
    }
}

// Formattazione del comando -s
void prints(t_user user, struct indent_s indent){
    // Idle time da printare
    char idle_print[4] = "";
    int idle_ind;
    check_idle_time(user.idle_time, idle_print);
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
    struct indent_s indent,
    t_user user
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

// Funzione per settare l'indentazione secondo le informazioni fi ogni utente
struct indent_s set_indent(t_user* users, struct indent_s indent){
    // Itera sugli elementi dell'array in cui sono salvati le informaizoni di ogni utente loggato o cercato
    for(int i = 0; i < sizeof(users) ; i++){
        if(users[i].exist == true){
            if(indent.ind_log < strlen(users[i].user_name)){indent.ind_log = strlen(users[i].user_name);}
            if(indent.ind_name < strlen(users[i].real_name)){indent.ind_name = strlen(users[i].real_name);}
            // Controlla se e' stato impostato il tty dell'utente, se non e' stato impostato non modifica il valore di indentazione
            if(strcmp(users[i].tty, "") != 0){
                if(indent.ind_tty < strlen(users[i].tty)){indent.ind_tty = strlen(users[i].tty);}
            }
        }
    }
    // Restituisce la struct contenente le indentazioni sulle parti dell'utente
    return indent;
}

// Formattazione del comando -l

void print_file(FILE* file){
    char line[256]; // Buffer per leggere ogni linea del file
    while (fgets(line, sizeof(line), file) != NULL) { // Legge una linea alla volta finché non raggiunge la fine del file
        printf("%s", line); // Stampa la linea
    }
}

void take_file(char* dir){
    char buffer_pr[80];
    char buffer_pl[80];
    char buffer_pgp[80];
    sprintf(buffer_pl, "%s/%s", dir, ".plan");
    sprintf(buffer_pr, "%s/%s", dir, ".project");
    sprintf(buffer_pgp, "%s/%s", dir, ".pgpkey");

    FILE *file_pl = fopen(buffer_pl, "r"); // Apre il file in modalità di sola lettura
    FILE *file_pr = fopen(buffer_pr, "r"); // Apre il file in modalità di sola lettura
    FILE *file_pgp = fopen(buffer_pgp, "r"); // Apre il file in modalità di sola lettura

    if(file_pgp != NULL) {
        printf("PGP key:\n");
        print_file(file_pgp);
    }

    if(file_pr != NULL){
        printf("Project:\n");
        print_file(file_pr);
    }

    if(file_pl == NULL) printf("No Plan.\n");
    else{
        printf("Plan:\n");
        print_file(file_pl);
    }

    return;
}

void print_gecos(t_user users, int ind){
    // Stampa le GECOS dell'utente se esistenti
    if(users.room_number != NULL && users.office != NULL){
        printf("Office: %s, %s", users.room_number, users.office);
        if(users.home != NULL){
            int spazio = ind - (strlen(users.room_number) + strlen(users.office) + 15);
            printf("%*sHome Phone: %s", spazio, "", users.home);
        }
        printf("\n");
    }
    else if(users.room_number != NULL){
        printf("Office: %s", users.room_number);
        if(users.home != NULL){
            int spazio = ind - (strlen(users.room_number) + 13);
            printf("%*sHome Phone: %s", spazio, "", users.home);
        }
        printf("\n");
    }
    else if(users.office != NULL){
        printf("Office: %s", users.office);
        if(users.home != NULL){
            int spazio = ind - (strlen(users.office) + 13);
            printf("%*sHome Phone: %s", spazio, "", users.home);
        }
        printf("\n");
    }
    else if(users.home != NULL){
        printf("Home Phone: %s", users.home);
        printf("\n");
    }
}

void print_mail(t_user user){
    char buffer[80];

    if(user.last_mail > user.last_mail_read){
        struct tm* time = localtime(&user.last_mail);
        strftime(buffer, sizeof(buffer), "%a %B %d %H:%M %Y", time);
        printf("New mail received %s (%s)\n", buffer, time->tm_zone);

        time = localtime(&user.last_mail_read);
        strftime(buffer, sizeof(buffer), "%a %B %d %H:%M %Y", time);
        printf("%5sUnread since %s (%s)\n", "", buffer, time->tm_zone);
    }
    else{
        struct tm* time = localtime(&user.last_mail_read);
        strftime(buffer, sizeof(buffer), "%a %B %d %H:%M %Y", time);
        printf("Mail last read %s (%s)\n", buffer, time->tm_zone);
    }
}

t_user get_mail_inf(t_user user){
    struct stat file_stat;
    char buffer[80];
    sprintf(buffer, "%s/%s", _PATH_MAILDIR, user.user_name);

    if(stat(buffer, &file_stat) != -1){
        user.last_mail = file_stat.st_mtime;
        user.last_mail_read = file_stat.st_atime;
        print_mail(user);
    }
    else printf("No mail\n");

    return user;
}

// Funzione che stampa a schermo l'idle time se il comando finger viene avviato con il comando -l
void print_idle_time(struct IdleTime idle){

    if(idle.hours >= 1){
        printf("%5ld hours %ld minutes idle\n", idle.hours, idle.minutes);
    }
    else if(idle.hours < 1 && idle.minutes >= 1){
        printf("%5ld minutes %ld seconds idle\n", idle.minutes, idle.seconds);
    }
    else{
        printf("%5ld seconds idle\n", idle.seconds);
    }
}

void printl(t_user users){
    
    // Stampa l'user name e il real name
    printf("Login: %-33s", users.user_name);
    printf("Real Name: %s\n", users.real_name);

    // Stampa le directory dell'utente e della shell
    printf("Directory: %-29s", users.usr_dir);
    printf("Shell: %s\n", users.shell_dir);

    // Stampa le GECOS
    print_gecos(users, 45);

    // 
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a %B %d %H:%M", users.login_time);

    // Stampa quando l'utente ha fatto il login
    printf("On since %s (%s) on %s from %s\n", buffer, users.timezone, users.tty, users.host);
    print_idle_time(users.idle_time);
    users = get_mail_inf(users);
}

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