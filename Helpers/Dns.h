#ifndef _DNS_H
#define _DNS_H
typedef struct {
    unsigned short id;
    unsigned char rd : 1;
    unsigned char tc : 1;
    unsigned char aa : 1;
    unsigned char opcode : 4;
    unsigned char qr : 1;
    unsigned char rcode : 4;
    unsigned char z : 3;
    unsigned char ra : 1;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
} dns_header_t;

typedef struct {
    unsigned short type;
    unsigned short class;
} dns_question_t;

typedef struct {
    unsigned short name;
    unsigned short type;
    unsigned short class;
    unsigned int ttl;
    unsigned short rdlength;
    unsigned char rdata[4];
} dns_answer_t;

#define DNSPORT 53
#endif
