#ifndef NONCOPYABLE_H__
#define NONCOPYABLE_H__

class noncopyable
{
protected:
    noncopyable(void){}
    ~noncopyable(void){} 
private:
    noncopyable(const noncopyable&);
    const noncopyable& operator= (const noncopyable&);
};

#endif
