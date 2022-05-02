//

/*-------------------------------------------------------------*/
/* Exemplo Socket Raw - Captura pacotes recebidos na interface. */
/*-------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

/* Diretorios: net, netinet, linux contem os includes que descrevem */
/* as estruturas de dados do header dos protocolos.  	  	        */

#include <net/if.h>        // Estrutura ifr.
#include <netinet/ether.h> // Header ethernet.
#include <netinet/in.h>    // Definicao de protocolos.
#include <arpa/inet.h>     // Funcoes para manipulacao de enderecos IP.

#include <netinet/in_systm.h> // Tipos de dados.

#define BUFFSIZE 1518
#define MAX_PACKAGES 10 // Quantidade maxima de pacotes que serao recebidos/enviados pelo programa.

// Funcoes auxiliares de maximo e minimo.
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

// Atencao!! Confira no /usr/include do seu sisop o nome correto
// das estruturas de dados dos protocolos.

unsigned char buff1[BUFFSIZE]; // Buffer de recepcao.

int sockd;
int on;
struct ifreq ifr;

// Cabecalho das funcoes auxiliares.
void printEthernetHeader(unsigned char buff1[]);
void printARPHeader(unsigned char buff1[]);
void printICMPv4Header(unsigned char buff1[]);
void printICMPv6Header(unsigned char buff1[]);
void printIPv4Header(unsigned char buff1[]);
void printIPv6Header(unsigned char buff1[]);
void printTCPHeader(unsigned char buff1[], char networkProtocol[]);
void printUDPHeader(unsigned char buff1[], char networkProtocol[]);

// Printa o cabecalho do Ethernet.
void printEthernetHeader(unsigned char buff1[])
{
    printf("\n=== HEADER ETHERNET ===\n");
    printf("\nDestination: %x:%x:%x:%x:%x:%x\n", buff1[0], buff1[1], buff1[2], buff1[3], buff1[4], buff1[5]);
    printf("Source:  %x:%x:%x:%x:%x:%x\n", buff1[6], buff1[7], buff1[8], buff1[9], buff1[10], buff1[11]);
    printf("Type: %x%x\n", buff1[12], buff1[13]);
}
// Printa o cabecalho do ARP.
void printARPHeader(unsigned char buff1[])
{
    printf("\n=== HEADER ARP ===\n");
    printf("\nHardware Type: %d\n", (buff1[14] << 8) | (buff1[15]));
    printf("Protocol Type: %x\n", (buff1[16] << 8) | (buff1[17]));
    printf("Hardware Size: %d\n", buff1[18]);
    printf("Protocol Size: %d\n", buff1[19]);
    printf("Opcode: %d\n", (buff1[20] << 8) | (buff1[21]));
    printf("Sender MAC Address: %x:%x:%x:%x:%x:%x\n", buff1[22], buff1[23], buff1[24], buff1[25], buff1[26], buff1[27]);
    printf("Sender IP Address: %d:%d:%d:%d\n", buff1[28], buff1[29], buff1[30], buff1[31]);
    printf("Target MAC Address: %x:%x:%x:%x:%x:%x\n", buff1[32], buff1[33], buff1[34], buff1[35], buff1[36], buff1[37]);
    printf("Target IP Address: %d:%d:%d:%d\n", buff1[38], buff1[39], buff1[40], buff1[41]);
}
// Printa o cabecalho do IPv4.
void printIPv4Header(unsigned char buff1[])
{
    printf("\n=== HEADER IPv4 ===\n");
    printf("\nVersion: %d\n", buff1[14] >> 4);
    unsigned char aux = buff1[14] << 4;
    printf("Header Length: %d\n", aux >> 4);
    printf("Differentiated Services Field: %x\n", buff1[15]);
    printf("Total Length: %d\n", (buff1[16] << 8) | (buff1[17]));
    printf("Identification: %d\n", (buff1[18] << 8) | (buff1[19]));
    printf("Flags: %x%x\n", buff1[20], buff1[21]);
    printf("Time to Live: %d\n", buff1[22]);
    printf("Protocol: %d\n", buff1[23]);
    printf("Header Checksum: %d\n", (buff1[24] << 8) | (buff1[25]));
    printf("Source: %d:%d:%d:%d\n", buff1[26], buff1[27], buff1[28], buff1[29]);
    printf("Destination: %d:%d:%d:%d\n", buff1[30], buff1[31], buff1[32], buff1[33]);
}
// Printa o cabecalho do IPv6.
void printIPv6Header(unsigned char buff1[])
{
    printf("\n=== HEADER IPv6 ===\n");
    printf("\nVersion: %d\n", buff1[14] >> 4);
    unsigned char aux = buff1[14] << 4;
    printf("Traffic Class: %x%x%x%x\n", aux >> 4, buff1[15], buff1[16], buff1[17]);
    printf("Payload Length: %d\n", (buff1[18] << 8) | (buff1[19]));
    printf("Next Header: %x\n", buff1[20]);
    printf("Hop Limit: %d\n", buff1[21]);
    printf("Source: %x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x\n", buff1[21], buff1[22], buff1[23], buff1[24], buff1[25], buff1[26], buff1[27], buff1[28], buff1[29], buff1[30], buff1[31], buff1[32], buff1[33], buff1[34], buff1[35], buff1[36]);
    printf("Destination: %x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x\n", buff1[37], buff1[38], buff1[39], buff1[40], buff1[41], buff1[42], buff1[43], buff1[44], buff1[45], buff1[46], buff1[47], buff1[48], buff1[49], buff1[50], buff1[51], buff1[52]);
}
// Printa o cabecalho do ICMPv4.
void printICMPv4Header(unsigned char buff1[])
{
    printf("\n=== HEADER ICMPv4 ===\n");
    printf("\nType: %d\n", buff1[34]);
    printf("Code: %d\n", buff1[35]);
    printf("Checksum: %x\n", (buff1[36] << 8) | (buff1[37]));
    printf("Identifier: %x %x\n", buff1[38], buff1[39]);
    printf("Sequence Number: %x %x\n", buff1[40], buff1[41]);
}
// Printa o cabecalho do ICMPv6.
void printICMPv6Header(unsigned char buff1[])
{
    printf("\n=== HEADER ICMPv6 ===\n");
    printf("\nType: %d\n", buff1[54]);
    printf("Code: %d\n", buff1[55]);
    printf("Checksum: %x\n", (buff1[56] << 8) | (buff1[57]));
    printf("Reserved: %x%x%x%x\n", buff1[58], buff1[59], buff1[60], buff1[61]);
    printf("Target Address: %x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x:%x\n", buff1[62], buff1[63], buff1[64], buff1[65], buff1[66], buff1[67], buff1[68], buff1[69], buff1[70], buff1[71], buff1[72], buff1[73], buff1[74], buff1[75], buff1[76], buff1[77]);
    printf("ICMPv6 Option (Type): %d\n", buff1[78]);
    printf("ICMPv6 Option (Length): %d\n", buff1[79]);
    printf("ICMPv6 Option (Link-layer Address): %x%x%x%x%x%x\n", buff1[80], buff1[81], buff1[82], buff1[83], buff1[84], buff1[85]);
}
// Printa o cabecalho do TCP.
void printTCPHeader(unsigned char buff1[], char networkProtocol[])
{
    printf("\n=== HEADER TCP ===\n");
    int offset;
    if (strcmp(networkProtocol, "IPv4") == 0)
    {
        offset = 34;
    }
    else if (strcmp(networkProtocol, "IPv6") == 0)
    {
        offset = 54;
    }
    printf("\nSource Port: %d\n", (buff1[offset] << 8) | (buff1[offset + 1]));
    printf("Destination Port: %d\n", (buff1[offset + 2] << 8) | (buff1[offset + 3]));
    printf("TCP Segment Len: %d\n", buff1[offset + 11]);
    printf("Sequence Number (Raw): %x %x %x %x\n", buff1[offset + 4], buff1[offset + 5], buff1[offset + 6], buff1[offset + 7]);
    printf("Acknowledgment Number (Raw): %x %x %x %x\n", buff1[offset + 8], buff1[offset + 9], buff1[offset + 10], buff1[offset + 11]);
    printf("Header Length: %d\n", buff1[offset + 12] >> 4);
    unsigned char aux = buff1[offset + 12] << 4;
    printf("Flags: %x%x\n", aux >> 4, buff1[offset + 13]);
    printf("Window: %d\n", (buff1[offset + 14] << 8) | (buff1[offset + 15]));
    printf("Checksum: %x\n", (buff1[offset + 16] << 8) | (buff1[offset + 17]));
    printf("Urgent Pointer: %x%x\n", buff1[offset + 18], buff1[offset + 19]);
}
// Printa o cabecalho do UDP.
void printUDPHeader(unsigned char buff1[], char networkProtocol[])
{
    printf("\n=== HEADER UDP ===\n");
    int offset;
    if (strcmp(networkProtocol, "IPv4") == 0)
    {
        offset = 34;
    }
    else if (strcmp(networkProtocol, "IPv6") == 0)
    {
        offset = 54;
    }
    printf("\nSource Port: %d\n", (buff1[offset] << 8) | (buff1[offset + 1]));
    printf("Destination Port: %d\n", (buff1[offset + 2] << 8) | (buff1[offset + 3]));
    printf("Length: %d\n", (buff1[offset + 4] << 8) | (buff1[offset + 5]));
    printf("Checksum: %x\n", (buff1[offset + 6] << 8) | (buff1[offset + 7]));
}

int main(int argc, char *argv[])
{
    /* Criacao do socket. Todos os pacotes devem ser construidos a partir do protocolo Ethernet. */
    /* De um "man" para ver os parametros.*/
    /* htons: converte um short (2-byte) integer para standard network byte order. */
    if ((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    {
        printf("Erro na criacao do socket.\n");
        exit(1);
    }

    // O procedimento abaixo eh utilizado para "setar" a interface em modo promiscuo.
    strcpy(ifr.ifr_name, "eth0");
    if (ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
        printf("Erro no ioctl!");
    ioctl(sockd, SIOCGIFFLAGS, &ifr);
    ifr.ifr_flags |= IFF_PROMISC;
    ioctl(sockd, SIOCSIFFLAGS, &ifr);

    // Contadores.
    int TOTAL_PACKAGES = 0;

    int IPV4 = 0;
    int IPV6 = 0;
    int ICMPV4 = 0;
    int ICMPV6 = 0;
    int ARP = 0;

    int TCP = 0;
    int UDP = 0;

    int SMTP_TRANSMITTED = 0, SMTP_RECEIVED = 0;
    int HTTP_TRANSMITTED = 0, HTTP_RECEIVED = 0;
    int HTTPS_TRANSMITTED = 0, HTTPS_RECEIVED = 0;
    int DHCP_TRANSMITTED = 0, DHCP_RECEIVED = 0;
    int DNS_TRANSMITTED = 0, DNS_RECEIVED = 0;

    // Recepcao de pacotes.
    while (TOTAL_PACKAGES < MAX_PACKAGES)
    {
        recv(sockd, (char *)&buff1, sizeof(buff1), 0x0);

        TOTAL_PACKAGES++;
        printEthernetHeader(buff1);

        if (buff1[12] == 0x08 && buff1[13] == 0x00) // IPv4.
        {
            IPV4++;
            printIPv4Header(buff1);

            // Acessar a posicao de protocol [23].
            if (buff1[23] == 0x01)
            {
                ICMPV4++;
                printICMPv4Header(buff1);
            }
            else if (buff1[23] == 0x06)
            {
                TCP++;
                printTCPHeader(buff1, "IPv4");

                // Checar a porta destino:

                // SMTP (Porta 25).
                if (buff1[36] == 0x00 && buff1[37] == 0x19)
                {
                    printf("\nProtocolo de Aplicacao: SMTP\n");
                    SMTP_TRANSMITTED++;
                }
                // HTTP (Porta 80).
                else if (buff1[36] == 0x00 && buff1[37] == 0x50)
                {
                    printf("\nProtocolo de Aplicacao: HTTP\n");
                    HTTP_TRANSMITTED++;
                }
                // HTTPS (Porta 443).
                else if (buff1[36] == 0x01 && buff1[37] == 0xbb)
                {
                    printf("\nProtocolo de Aplicacao: HTTPS\n");
                    HTTPS_TRANSMITTED++;
                }

                // Checar a porta origem:
                if (buff1[34] == 0x00 && buff1[35] == 0x19)
                {
                    SMTP_RECEIVED++;
                }
                else if (buff1[34] == 0x00 && buff1[35] == 0x50)
                {
                    HTTP_RECEIVED++;
                }
                else if (buff1[34] == 0x01 && buff1[35] == 0xbb)
                {
                    HTTPS_RECEIVED++;
                }
            }
            else if (buff1[23] == 0x11)
            {
                UDP++;
                printUDPHeader(buff1, "IPv4");

                // Checar a porta destino:

                // DHCP (Porta 67).
                if (buff1[36] == 0x00 && buff1[37] == 0x43)
                {
                    printf("\nProtocolo de Aplicacao: DHCP\n");
                    DHCP_TRANSMITTED++;
                }
                // DNS (Porta 53).
                else if (buff1[36] == 0x00 && buff1[37] == 0x35)
                {
                    printf("\nProtocolo de Aplicacao: DNS\n");
                    DNS_TRANSMITTED++;
                }

                // Checar a porta origem:

                if (buff1[36] == 0x00 && buff1[37] == 0x43)
                {
                    DHCP_RECEIVED++;
                }
                else if (buff1[36] == 0x00 && buff1[37] == 0x35)
                {
                    DNS_RECEIVED++;
                }
            }
        }
        else if (buff1[12] == 0x08 && buff1[13] == 0x06) // ARP.
        {
            ARP++;
            printARPHeader(buff1);
        }
        else if (buff1[12] == 0x86 && buff1[13] == 0xdd) // IPv6.
        {
            IPV6++;
            printIPv6Header(buff1);

            // Acessar a posicao de next header [20].
            if (buff1[20] == 0x3a)
            {
                ICMPV6++;
                printICMPv6Header(buff1);
            }
            else if (buff1[20] == 0x06)
            {
                TCP++;
                printTCPHeader(buff1, "IPv6");
            }
            else if (buff1[20] == 0x11)
            {
                UDP++;
                printUDPHeader(buff1, "IPv6");
            }
        }
    }

    // Estatisticas.
    printf("\n<---- Estatisticas ---->\n");

    printf("\nTotal de pacotes capturados: %d", TOTAL_PACKAGES);
    printf("\n%c de pacotes ARP capturados: %.3f", '%', ((float)ARP / TOTAL_PACKAGES) * 100);
    printf("\n%c de pacotes IPv4 capturados: %.3f", '%', ((float)IPV4 / TOTAL_PACKAGES) * 100);
    printf("\n%c de pacotes IPv6 capturados: %.3f", '%', ((float)IPV6 / TOTAL_PACKAGES) * 100);
    printf("\n%c de pacotes ICMPv4 capturados: %.3f", '%', ((float)ICMPV4 / TOTAL_PACKAGES) * 100);
    printf("\n%c de pacotes ICMPv6 capturados: %.3f", '%', ((float)ICMPV6 / TOTAL_PACKAGES) * 100);
    printf("\n%c de pacotes TCP capturados: %3f", '%', ((float)TCP / TOTAL_PACKAGES) * 100);
    printf("\n\% de pacotes UDP capturados: %.3f", '%', ((float)UDP / TOTAL_PACKAGES) * 100);

    int mostTransmittedAplication = MAX(SMTP_TRANSMITTED, HTTP_TRANSMITTED);
    mostTransmittedAplication = MAX(mostTransmittedAplication, HTTPS_TRANSMITTED);
    mostTransmittedAplication = MAX(mostTransmittedAplication, DHCP_TRANSMITTED);
    mostTransmittedAplication = MAX(mostTransmittedAplication, DNS_TRANSMITTED);

    if (mostTransmittedAplication == SMTP_TRANSMITTED)
    {
        printf("Protocolo de aplicacao mais usado nas transmissoes: SMTP");
    }
    else if (mostTransmittedAplication == HTTP_TRANSMITTED)
    {
        printf("Protocolo de aplicacao mais usado nas transmissoes: HTTP");
    }
    else if (mostTransmittedAplication == HTTPS_TRANSMITTED)
    {
        printf("Protocolo de aplicacao mais usado nas transmissoes: HTTPS");
    }
    else if (mostTransmittedAplication == DHCP_TRANSMITTED)
    {
        printf("Protocolo de aplicacao mais usado nas transmissoes: DHCP");
    }
    else if (mostTransmittedAplication == DNS_TRANSMITTED)
    {
        printf("Protocolo de aplicacao mais usado nas transmissoes: DNS");
    }
}