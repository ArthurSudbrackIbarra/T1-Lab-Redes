// 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

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

// Atencao!! Confira no /usr/include do seu sisop o nome correto
// das estruturas de dados dos protocolos.

unsigned char buff1[BUFFSIZE]; // Buffer de recepcao.

int sockd;
int on;
struct ifreq ifr;

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
    strcpy(ifr.ifr_name, "enp4s0");
    if (ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
        printf("erro no ioctl!");
    ioctl(sockd, SIOCGIFFLAGS, &ifr);
    ifr.ifr_flags |= IFF_PROMISC;
    ioctl(sockd, SIOCSIFFLAGS, &ifr);

    // Contadores.
    int IPV4 = 0;
    int IPV6 = 0;
    int ICMPV4 = 0;
    int ICMPV6 = 0;
    int ARP = 0;
    int TCP = 0;
    int UDP = 0;
    int SMTP = 0;
    int HTTP = 0;
    int HTTPS = 0;
    int DHCP = 0;
    int DNS = 0;

    // Recepcao de pacotes.
    while (1)
    {
        recv(sockd, (char *)&buff1, sizeof(buff1), 0x0);
        // Impressao do conteudo - exemplo Endereco Destino e Endereco Origem.
        printf("\n\nMAC Destino: %x:%x:%x:%x:%x:%x \n", buff1[0], buff1[1], buff1[2], buff1[3], buff1[4], buff1[5]);
        printf("MAC Origem:  %x:%x:%x:%x:%x:%x \n", buff1[6], buff1[7], buff1[8], buff1[9], buff1[10], buff1[11]);

        if (buff1[12] == 0x08 && buff1[13] == 0x00) // IPv4.
        {
            printf("Protocolo de Enlace: IPv4\n");
            IPV4++;

            // Acessar a posicao de protocol [23].
            if (buff1[23] == 0x01)
            {
                printf("Protocolo ICMP: ICMPv4\n");
                ICMPV4++;
            }
            else if (buff1[23] == 0x06)
            {
                printf("Protocolo de Transporte: TCP\n");
                TCP++;

                // Checar a porta:

                // SMTP (Porta 25).
                if (buff1[36] == 0x00 && buff1[37] == 0x19)
                {
                    printf("Protocolo de Aplicacao: SMTP\n");
                    SMTP++;
                }
                // HTTP (Porta 80).
                else if (buff1[36] == 0x00 && buff1[37] == 0x50)
                {
                    printf("Protocolo de Aplicacao: HTTP\n");
                    HTTP++;
                }
                // HTTPS (Porta 443).
                else if (buff1[36] == 0x01 && buff1[37] == 0xbb)
                {
                    printf("Protocolo de Aplicacao: HTTPS\n");
                    HTTPS++;
                }
            }
            else if (buff1[23] == 0x11)
            {
                printf("Protocolo de Transporte: UDP\n");
                UDP++;

                // Checar a porta:

                // DHCP (Porta 67).
                if (buff1[36] == 0x00 && buff1[37] == 0x43)
                {
                    printf("Protocolo de Aplicacao: DHCP\n");
                    DHCP++;
                }
                // DNS (Porta 53).
                else if (buff1[36] == 0x00 && buff1[37] == 0x35)
                {
                    printf("Protocolo de Aplicacao: DNS\n");
                    DNS++;
                }
            }
        }
        else if (buff1[12] == 0x08 && buff1[13] == 0x06) // ARP.
        {
            printf("Protocolo ARP: ARP\n");
            ARP++;
        }
        else // IPv6.
        {
            printf("Protocolo de Enlace: IPv6\n");
            IPV6++;

            // Acessar a posicao de next header [20].
            if (buff1[20] == 0x3a)
            {
                printf("Protocolo ICMP: ICMPv6\n");
                ICMPV6++;
            }
            else if (buff1[20] == 0x06)
            {
                printf("Protocolo de Transporte: TCP\n");
                TCP++;
            }
            else if (buff1[20] == 0x11)
            {
                printf("Protocolo de Transporte: UDP\n");
                UDP++;
            }
        }
    }
}