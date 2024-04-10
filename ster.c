#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/wireless.h>

int main() {
    const char *interface = "wlp2s0";

    int sockfd;
    struct iwreq wrq;

    // Otwórz gniazdo do interfejsu bezprzewodowego
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        return 1;
    }

    // Ustaw nazwę interfejsu
    strncpy(wrq.ifr_name, interface, IFNAMSIZ);

    // Pobierz aktualne ustawienia
    if (ioctl(sockfd, SIOCGIWTXPOW, &wrq) == -1) {
        perror("ioctl(SIOCGIWTXPOW)");
        close(sockfd);
        return 1;
    }

    // Wyświetl aktualny poziom mocy

    //wrq.u.txpower.value = 40;
    printf("Aktualny poziom mocy dla interfejsu %s: %d dBm\n", interface, wrq.u.txpower.value);

    // Ustaw te same parametry mocy
    if (ioctl(sockfd, SIOCSIWTXPOW, &wrq) == -1) {
        perror("ioctl(SIOCSIWTXPOW)");
        close(sockfd);
        return 1;
    }

    printf("Parametry mocy dla interfejsu %s zostały ustawione na takie same wartości.\n", interface);

    close(sockfd);
    return 0;
}