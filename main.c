#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024
//sudo iwconfig wlp2s0 txpower 21
//sudo iwlist wlp2s0 txpower
//sudo ethtool -S wlp2s0
#define clr() printf("\e[1;1H\e[2J")

#define MAX_INTERFACES 1000
char* interface;
double max_interface_pow = 144.0;
double max_sig_pow = 70.0;

void getInterfaces() {
    FILE *fp;
    char line[256];
    char interfaces[MAX_INTERFACES][256];
    int interface_count = 0;

    // Otwórz plik /proc/net/dev
    fp = fopen("/proc/net/dev", "r");
    if (fp == NULL) {
        perror("Błąd otwierania pliku /proc/net/dev");
        exit(EXIT_FAILURE);
    }

    // Pomiń pierwsze dwie linie pliku (nagłówki)
    fgets(line, sizeof(line), fp);
    fgets(line, sizeof(line), fp);

    // Wczytaj nazwy bezprzewodowych interfejsów z pliku
    while (fgets(line, sizeof(line), fp) != NULL && interface_count < MAX_INTERFACES) {
        // Sprawdź, czy linia zawiera interfejs bezprzewodowy
        if (strstr(line, "wlan") != NULL || strstr(line, "wlp") != NULL) {
            char *interface_name = strtok(line, ":");
            if (interface_name != NULL) {
                strcpy(interfaces[interface_count], interface_name);
                interface_count++;
            }
        }
    }

    // Zamknij plik
    fclose(fp);

    // Wyświetl nazwy bezprzewodowych interfejsów
    printf("Znalazłem dla Ciebie bezprzewodowe interfejsy sieciowe (%i):\n\n-----------------------------\n", interface_count);
    for (int i = 0; i < interface_count; i++) {
        printf("%i. %s\n", i+1, interfaces[i]);
    }
    
    printf("-----------------------------\n\nWpisz numer tego, który Cię interesuje: ");
    int choice = 1;
    scanf("%d", &choice);


    interface = interfaces[choice-1];
    printf("-----------------------------\n\n%-11s | %-11s\n", "MOC SYGNAŁU", "MOC PRĄDU");
}

void set_interface_pow(char* interface, int power_level) {
    char command[MAX_BUF_SIZE];

    // Tworzenie polecenia do ustawienia mocy transmisji
    snprintf(command, sizeof(command), "iwconfig %s txpower %d", interface, power_level);

    // Wykonywanie polecenia
    int status = system(command);
    if (status != 0) {
        perror("Błąd podczas ustawiania mocy transmisji");
        exit(EXIT_FAILURE);
    }

    //printf("Moc transmisji dla interfejsu %s ustawiona na %d dBm\n", interface, power_level);
}

int main() {
    clr();
    
    FILE *fp;
    char command[MAX_BUF_SIZE];
    getInterfaces();//"wlp2s0";
    char *token;
    char *token1;
    int sig_pow;
    int interface_pow;
    while(1){
        snprintf(command, sizeof(command), "iwconfig %s", interface);

        fp = popen(command, "r");
        if (fp == NULL) {
            perror("Błąd otwierania strumienia do polecenia iwconfig");
            exit(EXIT_FAILURE);
        }
        while(fgets(command, sizeof(command), fp) != NULL){
            if (strstr(command, "Signal level") != NULL) {
                token = strtok(command, "=");
                token = strtok(NULL, "=");
                sscanf(token, "%d", &sig_pow);
            }

            if (strstr(command, "Tx-Power") != NULL) {
                token1 = strtok(command, "=");
                token1 = strtok(NULL, "=");
                sscanf(token1, "%d", &interface_pow);
            }
        }   
        pclose(fp);

        double sig_pow_procent = (double)sig_pow/max_sig_pow;
        double to_set_procent = 1.0;
        if(sig_pow_procent<=0.4){
            to_set_procent = 1.0;
        }else if (sig_pow_procent >= 1.0){
            to_set_procent = 0.4;
        }else{
            to_set_procent = 1.4 - sig_pow_procent;
        }


        printf("\n\nprocent: %f zadana %f\n\n",to_set_procent, to_set_procent*max_interface_pow);
        printf("%-11i | %-11i\n", sig_pow, interface_pow);

        sleep(1);
    }
    return 0;
}
