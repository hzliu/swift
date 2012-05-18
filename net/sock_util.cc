#include <swift/net/sock_util.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <swift/util/safe_convert.h>

sockaddr *sockaddr_cast(sockaddr_in *addr)
{
    return reinterpret_cast<sockaddr *>(addr);
}

const sockaddr *sockaddr_cast(const sockaddr_in *addr)
{
    return reinterpret_cast<const sockaddr *>(addr);
}

sockaddr *sockaddr_cast(sockaddr_un *addr)
{
    return reinterpret_cast<sockaddr *>(addr);
}

const sockaddr *sockaddr_cast(const sockaddr_un *addr)
{
    return reinterpret_cast<const sockaddr *>(addr);
}


int string_to_sock_addr(const char *str, sockaddr_in *addr)
{
    memset(addr, 0, sizeof(sockaddr_in));
    addr->sin_family = AF_INET;

    char buf[32] = { 0 };   //ip addr can't be longer than 4*4+3
    const char *delimPos = strchr(str, ':');
    if(!delimPos)
        return -1;

    if(delimPos - str > static_cast<int>(sizeof(buf)) - 1)
        return -1;

    memcpy(buf, str, delimPos - str);
    if(inet_aton(buf, &addr->sin_addr) == 0)
        return -1;

    unsigned short port;
    if(!safe_strtous(delimPos + 1, &port))
        return -1;

    addr->sin_port = htons(port);
    return 0;
}

char *sock_addr_to_string(const sockaddr_in &addr, char buf[32])
{
    const unsigned char *pbyte =
        reinterpret_cast<const unsigned char *>(&addr.sin_addr.s_addr);
    sprintf(buf, "%d.%d.%d.%d:%d", pbyte[0], pbyte[1], pbyte[2], pbyte[3],
            ntohs(addr.sin_port));
    return buf;
}

int listen_or_die(const sockaddr_in &addr, int backlog)
{
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if(s < 0)
    {
        fprintf(stderr, "socket error: %m");
        abort();
        return -1;
    }

    //reuse addr should be set by default
    set_reuseaddr(s);

    if(bind(s, sockaddr_cast(&addr), sizeof(sockaddr_in)) < 0)
    {
        fprintf(stderr, "bind error: %m");
        abort();
        return -1;
    }

    if(listen(s, backlog) < 0)
    {
        fprintf(stderr, "listen error: %m");
        abort();
        return -1;
    }

    return s;
}

int listen_or_die(const sockaddr_un &addr, int backlog)
{
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if(s < 0)
    {
        fprintf(stderr, "socket error: %m");
        abort();
        return -1;
    }

    //reuse addr should be set by default
    set_reuseaddr(s);

    if(bind(s, sockaddr_cast(&addr), sizeof(sockaddr_un)) < 0)
    {
        fprintf(stderr, "bind error: %m");
        abort();
        return -1;
    }

    if(listen(s, backlog) < 0)
    {
        fprintf(stderr, "listen error: %m");
        abort();
        return -1;
    }

    return s;
}

