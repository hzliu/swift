// Author: Hongzhang Liu
// Date:

#ifndef XBUFFER_H__
#define XBUFFER_H__

#include <stdint.h>
#include <cstdlib>
#include <cstring>

#include "noncopyable.h"

struct xbuffer : public noncopyable
{
    char *buf;
    int size;
    int wpos;  //write position
    int rpos;  //read position

    static const int kBufferInitSize = 4096;

    xbuffer(int s = kBufferInitSize) : size(s)
    {
        if(size == 0)
            size = kBufferInitSize;
        buf = (char *)malloc(size);
        wpos = rpos = 0;
    }

    ~xbuffer()
    {
        free(buf);
    }

    void reset()
    {
        wpos = rpos = 0;
    }

    void ensure_capacity(int c)
    {
        if(capacity() >= c)
            return;

        if(rpos != 0 && size - wpos + rpos >= c)
        {
            tidy();
            return;
        }

        if(c < size / 2)
            c = size / 2;
        buf = (char *)realloc(buf, size + c);
        size = size + c;
    }

    void consume(int len)
    {
        rpos += len;
        if(rpos >= wpos)
        {
            rpos = wpos = 0;
        }
    }

    void gotbytes(int len)
    {
        wpos += len;
    }

    //how much we can write to
    int capacity()
    {
        return size - wpos;
    }

    char *writepos()
    {
        return buf + wpos;
    }

    char *readpos()
    {
        return buf + rpos;
    }

    //how much we can read
    int len()
    {
        return wpos -rpos;
    }

    void tidy()
    {
        if(rpos == 0)
            return;

        memmove(buf, buf + rpos, wpos - rpos);
        wpos -= rpos;
        rpos = 0;
    }

    //从fd中读取数据
    //@retval 1 成功，数据在buff中，或者读到EAGAIN
    //@retval 0 对端关闭连接
    //@retval <0 出错，-errno
    int readfrom(int fd);

    //将buf中的数据写入fd
    //@retval >0 写入的字节数
    //@retval  0 写到EINTER, EAGAIN
    //@retval <0 出错
    int writeto(int fd);
};

#endif

