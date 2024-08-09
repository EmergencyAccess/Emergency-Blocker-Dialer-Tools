#ifndef BLOCK_IPV4_H_
#define BLOCK_IPV4_H_

// #include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <linux/ip.h>
#include <netinet/tcp.h>
#include <linux/netfilter.h>        /* for NF_ACCEPT */
#include <errno.h>
#include <netinet/ip6.h>
#include <string.h>

struct blockIP4 {
    // Member variables
    int data;
    u_int16_t payload_len;
    struct iphdr *iph;
    char str_src[INET_ADDRSTRLEN], str_dst[INET_ADDRSTRLEN];
    struct in_addr *src_addr;
    struct in_addr *dst_addr;
    u_int16_t src_port;
    u_int16_t dst_port;
    
};  

uint16_t tcp4_checksum(struct iphdr iphdr, struct tcphdr tcphdr, char *data);
uint16_t checksum (uint16_t *addr, int len);
void ipv4_to_str(char *str, struct in_addr *addr);
void ipv4_sendACK(char *srcIP, char *dstIP, u_int16_t source_port, u_int16_t dest_port, u_int32_t seq, u_int32_t ack, int packet_len);
void ipv4_send100Trying(char *srcIP, char *dstIP, u_int16_t source_port, u_int16_t dest_port, u_int32_t seq, u_int32_t ack, int packet_len, char* payload);
unsigned short tcp_checksum(const void *ip_header, size_t ip_header_len, const void *tcp_header, size_t tcp_header_len, const void *payload, size_t payload_len);



#endif