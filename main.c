#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024
//iwlist wlp2s0 txpower
#define clr() printf("\e[1;1H\e[2J")

#define MAX_INTERFACES 1000
char* interface;

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
    clr();
}



int main() {
    clr();
    
    FILE *fp;
    char command[MAX_BUF_SIZE];
    getInterfaces();//"wlp2s0";
    char *token;
    int rssi;
    while(1){
        snprintf(command, sizeof(command), "iwconfig %s", interface);

        fp = popen(command, "r");
        if (fp == NULL) {
            perror("Błąd otwierania strumienia do polecenia iwconfig");
            exit(EXIT_FAILURE);
        }

        while (fgets(command, sizeof(command), fp) != NULL) {
            // Szukaj linii zawierającej informacje o sygnałach
            if (strstr(command, "Signal level") != NULL) {
                // Podziel linię na tokeny
                token = strtok(command, "=");
                token = strtok(NULL, "=");

                // Odczytaj wartość sygnału RSSI
                sscanf(token, "%d", &rssi);

                printf("Moc sygnału dla interfejsu %s: %d dBm\n", interface, rssi);
                pclose(fp);
                break;
                //return 0;
            }
        }

        sleep(2);
    }
    return 0;
}
