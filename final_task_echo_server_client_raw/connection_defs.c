#include "connection_defs.h"

int init_raw_socket(){

    // Создаем RAW-сокет 
    int fd_name = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (fd_name < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }   
    
    int flag = 1; 

    // Говорим ядру опцией IP_HDRINCL, что сами заполним заголовок IP
    if(setsockopt(fd_name, IPPROTO_IP, IP_HDRINCL, &flag, sizeof(flag)) < 0){
        perror("setsockopt problem");
        exit(EXIT_FAILURE);
    }
    return fd_name;
}

struct iphdr init_iphdr(uint32_t daddr){
    // Заполняем IP-заголовок в структуре iphdr
    struct iphdr ip = {0};
    ip.version = 4;
    ip.ihl = 5;
    ip.ttl = 200;
    ip.protocol = IPPROTO_UDP;
    inet_pton(AF_INET, "127.0.0.1", &ip.saddr);
    ip.daddr = daddr;
    return ip;
}