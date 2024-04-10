#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/wireless.h>

#define MAX_BUF_SIZE 1024
//PRZYDATNE KOMENDY
//sudo iwconfig wlp2s0 txpower 21
//sudo iwlist wlp2s0 txpower
//sudo ethtool -S wlp2s0

//iw reg get (od 6 do 22)
//strace ifconfig wlan0;
#define clr() printf("\e[1;1H\e[2J")

#define MAX_INTERFACES 1000
char* interface;
double max_interface_pow = 22.0;
double min_interface_pow = 6.0;
double max_sig_pow = 70.0;


void getInterfaces() {
    FILE *fp;
    char line[256];
    char interfaces[MAX_INTERFACES][256];
    int interface_count = 0;

    // plik /proc/net/dev
    fp = fopen("/proc/net/dev", "r");
    if (fp == NULL) {
        perror("Błąd otwierania pliku /proc/net/dev");
        exit(EXIT_FAILURE);
    }

    // pomijam nagłówki
    fgets(line, sizeof(line), fp);
    fgets(line, sizeof(line), fp);

    // wczytuje nazwy inter. bezp.
    while (fgets(line, sizeof(line), fp) != NULL && interface_count < MAX_INTERFACES) {
        // czy w lini jest nazwa
        if (strstr(line, "wlan") != NULL || strstr(line, "wlp") != NULL) {
            char *interface_name = strtok(line, ":");
            if (interface_name != NULL) {
                strcpy(interfaces[interface_count], interface_name);
                interface_count++;
            }
        }
    }

    fclose(fp);

    printf("Znalazłem dla Ciebie bezprzewodowe interfejsy sieciowe (%i):\n\n-----------------------------\n", interface_count);
    for (int i = 0; i < interface_count; i++) {
        printf("%i. %s\n", i+1, interfaces[i]);
    }
    
    printf("-----------------------------\n\nWpisz numer tego, który Cię interesuje: ");
    int choice = 1;
    scanf("%d", &choice);


    interface = interfaces[choice-1];
    printf("-----------------------------\n\n%-15s | %-15s\n", "MOC SYGNAŁU (dBm)", "MOC PRĄDU (dBm)");
}

int set_interface_power(char* interface, int power) {

    int sockfd;
    struct iwreq wrq;

    // tworze gniazdo
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        return 1;
    }

    // ustawiam nazwe inter.
    strncpy(wrq.ifr_name, interface, IFNAMSIZ);

    // pobieram aktualne ustawienia
    if (ioctl(sockfd, SIOCGIWTXPOW, &wrq) == -1) {
        perror("ioctl(SIOCGIWTXPOW)");
        close(sockfd);
        return 1;
    }

    // zmieniam moc
    wrq.u.txpower.value = power;

    // aktualizuje parametry
    if (ioctl(sockfd, SIOCSIWTXPOW, &wrq) == -1) {
        perror("ioctl(SIOCSIWTXPOW)");
        close(sockfd);
        return 1;
    }

    close(sockfd);
    return wrq.u.txpower.value;
}

int main() {
    clr();
    
    FILE *fp;
    char command[MAX_BUF_SIZE];
    getInterfaces();
    char *token;
    int sig_pow;

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
        }
        pclose(fp);

        double sig_pow_procent = (double)sig_pow/max_sig_pow;

        int pow_to_set = (int)(2.0*(22.0*(1.0-(sig_pow_procent))));
        if(pow_to_set>22) pow_to_set = 22;
        else if(pow_to_set<6) pow_to_set = 6;

        int is_set = set_interface_power(interface, pow_to_set);
        printf("%-17i | %-17i\n", sig_pow, is_set);

        sleep(1);
    }
    return 0;
}
