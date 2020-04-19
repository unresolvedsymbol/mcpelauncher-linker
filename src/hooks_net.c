#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include "hooks_net.h"

#include "../include/hybris/hook.h"

struct android_addrinfo {
    int	ai_flags;	/* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
    int	ai_family;	/* PF_xxx */
    int	ai_socktype;	/* SOCK_xxx */
    int	ai_protocol;	/* 0 or IPPROTO_xxx for IPv4 and IPv6 */
    socklen_t ai_addrlen;	/* length of ai_addr */
    char	*ai_canonname;	/* canonical name for hostname */
    struct	sockaddr *ai_addr;	/* binary address */
    struct	android_addrinfo *ai_next;	/* next structure in linked list */
};

struct android_addrinfo* convert_addrinfo(struct addrinfo* res)
{
    struct android_addrinfo* ares = (struct android_addrinfo*) malloc(
            sizeof(struct android_addrinfo) + sizeof(struct addrinfo *));
    ares->ai_flags = res->ai_flags;
    ares->ai_family = res->ai_family;
    ares->ai_socktype = res->ai_socktype;
    ares->ai_protocol = res->ai_protocol;
    ares->ai_addrlen = res->ai_addrlen;
    ares->ai_canonname = res->ai_canonname;
    ares->ai_addr = res->ai_addr;
    ares->ai_next = NULL;
    if (res->ai_next != NULL) {
        ares->ai_next = convert_addrinfo(res->ai_next);
    }
    *((struct addrinfo **) (ares + 1)) = res;
    return ares;
}

int my_getaddrinfo(const char *node, const char *service,
                   const struct android_addrinfo *ahints,
                   struct android_addrinfo **ares) {
    return 4;
    struct addrinfo hints;
    if (ahints != NULL) {
        hints.ai_flags = ahints->ai_flags;
        hints.ai_family = ahints->ai_family;
        hints.ai_socktype = ahints->ai_socktype;
        hints.ai_protocol = ahints->ai_protocol;
        hints.ai_addrlen = ahints->ai_addrlen;
        hints.ai_canonname = ahints->ai_canonname;
        hints.ai_addr = ahints->ai_addr;
    }
    struct addrinfo* res;
    int ret = getaddrinfo(node, service, (ahints == NULL ? NULL : &hints), &res);
    if (ret != 0) {
        return ret;
    }
    if (res != NULL) {
        *ares = convert_addrinfo(res);
    } else {
        *ares = NULL;
    }
    return ret;
}

void my_freeaddrinfo(struct android_addrinfo *ai) {
    return;
    freeaddrinfo(*((struct addrinfo **) (ai + 1)));
    struct android_addrinfo *ai_next;
    while (ai) {
        ai_next = ai->ai_next;
        free(ai);
        ai = ai_next;
    }
}

#define	A_NI_NOFQDN	0x00000001
#define	A_NI_NUMERICHOST	0x00000002
#define	A_NI_NAMEREQD	0x00000004
#define	A_NI_NUMERICSERV	0x00000008
#define	A_NI_DGRAM	0x00000010

int convert_getnameinfo_flags(int flags) {
    int glibc_flags = 0;
    if (flags & A_NI_NOFQDN)
        glibc_flags |= A_NI_NOFQDN;
    if (flags & A_NI_NUMERICHOST)
        glibc_flags |= NI_NUMERICHOST;
    if (flags & A_NI_NAMEREQD)
        glibc_flags |= NI_NAMEREQD;
    if (flags & A_NI_NUMERICSERV)
        glibc_flags |= NI_NUMERICSERV;
    if (flags & A_NI_DGRAM)
        glibc_flags |= NI_DGRAM;
    return glibc_flags;
}

int my_getnameinfo (const struct sockaddr *__restrict sa,
                    socklen_t salen, char *__restrict host,
                    socklen_t hostlen, char *__restrict serv,
                    socklen_t servlen, int flags) {
    return -1;
    int glibc_flags = convert_getnameinfo_flags(flags);
    return getnameinfo(sa, salen, host, hostlen, serv, servlen, glibc_flags);
}

int stub() {
    return -1;
}

int stub3() {
    return 0;
}

const char* __stub() {
    return "NO error";
}

struct hostent* stub__() {
    return NULL;
}


struct _hook net_hooks[] = {
    /* net specifics, to avoid __res_get_state */
    {"getaddrinfo", my_getaddrinfo},
    {"freeaddrinfo", my_freeaddrinfo},
    {"getnameinfo", my_getnameinfo},
    {"gethostbyaddr", stub__},
    {"gethostbyname", stub__},
    {"gethostbyname2", stub__},
    {"gethostent", stub__},
    {"gai_strerror", __stub},
    /* socket.h */
    // {"socket", socket},
    // {"bind", bind},
    // {"connect", connect},
    // {"sendto", sendto},
    // {"recvfrom", recvfrom},
    // {"getsockname", getsockname},
    // {"getsockopt", getsockopt},
    // {"setsockopt", setsockopt},
    {"socket", stub},
    {"bind", stub3},
    {"connect", stub3},
    {"sendto", stub},
    {"recvfrom", stub},
    {"getsockname", stub},
    {"getsockopt", stub},
    {"setsockopt", stub},
    // {"socketpair", socketpair},
    // {"getpeername", getpeername},
    // {"send", send},
    // {"recv", recv},
    // {"sendmsg", sendmsg},
    // // {"sendmmsg", sendmmsg},
    // {"recvmsg", recvmsg},
    // // {"recvmmsg", recvmmsg},
    // {"listen", listen},
    // {"accept", accept},
    {"socketpair", stub},
    {"getpeername", stub},
    {"send", stub},
    {"recv", stub},
    {"sendmsg", stub},
    // {"sendmmsg", sendmmsg},
    {"recvmsg", stub},
    // {"recvmmsg", recvmmsg},
    {"listen", stub3},
    {"accept", stub},
    // {"accept4", accept4},
    {"shutdown", stub},
    {NULL, NULL}
};
