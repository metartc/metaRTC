//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGRESIZE_H
#define YANGRESIZE_H


class PicUtilBmp
{
public:
    PicUtilBmp(void);
    virtual ~PicUtilBmp(void);

    virtual void resize();
    virtual void init(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight);
    virtual void initYuy2(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight);
    virtual void initYuy2tonv12(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight);
    virtual void initNv12(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight);
    virtual void init420P(int32_t pinWidth,int32_t pinHeight,int32_t poutWidth,int32_t poutHeight);
    virtual void getAddr(uint8_t **p_in,uint8_t **p_out);
    virtual void close();
};



class YangResize
{
public:
    YangResize();
    virtual ~YangResize();
public:

    //uint8_t *in;
    //uint8_t *out;
    PicUtilBmp *pu;


protected:

private:
};


#endif // YANGRESIZE_H
