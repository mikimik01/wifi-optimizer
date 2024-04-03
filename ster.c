#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024

int main() {
    char command[MAX_BUF_SIZE];
    char* interface = "wlp2s0"; // Nazwa interfejsu sieciowego WiFi
    int power_level = 10; // Poziom mocy transmisji (przykładowa wartość)

    // Tworzymy polecenie do ustawienia mocy transmisji
    snprintf(command, sizeof(command), "iwconfig %s txpower %d", interface, power_level);

    // Wykonujemy polecenie
    int status = system(command);
    if (status != 0) {
        perror("Błąd podczas ustawiania mocy transmisji");
        exit(EXIT_FAILURE);
    }

    printf("Moc transmisji dla interfejsu %s ustawiona na %d dBm\n", interface, power_level);

    return 0;
}
