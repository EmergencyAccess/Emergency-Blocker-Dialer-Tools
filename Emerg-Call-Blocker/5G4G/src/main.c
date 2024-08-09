#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <linux/ip.h>
//#include <linux/tcp.h>
#include <netinet/tcp.h>
#include <linux/netfilter.h>        /* for NF_ACCEPT */
#include <errno.h>
#include <time.h>
//#include <linux/ipv6.h>
#include <netinet/ip6.h>
#include <string.h>
#include "block_ipv4.h"
#include <libnetfilter_queue/libnetfilter_queue.h>

#ifdef __LITTLE_ENDIAN
#define IPQUAD(addr) \
    ((unsigned char *)&addr)[0], \
    ((unsigned char *)&addr)[1], \
    ((unsigned char *)&addr)[2], \
    ((unsigned char *)&addr)[3]
#else
#define IPQUAD(addr) \
    ((unsigned char *)&addr)[3], \
    ((unsigned char *)&addr)[2], \
    ((unsigned char *)&addr)[1], \
    ((unsigned char *)&addr)[0]
#endif


int check_packet_len = 0;
int check_packet_proto = 0;
int packet_length = 0;
int packet_proto_indicator = 0;

int block_sip_invite = 0;
//parameter to indicate that a previous sip invite was found, which means that the next one shall be the rest of invite message 
bool sip_invite_found = false;
//we currently hard-code the verizon server port
int verizon_server_port = 5060;
int target_port = 5060;
int sip_esp_port = 0;
int ip_version = 0;

char inviteTtext[7] = "INVITE";

char dst[40];
char src[40];
int tcp=0,udp=0,esp=0,others=0,total=0; 

struct SIP{
    char *from;
    char *to;
    char *callId;
    char *via;
};

struct SIP sip;
char *from_string = "From:";
char *to_string = "To:";
char *callId_string = "Call-ID:";  
char *via_string = "Via:";

void ipv6_to_str_unexpanded(char *str, struct in6_addr * addr) {
   sprintf(str, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
		(int)addr->s6_addr[0], (int)addr->s6_addr[1],
		(int)addr->s6_addr[2], (int)addr->s6_addr[3],
		(int)addr->s6_addr[4], (int)addr->s6_addr[5],
		(int)addr->s6_addr[6], (int)addr->s6_addr[7],
		(int)addr->s6_addr[8], (int)addr->s6_addr[9],
		(int)addr->s6_addr[10], (int)addr->s6_addr[11],
		(int)addr->s6_addr[12], (int)addr->s6_addr[13],
		(int)addr->s6_addr[14], (int)addr->s6_addr[15]);
}


static u_int32_t print_pkt (struct nfq_data *tb)
{
	int id = 0;
	struct nfqnl_msg_packet_hdr *ph;
	struct nfqnl_msg_packet_hw *hwph;
	u_int32_t mark,ifi; 
	int ret;
	unsigned char *data;
 
	ph = nfq_get_msg_packet_hdr(tb);
	if (ph) {
		id = ntohl(ph->packet_id);
		printf("hw_protocol=0x%04x hook=%u id=%u ",ntohs(ph->hw_protocol), ph->hook, id);
	}
	 
	hwph = nfq_get_packet_hw(tb);
	if (hwph) {
		int i, hlen = ntohs(hwph->hw_addrlen); 
		printf("hw_src_addr=");
		for (i = 0; i < hlen-1; i++)
			printf("%02x:", hwph->hw_addr[i]);
		printf("%02x ", hwph->hw_addr[hlen-1]);
	}
 
	mark = nfq_get_nfmark(tb);
	if (mark)
		printf("mark=%u ", mark); 
 
	ifi = nfq_get_indev(tb);
	if (ifi)
		printf("indev=%u ", ifi); 
 
	ifi = nfq_get_outdev(tb);
	if (ifi)
		printf("outdev=%u ", ifi);
		
	ifi = nfq_get_physindev(tb);
	if (ifi)
		printf("physindev=%u ", ifi); 
 
	ifi = nfq_get_physoutdev(tb);
	if (ifi)
		printf("physoutdev=%u ", ifi);
 
	ret = nfq_get_payload(tb, &data);
	if (ret >= 0)
		printf("payload_len=%d ", ret); 
    
    struct ip6_hdr *iph = (struct ip6_hdr *) data;

    switch(iph->ip6_nxt){
        case 6:  //TCP Protocol
            printf("Protocol: A TCP\n");
			//++tcp;
			//print_tcp_packet(buffer , size);
			break;
		
		case 17: //UDP Protocol
            printf("Protocol: A UDP\n");
			//++udp;
			//print_udp_packet(buffer , size);
			break;

		case 50: //ESP Protocol
            printf("Protocol: A ESP\n");
			//++esp;
			//print6_esp_header(buffer, size);
			break;
    }

	fputc('\n', stdout); 
	return id;
}

int *allocate_intmem (int len) {

  void *tmp;

  tmp = (int *) malloc (len * sizeof (int));
  if (tmp != NULL) {
    memset (tmp, 0, len * sizeof (int));
    return (tmp);
  } else {
    fprintf (stderr, "ERROR: Cannot allocate memory for array allocate_intmem().\n");
    exit (EXIT_FAILURE);
  }
}

uint16_t tcp6_checksum (struct ip6_hdr iphdr, struct tcphdr tcphdr, char *data) {

  uint32_t lvalue;
  char buf[65535], cvalue;
  char *ptr;
  int chksumlen = 0;
  int payloadLen = strlen(data);

  ptr = &buf[0];  // ptr points to beginning of buffer buf

  // Copy source IP address into buf (128 bits)
  memcpy (ptr, &iphdr.ip6_src, sizeof (iphdr.ip6_src));
  ptr += sizeof (iphdr.ip6_src);
  chksumlen += sizeof (iphdr.ip6_src);

  // Copy destination IP address into buf (128 bits)
  memcpy (ptr, &iphdr.ip6_dst, sizeof (iphdr.ip6_dst));
  ptr += sizeof (iphdr.ip6_dst);
  chksumlen += sizeof (iphdr.ip6_dst);

  // Copy TCP length(TCP header + payload) to buf (32 bits)
  lvalue = htonl (sizeof (tcphdr) + payloadLen);
  memcpy (ptr, &lvalue, sizeof (lvalue));
  ptr += sizeof (lvalue);
  chksumlen += sizeof (lvalue);

  // Copy zero field to buf (24 bits)
  *ptr = 0; ptr++;
  *ptr = 0; ptr++;
  *ptr = 0; ptr++;
  chksumlen += 3;

  // Copy next header field to buf (8 bits)
  memcpy (ptr, &iphdr.ip6_nxt, sizeof (iphdr.ip6_nxt));
  ptr += sizeof (iphdr.ip6_nxt);
  chksumlen += sizeof (iphdr.ip6_nxt);

  // Copy TCP source port to buf (16 bits)
  memcpy (ptr, &tcphdr.th_sport, sizeof (tcphdr.th_sport));
  ptr += sizeof (tcphdr.th_sport);
  chksumlen += sizeof (tcphdr.th_sport);

  // Copy TCP destination port to buf (16 bits)
  memcpy (ptr, &tcphdr.th_dport, sizeof (tcphdr.th_dport));
  ptr += sizeof (tcphdr.th_dport);
  chksumlen += sizeof (tcphdr.th_dport);

  // Copy sequence number to buf (32 bits)
  memcpy (ptr, &tcphdr.th_seq, sizeof (tcphdr.th_seq));
  ptr += sizeof (tcphdr.th_seq);
  chksumlen += sizeof (tcphdr.th_seq);

  // Copy acknowledgement number to buf (32 bits)
  memcpy (ptr, &tcphdr.th_ack, sizeof (tcphdr.th_ack));
  ptr += sizeof (tcphdr.th_ack);
  chksumlen += sizeof (tcphdr.th_ack);

  // Copy data offset to buf (4 bits) and
  // copy reserved bits to buf (4 bits)
  cvalue = (tcphdr.th_off << 4) + tcphdr.th_x2;
  memcpy (ptr, &cvalue, sizeof (cvalue));
  ptr += sizeof (cvalue);
  chksumlen += sizeof (cvalue);

  // Copy TCP flags to buf (8 bits)
  memcpy (ptr, &tcphdr.th_flags, sizeof (tcphdr.th_flags));
  ptr += sizeof (tcphdr.th_flags);
  chksumlen += sizeof (tcphdr.th_flags);

  // Copy TCP window size to buf (16 bits)
  memcpy (ptr, &tcphdr.th_win, sizeof (tcphdr.th_win));
  ptr += sizeof (tcphdr.th_win);
  chksumlen += sizeof (tcphdr.th_win);

  // Copy TCP checksum to buf (16 bits)
  // Zero, since we don't know it yet
  *ptr = 0; ptr++;
  *ptr = 0; ptr++;
  chksumlen += 2;

  // Copy urgent pointer to buf (16 bits)
  memcpy (ptr, &tcphdr.th_urp, sizeof (tcphdr.th_urp));
  ptr += sizeof (tcphdr.th_urp);
  chksumlen += sizeof (tcphdr.th_urp);

  // Copy TCP payload to buf
  memcpy(ptr, data, payloadLen);
  ptr += payloadLen;
  chksumlen += payloadLen;

  return checksum ((uint16_t *) buf, chksumlen);
}


void readSipInvite(){
    printf("read sip invite!!\n");
    
    FILE *fp;

    fp  = fopen("inviteDocument", "rb"); // read only 
    if (fp == NULL){
        printf("No file: inviteDocument\n");
        exit(1);
    }
    char buffer[256];
    while (fgets(buffer, 256, fp))
    {
        if(strlen(buffer) >= 5 && strncmp(buffer, from_string, 5) == 0){
            sip.from = (char*)malloc(sizeof(buffer));
            strcpy(sip.from, buffer);
            printf("sip from: %s\n", sip.from);
        }else if(strlen(buffer) >= 3 && strncmp(buffer, to_string, 3) == 0){
            sip.to = (char*)malloc(sizeof(buffer));
            strcpy(sip.to, buffer);
            printf("sip to: %s\n", sip.to);
        }else if(strlen(buffer) >= 8 && strncmp(buffer, callId_string, 8) == 0){
            sip.callId = (char*)malloc(sizeof(buffer));
            strcpy(sip.callId, buffer);
            printf("sip callId: %s\n", sip.callId);
        }else if(strlen(buffer) >= 4 && strncmp(buffer, via_string, 4) == 0){
            sip.via = (char*)malloc(sizeof(buffer));
            strcpy(sip.via, buffer);
            printf("sip via: %s\n", sip.via);
        }
        memset(buffer, 0, strlen(buffer));
    }
    fclose(fp);
}

char **A2P(char *content[], int count){
    char **t_content = NULL;
    t_content = (char*)malloc(sizeof(char*)*count);
    for(int i=0; i<count; i++){
        t_content[i] = (char*)malloc(strlen(content[i])+2);
        strcpy(t_content[i], content[i]);
    }
    return t_content;
}

char *generateSipTrying(){

    char *t_content[] = {
        "SIP/2.0 100 Trying\r\n",
        sip.from,
        sip.to,
        sip.callId,
        "CSeq: 1 INVITE\r\n",
        sip.via,
        "Content-Length: 0\r\n\r\n"
	};
    int payload_len = sizeof(t_content)/ sizeof(t_content[0]);
    char **t_payload = A2P(t_content, payload_len);
    char payload[4096] = "";
    for(int i=0; i<payload_len; i++){
        strcat(payload, t_payload[i]);
    }
    payload[payload_len] = '\0';
    return payload;
}

static void sendACK(char *srcIP6, char *dstIP6, u_int16_t source_port, u_int16_t dest_port, u_int32_t seq, u_int32_t ack, int packet_len){
    printf("Now sending ONE TCP ACK...\n");
    int s = socket (AF_INET6, SOCK_RAW, IPPROTO_RAW);
    if(s == -1)
    {
        //socket creation failed, may be because of non-root privileges
        perror("Failed to create raw socket");
        exit(1);
    }

    //Datagram to represent the packet
    char datagram[4096];

    //zero out the packet buffer
    memset (datagram, 0, 4096);

    //IP header
    struct ip6_hdr ip;

    //TCP header
    struct tcphdr tcph;

    struct sockaddr_in6 sin;
    sin.sin6_family = AF_INET6;
    sin.sin6_port = htons(0);
    inet_pton(AF_INET6, srcIP6, &(sin.sin6_addr));

    ip.ip6_flow = htonl ((6 << 28) | (0 << 20));
    
    ip.ip6_plen = htons(sizeof (struct tcphdr)); 

    ip.ip6_hops = 60;
    
    ip.ip6_nxt = IPPROTO_TCP;

    //reverse the dstIP and srcIP
    if (inet_pton (AF_INET6, dstIP6, &(ip.ip6_src)) != 1){
        fprintf (stderr, "inet_pton() failed for source address.\n");
        exit (EXIT_FAILURE);
    }
    if (inet_pton (AF_INET6, srcIP6, &(ip.ip6_dst)) != 1) {
        fprintf (stderr, "inet_pton() failed for destination address.\n");
        exit (EXIT_FAILURE);
    }


    tcph.th_sport = htons(dest_port);
    tcph.th_dport = htons(source_port);
    tcph.th_seq = htonl (ack);
    tcph.th_ack = htonl (seq + packet_len);
    tcph.th_x2 = 0;

    // Data offset (4 bits): size of TCP header in 32-bit words
    tcph.th_off = 20 / 4;

    int *tcp_flags;
    tcp_flags = allocate_intmem(8);

    // FIN flag (1 bit)
    tcp_flags[0] = 0;

    // SYN flag (1 bit): set to 1
    tcp_flags[1] = 0;

    // RST flag (1 bit)
    tcp_flags[2] = 0;

    // PSH flag (1 bit)
    tcp_flags[3] = 0;

    // ACK flag (1 bit)
    tcp_flags[4] = 1;

    // URG flag (1 bit)
    tcp_flags[5] = 0;

    // ECE flag (1 bit)
    tcp_flags[6] = 0;

    // CWR flag (1 bit)
    tcp_flags[7] = 0;

    tcph.th_flags = 0;
    for (int i=0; i<8; i++) {
        tcph.th_flags += (tcp_flags[i] << i);
    }

    // Window size (16 bits)
    tcph.th_win = htons (65535);

    // Urgent pointer (16 bits): 0 (only valid if URG flag is set)
    tcph.th_urp = htons (0);

    // printf("Setting checksum...\n");

    tcph.th_sum = tcp6_checksum (ip, tcph, "");

    memcpy(datagram, &ip , sizeof(struct ip6_hdr));
    memcpy(datagram + sizeof(struct ip6_hdr), &tcph , sizeof(struct tcphdr));

    printf("Now sending TCP ACK \n");

    if (sendto (s, datagram, sizeof(struct ip6_hdr) + sizeof(struct tcphdr) + strlen("") , 0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
    {
        perror("sendto failed");
    }else
    {
        printf("ACK sent!\n");
    }
}

static void sendSIPTrying(char *srcIP6, char *dstIP6, u_int16_t source_port, u_int16_t dest_port, u_int32_t seq, u_int32_t ack, int packet_len){
    char *sip_trying_data;
    sip_trying_data = generateSipTrying();
    printf("100 Trying:\n%s\n", sip_trying_data);

    int s = socket (AF_INET6, SOCK_RAW, IPPROTO_RAW);
    if(s == -1)
    {
        //socket creation failed, may be because of non-root privileges
        perror("Failed to create raw socket");
        exit(1);
    }

    //Datagram to represent the packet
    char datagram[4096];

    //zero out the packet buffer
    memset (datagram, 0, 4096);

    //IP header
    struct ip6_hdr ip;

    //TCP header
    struct tcphdr tcph;

    memcpy(datagram + sizeof(struct ip6_hdr) + sizeof(struct tcphdr) , sip_trying_data , strlen(sip_trying_data));

    struct sockaddr_in6 sin;
    sin.sin6_family = AF_INET6;
    sin.sin6_port = htons(0);
    inet_pton(AF_INET6, srcIP6, &(sin.sin6_addr));
    ip.ip6_flow = htonl ((6 << 28) | (0 << 20));    
    ip.ip6_plen = htons(sizeof (struct tcphdr) + strlen(sip_trying_data)); 
    ip.ip6_hops = 60;
    ip.ip6_nxt = IPPROTO_TCP;

    //reverse the dstIP and srcIP
    if (inet_pton (AF_INET6, dstIP6, &(ip.ip6_src)) != 1){
        fprintf (stderr, "inet_pton() failed for source address.\n");
        exit (EXIT_FAILURE);
    }
    if (inet_pton (AF_INET6, srcIP6, &(ip.ip6_dst)) != 1) {
        fprintf (stderr, "inet_pton() failed for destination address.\n");
        exit (EXIT_FAILURE);
    }

    tcph.th_sport = htons(dest_port);
    tcph.th_dport = htons(source_port);
    tcph.th_seq = htonl (ack);
    tcph.th_ack = htonl (seq + packet_len);
    
    // Reserved (4 bits): should be 0
    tcph.th_x2 = 0;

    // Data offset (4 bits): size of TCP header in 32-bit words
    tcph.th_off = 20 / 4;

    int *tcp_flags;
    tcp_flags = allocate_intmem(8);

    // FIN flag (1 bit)
    tcp_flags[0] = 0;

    // SYN flag (1 bit): set to 1
    tcp_flags[1] = 0;

    // RST flag (1 bit)
    tcp_flags[2] = 0;

    // PSH flag (1 bit)
    tcp_flags[3] = 1;

    // ACK flag (1 bit)
    tcp_flags[4] = 1;

    // URG flag (1 bit)
    tcp_flags[5] = 0;

    // ECE flag (1 bit)
    tcp_flags[6] = 0;

    // CWR flag (1 bit)
    tcp_flags[7] = 0;

    tcph.th_flags = 0;
    for (int i=0; i<8; i++) {
        tcph.th_flags += (tcp_flags[i] << i);
    }

     // Window size (16 bits)
    tcph.th_win = htons (65535);

    // Urgent pointer (16 bits): 0 (only valid if URG flag is set)
    tcph.th_urp = htons (0);

    // printf("Setting checksum...\n");

    tcph.th_sum = tcp6_checksum (ip, tcph, sip_trying_data);

    memcpy(datagram, &ip , sizeof(struct ip6_hdr));
    memcpy(datagram + sizeof(struct ip6_hdr), &tcph , sizeof(struct tcphdr));

    printf("Now sending 100 Trying... \n");

    if (sendto (s, datagram, sizeof(struct ip6_hdr) + sizeof(struct tcphdr) + strlen(sip_trying_data) , 0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
    {
        perror("sendto failed");
    }else
    {
        // printf ("Packet Send. Length : %d \n" , iph->tot_len);
        // sendto (s, datagram, sizeof(struct ip6_hdr) + sizeof(struct tcphdr) + strlen(t_data) , 0, (struct sockaddr *) &sin, sizeof (sin));
        printf ("100 Trying Sent. Length: %u \n" , ntohl(ip.ip6_plen));
    }
}


void* print_payload(unsigned char* payload, int len){
    printf("TCP Header->Payload lenth: %d, Payload: \n", len);
    printf("-------START PRINTING FILTERED PACKET!--------\n");

    for(int i = 0; i< len; i++){
        printf("%c", payload[i]);
    }
    printf("\n");
    printf("-------END PRINTING FILTERED PACKET!--------\n");

}

struct struct_process_pkt_return /*the tcp pseudo header*/
{
    u_int32_t id;
    int accept;
};

// To process IPv6 packets
static struct struct_process_pkt_return process_pkt (struct nfq_data *tb)
{   
    
    struct struct_process_pkt_return pkt_return;
	int id = 0;
	struct nfqnl_msg_packet_hdr *ph;
	struct nfqnl_msg_packet_hw *hwph;
	u_int32_t mark,ifi; 
	int ret;
	unsigned char *data;
 
	ph = nfq_get_msg_packet_hdr(tb);
	if (ph) {
		id = ntohl(ph->packet_id);
	}
	hwph = nfq_get_packet_hw(tb); 
	ret = nfq_get_payload(tb, &data);
    total ++;
    pkt_return.id = id; 
    pkt_return.accept = 1;
    struct ip6_hdr *iph = (struct ip6_hdr *) data;
    struct in6_addr *dst_addr = &iph->ip6_dst;
    struct in6_addr *src_addr = &iph->ip6_src;
    u_int16_t ip6_payload_len = ntohs(iph->ip6_plen);
    // printf("IP Header->payload length: %u\n", ip6_payload_len);
    ipv6_to_str_unexpanded(dst, dst_addr);
    ipv6_to_str_unexpanded(src, src_addr);

    switch(iph->ip6_nxt){
        
        case 6:  //TCP Protocol
            tcp++;
            printf("Protocol: TCP with ");
			break;
		
		case 17: //UDP Protocol
            udp++;
            printf("Protocol: UDP with ");
			break;
        

		case 50: //ESP Protocol
            esp ++;
            printf("Protocol: ESP with ");
			break;

        default:
            others ++;
            pkt_return.accept = 1;
            break;
    }


    //check if blocking sip invite
    if (block_sip_invite != 0){

        struct tcphdr *tcph = (struct tcphdr *)(data + sizeof(struct ip6_hdr));
        u_int16_t dest_port = ntohs(tcph->th_dport);
        printf("Dest Port: %u\n", dest_port);

        //tcp header is 20-byte long
        //ip6_payload_len - 20
        unsigned char * sip_payload = (unsigned char *)(data + sizeof(struct ip6_hdr) + sizeof(struct tcphdr));

        int tcp_payload_len = ip6_payload_len - sizeof(struct tcphdr);

        // we are only looking for the SIP invite packet, which may be larger than 500 bytes
        // ignore packets smaller than 500 bytes
        // you can configure it
        if (dest_port == target_port && tcp_payload_len > 500){
            print_payload(sip_payload, tcp_payload_len);
            if (sip_invite_found == true){
                //trigger the raw socket program to send a tcp ack and sip trying
                u_int16_t source_port = ntohs(tcph->source);
                u_int32_t seq = ntohl(tcph->seq);
                u_int32_t ack = ntohl(tcph->ack_seq);

                readSipInvite();
                sendACK(src, dst, source_port, dest_port, seq, ack, tcp_payload_len);
                sendSIPTrying(src, dst, source_port, dest_port, seq, ack, tcp_payload_len);
                pkt_return.accept = 0;
                sip_invite_found = false;
            }else{
                sip_invite_found = true;
                printf("SIP INVITE Found!!\n");

                //save SIP invite as char to a file
                FILE *file = fopen("inviteDocument", "wb");
                fwrite(sip_payload , 1 , tcp_payload_len , file);
                fclose(file);

                //drop the packet
                pkt_return.accept = 0;
            }
        }
    }
    else{
        if ((packet_proto_indicator == iph->ip6_nxt || packet_proto_indicator == 0) && ret > packet_length){
            pkt_return.accept = 0;
        }else{
            pkt_return.accept = 1;
        }
    }
    if(pkt_return.accept == 1){
        printf("Accept\n");
    }
    else{
        printf("Drop\n");
        printf("Source IP       : %s\n", src);
        printf("Destination IP  : %s\n", dst);
    }
    return pkt_return;
}
void print_time(){
    time_t currentTime;
    time(&currentTime);

    // Convert the current time to a string
    char* timeString = ctime(&currentTime);
    printf("Current timestamp: %s", timeString);
}
static struct struct_process_pkt_return ipv4_process_pkt (struct nfq_data *tb)
{   
    struct struct_process_pkt_return pkt_return;
	int id = 0;
	struct nfqnl_msg_packet_hdr *ph;
	struct nfqnl_msg_packet_hw *hwph;
	u_int32_t mark,ifi; 
	int ret;
	unsigned char *data;
 
	ph = nfq_get_msg_packet_hdr(tb);
	if (ph) {
		id = ntohl(ph->packet_id);
	}

	hwph = nfq_get_packet_hw(tb);
	ret = nfq_get_payload(tb, &data);
    // Pre-defined data structure for IPv4 packets
    struct blockIP4 pkt_info;
    total ++;
    pkt_return.id = id; 
    pkt_return.accept = 1;

    //Store information of the selected packet
    pkt_info.iph = ((struct iphdr *) data);
    pkt_info.dst_addr = &pkt_info.iph->daddr;
    pkt_info.src_addr = &pkt_info.iph->saddr;    
    pkt_info.payload_len = ntohs(pkt_info.iph->tot_len) - pkt_info.iph->ihl*4;
    ipv4_to_str(pkt_info.str_src, pkt_info.src_addr);
    ipv4_to_str(pkt_info.str_dst, pkt_info.dst_addr);

    switch(pkt_info.iph->protocol){
        
        case 6:  //TCP Protocol
            tcp++;
            printf("Protocol: TCP with ");
			break;
		
		case 17: //UDP Protocol
            udp++;
            printf("Protocol: UDP with ");
			break;

		case 50: //ESP Protocol
            esp ++;
            printf("Protocol: ESP with ");
            break;

        default:
            others ++;
            pkt_return.accept = 1;
            break;
    }

    // -b option
    // Look for SIP INVITE
    if (block_sip_invite != 0){

        struct tcphdr *tcph = (struct tcphdr *)(data + sizeof(struct iphdr));
        
        pkt_info.dst_port = ntohs(tcph->th_dport);
        printf("Dest Port: %u\n", pkt_info.dst_port);

        unsigned char * sip_payload = (unsigned char *)(data + sizeof(struct iphdr) + sizeof(struct tcphdr));

        int tcp_payload_len = pkt_info.payload_len - sizeof(struct tcphdr);

        //we are only looking for SIP INVITE
        if (pkt_info.dst_port == target_port && tcp_payload_len > 500){
            print_payload(sip_payload, tcp_payload_len);
            if (sip_invite_found == true){
                //Use raw socket to send a TCP ack and 100 Trying to the device

                // Obtain header information
                pkt_info.src_port = ntohs(tcph->source);
                u_int32_t seq = ntohl(tcph->seq);
                u_int32_t ack = ntohl(tcph->ack_seq);

                // Read in template SIP INVITE
                readSipInvite();

                // Send ACK, direction: ENB to UE
                ipv4_sendACK(pkt_info.str_dst, pkt_info.str_src, pkt_info.dst_port, pkt_info.src_port, seq, ack, tcp_payload_len);
                
                // Generate a fake 100 Trying
                char *sip_trying_data = generateSipTrying();
                size_t payload_len = strlen(sip_trying_data);
                char* null_terminated_payload = (char*)malloc(payload_len + 1);
                strncpy(null_terminated_payload, sip_trying_data, payload_len);
                null_terminated_payload[payload_len] = '\0';
                
                // Send 100 Trying, direction: ENB to UE
                ipv4_send100Trying(pkt_info.str_dst, pkt_info.str_src, pkt_info.dst_port, pkt_info.src_port, seq, ack, tcp_payload_len, null_terminated_payload);
                print_time();

                // Drop the packet
                pkt_return.accept = 0;

                // Reset flag
                sip_invite_found = false;
            }else{
                sip_invite_found = true;
                printf("SIP INVITE Found!!\n");
                print_time();
               
                //Save SIP INVITE template for future use
                FILE *file = fopen("inviteDocument", "wb");
                fwrite(sip_payload , 1 , tcp_payload_len , file);
                fclose(file);

                //Drop the packet
                pkt_return.accept = 0;
            }
        }
    }
    else{
        if (!block_sip_invite && (packet_proto_indicator == pkt_info.iph->protocol || packet_proto_indicator == 0) && ret > packet_length){
            pkt_return.accept = 0;
        }else{
            pkt_return.accept = 1;
        }
    }
    // Print out result
    if(pkt_return.accept == 1){
        printf("Accept\n");
    }
    else{
        printf("Drop\n");
        printf("Source IP       : %-15s ,port: %5u\n", pkt_info.str_src, pkt_info.src_port);
        printf("Destination IP  : %-15s ,port: %5u\n", pkt_info.str_dst, pkt_info.dst_port);
    }
    return pkt_return;
}


static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data)
{
    printf("Start processing a packet!\n");
    struct struct_process_pkt_return struct_pkt_return;
    if(ip_version == 4){
        struct_pkt_return = ipv4_process_pkt(nfa);
    }
    else{
        struct_pkt_return = process_pkt(nfa);
    }
    printf("End processing a packet!\n\n");

    if (struct_pkt_return.accept == 1){
        return nfq_set_verdict(qh, struct_pkt_return.id, NF_ACCEPT, 0, NULL); 
    }else{
        // printf("DROP!!\n");
        return nfq_set_verdict(qh, struct_pkt_return.id, NF_DROP, 0, NULL); 
    }
    
}

static void printHelp(){
    printf("Please use the following commands\n");
    printf("Get help: ./block -h\n");
    printf("Block IPV6 packet by protocol: ./block -P [Protocol]\n");
    printf("     Protocl List: TCP, UDP, ESP\n");
    printf("Block IPV6 packet by packet length: ./block -l [Length]\n");
    printf("E.g., Block IPV6 packet by TCP with packet with a size over 900 (start from IP layer):\n");
    printf("    ./block -p TCP -l 900\n");
    printf("Block SIP INVITE over IPv4/v6 for TCP, UDP, and ESP\n");
    printf("E.g., Block IPv4 SIP INVITE for TCP, UDP, and ESP on port 5060:\n");
    printf("E.g., ./block -v 4 -b -p 5060\n");

}


int main(int argc, char **argv)
{   
    char packet_proto[3];
    int c;
    while ((c = getopt (argc, argv, "hP:l:bv:p:")) != -1){
        switch (c)
        {
        case 'h':
            printHelp();
            return 0;
        case 'P':
            check_packet_proto = 1;
            strcpy(packet_proto, optarg);
            if (strcmp(packet_proto, "TCP") == 0){
                packet_proto_indicator = 6;
            }else if(strcmp(packet_proto, "UDP") == 0){
                packet_proto_indicator = 17;
            }else if(strcmp(packet_proto, "ESP") == 0){
                packet_proto_indicator = 50; 
            }else{
                printf("Protocol not supported!\n");
                printf("Protocl List: TCP, UDP, ESP\n");
                return 0;
            }
            break;
        case 'l':
            check_packet_len = 1;
            packet_length = atoi(optarg);
            //cvalue = optarg;
            break;
        case 'b':
            block_sip_invite = 1;
            break;
        case 'v':
            ip_version = atoi(optarg);
            // printf("-v: %d\n", ip_version);
            if(ip_version != 4 && ip_version != 6){
                printf("Error: -v\n");
                return 0;
            }
            break;
        case 'p':
            target_port = atoi(optarg);
            // printf("-p: %d\n", target_port);
            break;
        case '?':
            if (optopt == 'P' || optopt == 'l'){
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            }
            else{
                fprintf (stderr,
                    "Unknown option character `\\x%x'.\n",
                    optopt);
            }
            return 1;

        }
    }

    if (argc == 1){
        printHelp();
        return 0;
    }


    struct nfq_handle *h;
    struct nfq_q_handle *qh;
    struct nfnl_handle *nh;
    int fd;
    int rv;
    char buf[4096] __attribute__ ((aligned));

    //init library
    printf("opening library handle\n");
    h = nfq_open();
    if (!h) {
        fprintf(stderr, "error during nfq_open()\n");
        exit(1);
    }

    printf("unbinding existing nf_queue handler for AF_INET (if any)\n");
    if (nfq_unbind_pf(h, AF_INET6) < 0) {
        fprintf(stderr, "error during nfq_unbind_pf()\n");
        exit(1);
    }

    printf("binding nfnetlink_queue as nf_queue handler for AF_INET\n");
    if (nfq_bind_pf(h, AF_INET6) < 0) {
        fprintf(stderr, "error during nfq_bind_pf()\n");
        exit(1);
    }

    printf("binding this socket to queue '0'\n");
    qh = nfq_create_queue(h, 0, &cb, NULL);
    if (!qh) {
        fprintf(stderr, "error during nfq_create_queue()\n");
        exit(1);
    }

    printf("setting copy_packet mode\n");
    if (nfq_set_mode(qh, NFQNL_COPY_PACKET, 0xffff) < 0) {
        fprintf(stderr, "can't set packet_copy mode\n");
        exit(1);
    }

    fd = nfq_fd(h);

    printf("entering loop...\n");
    for (;;) {
        if ((rv = recv(fd, buf, sizeof(buf), 0)) >= 0) {
            nfq_handle_packet(h, buf, rv);
            continue;
        }
        if (rv < 0 && errno == ENOBUFS) {
            printf("losing packets!\n");
            continue;
        }
        perror("recv failed");
        break;
    }

    printf("unbinding from queue 0\n");
    nfq_destroy_queue(qh);

#ifdef INSANE
    /* normally, applications SHOULD NOT issue this command, since
     * it detaches other programs/sockets from AF_INET, too ! */
    printf("unbinding from AF_INET\n");
    nfq_unbind_pf(h, AF_INET);
#endif

    printf("closing library handle\n");
    nfq_close(h);

    exit(0);
}