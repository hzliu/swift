#include <swift/net/xbuffer.h>

#include <unistd.h>
#include <errno.h>
#include <assert.h>

namespace swift { namespace net
{

XBuffer::XBuffer(int size)
    : size_(size), wpos_(0), rpos_(0)
{
    assert(size_ > 0);
    buf_ = static_cast<char *>(malloc(size_));
}

XBuffer::~XBuffer()
{
    free(buf_);
}

void XBuffer::EnsureCapacity(int c)
{
    if(WriteableBytes() >= c)
        return;

    if(rpos_ != 0 && size_ - wpos_ + rpos_ >= c)
    {
        Tidy();
        return;
    }

    if(c < size_ / 2)
        c = size_ / 2;
    buf_ = static_cast<char *>(realloc(buf_, size_ + c));
    size_ += c;
}

void XBuffer::ConsumeBytes(int len)
{
    assert(len >= 0 && len <= ReadableBytes());
    rpos_ += len;
    if(rpos_ >= wpos_)  //all data is consumed, so reset it
    {
        rpos_ = wpos_ = 0;
    }
}

void XBuffer::Tidy()
{
    if(rpos_ == 0)
        return;
    memmove(buf_, buf_ + rpos_, wpos_ - rpos_);
    wpos_ -= rpos_;
    rpos_ = 0;
}

void XBuffer::Peek(void *data, int len)
{
    assert(len > 0 && len <= ReadableBytes());
    memcpy(data, ReadPos(), len);
}

void XBuffer::Retrive(void *data, int len)
{
    Peek(data, len);
    ConsumeBytes(len);
}

void XBuffer::Append(const void *data, int len)
{
    EnsureCapacity(len);
    memcpy(WritePos(), data, len);
    GotBytes(len);
}

int XBuffer::ReadFrom(int fd)
{
    Tidy();
    if(WriteableBytes() == 0)   //buf is full, enlarge it
        EnsureCapacity(size_);

    int rtn = read(fd, WritePos(), WriteableBytes());
    if(rtn < 0)
    {
        if(errno == EAGAIN || errno == EINPROGRESS || errno == EINTR)
            return 1;

        return -errno;
    }

    if(rtn == 0)
        return 0;

    GotBytes(rtn);
    return 1;
}

int XBuffer::WriteTo(int fd)
{
    if(ReadableBytes() == 0)
        return 0;

    int rtn = write(fd, ReadPos(), ReadableBytes());
    if(rtn < 0)
    {
        if(errno == EAGAIN || errno == EINPROGRESS || errno == EINTR)
            return 0;

        return -errno;
    }

    ConsumeBytes(rtn);
    return rtn;
}

}}
