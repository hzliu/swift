#include <unistd.h>
#include <errno.h>

#include "xbuffer.h"

int xbuffer::readfrom(int fd)
{
    tidy();
    if(capacity() == 0)
        ensure_capacity(size);

    int rtn = read(fd, writepos(), capacity());
    if(rtn < 0)
    {
        if(errno == EAGAIN || errno == EINPROGRESS || errno == EINTR)
            return 1;

        return -errno;
    }

    if(rtn == 0)
        return 0;

    gotbytes(rtn);
    return 1;
}

int xbuffer::writeto(int fd)
{
    int rtn = write(fd, readpos(), len());
    if(rtn < 0)
    {
        if(errno == EAGAIN || errno == EINPROGRESS || errno == EINTR)
            return 0;

        return -errno;
    }

    consume(rtn);
    return rtn;
}

