#ifndef CONNECTION_DEFS_H
#define CONNECTION_DEFS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVER_PORT 12345
#define MAX_DATA_SIZE 1500
#define SOURCE_IP_OFFSET 12
#define DESTINATION_IP_OFFSET 16
#define SOURCE_PORT_OFFSET 20
#define DESTINATION_PORT_OFFSET 22
#define PAYLOAD_OFFSET 28 // 20 IP + 8 UDP

struct udp_header{
    uint16_t source_port;
    uint16_t destination_port;
    uint16_t length;
    uint16_t check_sum;
};

struct client{
    struct sockaddr_in client_endpoint;
    int message_count;
    char packet_ip_udp_data[MAX_DATA_SIZE];
    struct client *next;
};

int init_raw_socket();
struct iphdr init_iphdr(uint32_t daddr);


#endif