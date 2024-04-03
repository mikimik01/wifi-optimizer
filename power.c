#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024

int main() {
    FILE *fp;
    char filepath[MAX_BUF_SIZE];
    char* interface = "wlp2s0"; // Nazwa interfejsu sieciowego WiFi
    char value[MAX_BUF_SIZE];

    // Tworzymy ścieżkę do pliku z informacjami o prądzie
    snprintf(filepath, sizeof(filepath), "/sys/class/net/%s/statistics/rx_bytes", interface);

    // Otwieramy plik do odczytu
    fp = fopen(filepath, "r");
    if (fp == NULL) {
        perror("Błąd otwierania pliku");
        exit(EXIT_FAILURE);
    }

    // Odczytujemy wartość prądu
    if (fgets(value, sizeof(value), fp) != NULL) {
        printf("Obecna wartość prądu dla interfejsu %s: %s\n", interface, value);
    } else {
        printf("Nie można odczytać wartości prądu dla interfejsu %s\n", interface);
    }

    // Zamykamy plik
    fclose(fp);

    return 0;
}
