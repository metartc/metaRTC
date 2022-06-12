//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SCREENEVENT_H
#define SCREENEVENT_H
#include <string>

class YangScreenEventI{
public:
    YangScreenEventI(){};
    virtual ~YangScreenEventI(){};
    virtual void screenEvent(char* str)=0;
};

class YangScreenEvent
{
public:
    YangScreenEvent();
    virtual ~YangScreenEvent();
    void screenEvent(char* str);
public:
    int m_width,m_height;

    //CMouse m_mouse;
};

#endif // SCREENEVENT_H
