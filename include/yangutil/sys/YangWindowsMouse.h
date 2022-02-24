//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_YangWindowsMouse_
#define INCLUDE_YANGUTIL_YangWindowsMouse_
#ifdef _WIN32
#include <windows.h>
#endif
#include <stdint.h>
#include <string>
using namespace std;
struct YangScreenKeyEvent{
    uint8_t key;
    int x;
    int y;
    int wheel;
    string direction;
    string event;
};

class YangWindowsMouse
{
public:
    YangWindowsMouse();
    ~YangWindowsMouse();

public:
    void    moveTo(int x, int y);
    void    relativeMove(int cx, int cy);
    void    setPos();
    void    restorePos();

    void    lockMouse();
    void    unlockMouse();

    void    leftBClick();
    void    leftbDClick();
    void    leftBDown();
    void    leftBUp();

    void    middleBClick();
    void    middleBDbClick();
    void    middleBDown();
    void    middleBUp();
    void    middleBRoll(int px,int py,int ch);


    void    rightBClick();
    void    rightBDbClick();
    void    rightBDown();
    void    rightBUp();
private:
 #ifdef _WIN32
    POINT p;
#endif
   
};
#endif
