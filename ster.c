#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024

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
