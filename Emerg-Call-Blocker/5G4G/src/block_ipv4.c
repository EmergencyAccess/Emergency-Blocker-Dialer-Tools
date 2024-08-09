#include "block_ipv4.h"


void ipv4_to_str(char *str, struct in_addr *addr) {
    // Use inet_ntop to convert IPv4 address to string
    inet_ntop(AF_INET, addr, str, INET_ADDRSTRLEN);
    // printf("333 Source IP address: %s\n", str);
}

unsigned short tcp_checksum(const void *ip_header, size_t ip_header_len, const void *tcp_header, size_t tcp_header_len, const void *payload, size_t payload_len) {
    uint32_t sum = 0;

    // Sum pseudo-header
    const uint16_t *ip_ptr = ip_header;
    for (size_t i = 0; i < ip_header_len / 2; ++i) {
        sum += ntohs(ip_ptr[i]);
    }

    // Sum TCP header and payload
    const uint16_t *tcp_ptr = tcp_header;
    for (size_t i = 0; i < tcp_header_len / 2; ++i) {
        sum += ntohs(tcp_ptr[i]);
    }

    // If payload is present, include it in the checksum
    if (payload != NULL && payload_len > 0) {
        const uint16_t *payload_ptr = payload;
        for (size_t i = 0; i < payload_len / 2; ++i) {
            sum += ntohs(payload_ptr[i]);
        }
    }

    // Fold 32-bit sum to 16 bits
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // Take one's complement
    return ~((uint16_t)sum);
}

void ipv4_sendACK(char *s_ip, char *d_ip, u_int16_t s_port, u_int16_t d_port, u_int32_t seq, u_int32_t ack, int packet_len) {
    printf("IPv4: Sending TCP ACK...\n");

    int s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (s == -1) {
        perror("Failed to create raw socket");
        exit(1);
    }

    // Datagram to represent the packet
    char datagram[4096];

    // Zero out the packet buffer
    memset(datagram, 0, 4096);

    // IP header
    struct iphdr iph;

    // TCP header
    struct tcphdr tcph;

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(d_port);
    inet_pton(AF_INET, d_ip, &(sin.sin_addr));
    // printf("ACK: Source IP address: %s\n", pkt_info.str_src);
    

    iph.version = 4; // IPv4
    iph.ihl = 5; // Header length in 32-bit words (5 for standard headers)
    iph.tos = 0; // Type of service (usually set to 0)
    iph.tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr) + packet_len); // Total length of the packet
    iph.id = htons(54321); // Identification (you can set it to any value)
    iph.frag_off = 0; // Fragment offset (usually set to 0)
    iph.ttl = 60; // Time to live
    iph.protocol = IPPROTO_TCP; // Protocol (TCP)
    iph.check = 0; // Checksum initially set to 0

    // Reverse the srcIP and dstIP
    inet_pton(AF_INET, d_ip, &(iph.daddr));
    inet_pton(AF_INET, s_ip, &(iph.saddr));
    char source_ip_buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(iph.daddr), source_ip_buf, INET_ADDRSTRLEN);
    // printf("ACK Source IP address: %s\n", inet_ntoa(sin.sin_addr));
    // printf("Destination IP address: %s\n", source_ip_buf);
    // printf("seq: %u\n", seq);
    // printf("ack: %u\n", ack);
    // printf("payload len: %d\n", packet_len);
    
    // printf("ACK Destination IP address: %s\n", inet_ntoa(iph.daddr));
    

    tcph.th_sport = htons(s_port);
    tcph.th_dport = htons(d_port);
    tcph.th_seq = htonl(ack);
    tcph.th_ack = htonl(seq + packet_len);
    tcph.th_x2 = 0; // Reserved (4 bits): should be 0
    tcph.th_off = 5; // Data offset (4 bits): size of TCP header in 32-bit words
    tcph.th_flags = TH_ACK; // Set ACK flag
    tcph.th_win = htons(65535); // Window size (16 bits)
    tcph.th_sum = 0; // Checksum initially set to 0
    tcph.th_urp = htons(0); // Urgent pointer (16 bits): 0

    // Calculate TCP checksum
    tcph.th_sum = tcp4_checksum(iph, tcph, "");

    // Copy the headers and payload to the datagram
    memcpy(datagram, &iph, sizeof(struct iphdr));
    memcpy(datagram + sizeof(struct iphdr), &tcph, sizeof(struct tcphdr));

    // printf("Now sending TCP ACK\n");

    if (sendto(s, datagram, sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen(""), 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("sendto failed");
    } else {
        // printf("TEST: tot: %u\n", ntohs(iph.tot_len));
        // ph.tot_len in host byte order: %u\n", ntohs(iph.tot_len)
        // printf("IPv4: ACK sent for ack: %u!\n", ntohl(tcph.th_ack));
        printf("IPv4: TCP ACK sent!\n");
    }
}

// Checksum function
uint16_t checksum (uint16_t *addr, int len)
{
  int nleft = len;
  int sum = 0;
  uint16_t *w = addr;
  uint16_t answer = 0;

  while (nleft > 1) {
    sum += *w++;
    nleft -= sizeof (uint16_t);
  }

  if (nleft == 1) {
    *(uint8_t *) (&answer) = *(uint8_t *) w;
    sum += answer;
  }

  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  answer = ~sum;
  return (answer);
}

uint16_t tcp4_checksum(struct iphdr iphdr, struct tcphdr tcphdr, char *data) {
    uint32_t lvalue;
    char buf[65535], cvalue;
    char *ptr;
    int chksumlen = 0;
    int payloadLen = strlen(data);

    ptr = &buf[0];  // ptr points to the beginning of buffer buf

    // Copy source IP address into buf (32 bits)
    lvalue = htonl(iphdr.saddr);
    memcpy(ptr, &lvalue, sizeof(lvalue));
    ptr += sizeof(lvalue);
    chksumlen += sizeof(lvalue);

    // Copy destination IP address into buf (32 bits)
    lvalue = htonl(iphdr.daddr);
    memcpy(ptr, &lvalue, sizeof(lvalue));
    ptr += sizeof(lvalue);
    chksumlen += sizeof(lvalue);

    // Copy zero field to buf (8 bits)
    *ptr = 0;
    ptr++;
    chksumlen += 1;

    // Copy protocol field to buf (8 bits)
    memcpy(ptr, &iphdr.protocol, sizeof(iphdr.protocol));
    ptr += sizeof(iphdr.protocol);
    chksumlen += sizeof(iphdr.protocol);

    // Copy TCP length (TCP header + payload) to buf (16 bits)
    lvalue = htons(sizeof(tcphdr) + payloadLen);
    memcpy(ptr, &lvalue, sizeof(lvalue));
    ptr += sizeof(lvalue);
    chksumlen += sizeof(lvalue);

    // Copy TCP source port to buf (16 bits)
    memcpy(ptr, &tcphdr.th_sport, sizeof(tcphdr.th_sport));
    ptr += sizeof(tcphdr.th_sport);
    chksumlen += sizeof(tcphdr.th_sport);

    // Copy TCP destination port to buf (16 bits)
    memcpy(ptr, &tcphdr.th_dport, sizeof(tcphdr.th_dport));
    ptr += sizeof(tcphdr.th_dport);
    chksumlen += sizeof(tcphdr.th_dport);

    // Copy sequence number to buf (32 bits)
    memcpy(ptr, &tcphdr.th_seq, sizeof(tcphdr.th_seq));
    ptr += sizeof(tcphdr.th_seq);
    chksumlen += sizeof(tcphdr.th_seq);

    // Copy acknowledgement number to buf (32 bits)
    memcpy(ptr, &tcphdr.th_ack, sizeof(tcphdr.th_ack));
    ptr += sizeof(tcphdr.th_ack);
    chksumlen += sizeof(tcphdr.th_ack);

    // Copy data offset to buf (4 bits) and
    // copy reserved bits to buf (4 bits)
    cvalue = (tcphdr.th_off << 4) + tcphdr.th_x2;
    memcpy(ptr, &cvalue, sizeof(cvalue));
    ptr += sizeof(cvalue);
    chksumlen += sizeof(cvalue);

    // Copy TCP flags to buf (8 bits)
    memcpy(ptr, &tcphdr.th_flags, sizeof(tcphdr.th_flags));
    ptr += sizeof(tcphdr.th_flags);
    chksumlen += sizeof(tcphdr.th_flags);

    // Copy TCP window size to buf (16 bits)
    memcpy(ptr, &tcphdr.th_win, sizeof(tcphdr.th_win));
    ptr += sizeof(tcphdr.th_win);
    chksumlen += sizeof(tcphdr.th_win);

    // Copy TCP checksum to buf (16 bits)
    // Zero, since we don't know it yet
    *ptr = 0;
    ptr++;
    chksumlen += 1;

    // Copy urgent pointer to buf (16 bits)
    memcpy(ptr, &tcphdr.th_urp, sizeof(tcphdr.th_urp));
    ptr += sizeof(tcphdr.th_urp);
    chksumlen += sizeof(tcphdr.th_urp);

    // Copy TCP payload to buf
    memcpy(ptr, data, payloadLen);
    ptr += payloadLen;
    chksumlen += payloadLen;

    return checksum((uint16_t *)buf, chksumlen);
}



void ipv4_send100Trying(char *s_ip, char *d_ip, u_int16_t s_port, u_int16_t d_port, u_int32_t seq, u_int32_t ack, int packet_len, char* payload) {
    // char *sip_trying_data;
    // Check generateSIPTrying()
    // sip_trying_data = generateSipTrying();
    
    // printf("000Len: %d,  100 Trying:\n%s\n", 123, payload);
    size_t payload_len = strlen(payload);
    // printf("Src IP: %s\n", srcIP);
    // printf("Dst IP: %s\n", dstIP);
    
    char t_payload[payload_len];
    memcpy(t_payload, payload, payload_len);
    t_payload[payload_len] = '\0';
    // printf("111Len: %d,  100 Trying:\n%s\n", payload_len, t_payload);

    int s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (s == -1) {
        // socket creation failed, may be because of non-root privileges
        perror("Failed to create raw socket");
        exit(1);
    }

    // Datagram to represent the packet
    char datagram[2048];

    // Zero out the packet buffer
    memset(datagram, 0, 2048);

    // IP header
    struct iphdr ip;

    // TCP header
    struct tcphdr tcph;

    memcpy(datagram + sizeof(struct iphdr) + sizeof(struct tcphdr), t_payload, payload_len);
    
    // printf("TCP packet:\n");
    // for (size_t i = 0; i < sizeof(struct iphdr) + sizeof(struct tcphdr) + payload_len; ++i) {
    //     printf("%02X ", (unsigned char)datagram[i]);
    // }

    // printf("\n");

    // printf("222Len: %d,  100 Trying:\n%s\n", payload_len, t_payload);
    // Initialize sockaddr_in structure
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(d_port);
    inet_pton(AF_INET, d_ip, &(sin.sin_addr));

    // IP header setup
    ip.ihl = 5;
    ip.version = 4;
    ip.tos = 0;
    ip.tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr) + payload_len);
    ip.id = htons(54321); // Random identification number
    ip.frag_off = 0;
    ip.ttl = 60;
    ip.protocol = IPPROTO_TCP;
    ip.check = 0; // Let the kernel calculate the checksum

    // Reverse srcIP and dstIP
    ip.saddr = inet_addr(s_ip);
    ip.daddr = inet_addr(d_ip);

    // TCP header setup
    tcph.th_sport = htons(s_port);
    tcph.th_dport = htons(d_port);
    tcph.th_seq = htonl(ack);
    tcph.th_ack = htonl(seq + packet_len);
    tcph.th_off = 5; // Data offset in 32-bit words (20 bytes)
    tcph.th_flags = TH_PUSH | TH_ACK; // Flags (Push and Acknowledge)
    tcph.th_win = htons(65535);
    tcph.th_urp = htons(0);

    // Calculate TCP checksum
    // tcph.th_sum = tcp_checksum((unsigned short *)&iph, sizeof(struct iphdr), (unsigned short *)&tcph, sizeof(struct tcphdr), "", packet_len);

    tcph.th_sum = tcp4_checksum(ip, tcph, t_payload);

    // Copy headers into datagram
    memcpy(datagram, &ip, sizeof(struct iphdr));
    memcpy(datagram + sizeof(struct iphdr), &tcph, sizeof(struct tcphdr));

    printf("IPv4: Sending 100 Trying...\n");

    if (sendto(s, datagram, sizeof(struct iphdr) + sizeof(struct tcphdr) + payload_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("sendto failed");
    } else {
        printf("100 Trying Sent. Length: %d\n", ntohs(ip.tot_len));
        //  printf("TEST: tot_len: %d\n", ntohs(pkt_info.iph->tot_len));
    }
}

