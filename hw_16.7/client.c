#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <stdint.h>
#include <net/if.h>

// Добавляю свои MAC-адреса
static const unsigned char SRC_MAC[6] = {0x00, 0x15, 0x5d, 0xef, 0xeb, 0x62}; 
static const unsigned char DEST_MAC[6] = {0x00, 0x15, 0x5d, 0xfc, 0x12, 0x28};

uint16_t ip_checksum(void *data, size_t len) {
    uint32_t sum = 0;
    uint16_t *ptr = (uint16_t*)data;
    
    // Суммируем 16-bit
    while (len > 1) {
        sum += *ptr++;
        len -= 2;
    }
    
    // Если остался 1 байт (нечётная длина)
    if (len) {
        sum += *(uint8_t*)ptr;
    }
    
    // Сворачиваем пока не останется 16 бит
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    // Инвертируем
    return (uint16_t)(~sum);
}

struct udp_header{
    uint16_t source_port;
    uint16_t destination_port;
    uint16_t length;
    uint16_t check_sum;
};

int main(void){
    int server_fd;
    struct sockaddr_ll server = {0};
    char buffer[1500] = {0};  
    char read_buffer[1500] = {0};  
    struct udp_header my_udp = {0};
    struct iphdr ip = {0};

    char *response = "Hello!\n";
    int response_length = strlen(response);

    // Создаем сокет
    server_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (server_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }

    server.sll_family = AF_PACKET;
    server.sll_ifindex = if_nametoindex("eth0");
    server.sll_halen = 6;
    memcpy(server.sll_addr, DEST_MAC, 6);
    socklen_t server_len = sizeof(server);

    // Заполняем Ethernet-заголовок в структуре ethhdr
    unsigned char frame[1514];
    struct ethhdr *eth = (struct ethhdr*)frame;

    memcpy(eth->h_dest, DEST_MAC, 6);
    memcpy(eth->h_source, SRC_MAC, 6);
    eth->h_proto = htons(0x0800); // IPv4

    // Сохраняем Ethernet заголовок
    memcpy(buffer, eth, sizeof(*eth));

    // Заполняем IP-заголовок в структуре iphdr
    ip.version = 4;
    ip.ihl = 5;
    ip.tos = 0;
    ip.tot_len = htons(sizeof(struct iphdr) + sizeof(struct udp_header) + response_length);;
    ip.id = 0;
    ip.frag_off = 0;
    ip.ttl = 200;
    ip.protocol = IPPROTO_UDP;

    // Мои локальные ip-адреса
    inet_pton(AF_INET, "172.31.23.166", &ip.saddr);
    inet_pton(AF_INET, "172.31.16.1", &ip.daddr);

    // Заполняем checksum для ip
    ip.check = 0;
    uint16_t csum = ip_checksum(&ip, sizeof(ip));
    ip.check = csum;

    // Сохраняем ip заголовок
    memcpy(buffer + sizeof(*eth), &ip, sizeof(ip));

    // Заполняем заголовок UDP
    my_udp.source_port = htons(33333);
    my_udp.destination_port = htons(12345);

    // Высчитываем общую длину UDP-дейтаграммы, как сумму длины передаваеммых данных и размера стурктуры заголовка
    my_udp.length = htons(sizeof(my_udp) + response_length);

    // Checksum udp не проверяем
    my_udp.check_sum = 0;

    // Сохраняем область памяти структуры my_udp в buffer, после IP заголовка
    memcpy(buffer + sizeof(*eth) + sizeof(ip), &my_udp, sizeof(my_udp));

    // Сохраняем область памяти передаваемых данных в buffer, после структуры my_udp
    memcpy(buffer + sizeof(*eth) + sizeof(ip) + sizeof(my_udp), response, response_length);

    // Отправляем сообщение серверу
    if (sendto(server_fd, buffer, sizeof(*eth)+ sizeof(ip) + sizeof(my_udp) + response_length, 0, (struct sockaddr *)&server, server_len) < 0){
        perror("send problem");
        close(server_fd);
        exit(EXIT_FAILURE);        
    }

    printf("Клиент отправил: %s\n", response);
    
    while(1){
        // Получаем сообщение
        ssize_t n = recv(server_fd, read_buffer, sizeof(read_buffer), 0);
        if (n < 0){
            perror("recv problem");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // Находим в принятом потоке данных порт получателя и проверяем, чтобы он был наш
        uint16_t check_for_my_port = *(uint16_t *)(read_buffer + 14 + 20 + 2); // Ethernet + ip заголовки + смещение до порта
        if(my_udp.source_port == check_for_my_port){
            int data_offset = 14 + 20 + 8;
            int data_len = n - data_offset;
            printf("Raw-сокет прочитал (%d байт): %.*s\n", data_len, data_len, read_buffer + data_offset);
        }
    }

    close(server_fd);
    return 0;
}