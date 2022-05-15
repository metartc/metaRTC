//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangWindowsMouse.h>
#ifdef _WIN32

YangWindowsMouse::YangWindowsMouse()
{

}

YangWindowsMouse::~YangWindowsMouse()
{

}

void YangWindowsMouse::moveTo(int x, int y)
{
    p.x = x;
    p.y = y;
    SetCursorPos(x, y);
}


void YangWindowsMouse::relativeMove(int cx, int cy)
{
    GetCursorPos(&p);
    p.x += cx;
    p.y += cy;
    SetCursorPos(p.x, p.y);
}


void YangWindowsMouse::setPos()
{
    GetCursorPos(&p);
}


void YangWindowsMouse::restorePos()
{
    SetCursorPos(p.x, p.y);
}


void YangWindowsMouse::lockMouse()
{
    POINT    pt;
    RECT    rt;

    GetCursorPos(&pt);
    rt.left = rt.right = pt.x;
    rt.top = rt.bottom = pt.y;
    rt.right++;
    rt.bottom++;
    ClipCursor(&rt);
}


void YangWindowsMouse::unlockMouse()
{
    ClipCursor(NULL);
}


void YangWindowsMouse::leftBClick()
{
    setPos();
    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, p.x, p.y, 0, 0);
}


void YangWindowsMouse::leftbDClick()
{
    setPos();
    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, p.x, p.y, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, p.x, p.y, 0, 0);
}


void YangWindowsMouse::leftBDown()
{
    setPos();
    mouse_event(MOUSEEVENTF_LEFTDOWN, p.x, p.y, 0, 0);
}


void YangWindowsMouse::leftBUp()
{
    setPos();
    mouse_event(MOUSEEVENTF_LEFTUP, p.x, p.y, 0, 0);
}

//middle
void YangWindowsMouse::middleBClick()
{
    setPos();
    mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, p.x, p.y, 0, 0);
}


void YangWindowsMouse::middleBDbClick()
{
    setPos();
    mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, p.x, p.y, 0, 0);
    mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, p.x, p.y, 0, 0);
}


void YangWindowsMouse::middleBDown()
{
    setPos();
    mouse_event(MOUSEEVENTF_MIDDLEDOWN, p.x, p.y, 0, 0);
}


void YangWindowsMouse::middleBUp()
{
    setPos();
    mouse_event(MOUSEEVENTF_MIDDLEUP, p.x, p.y, 0, 0);
}


void YangWindowsMouse::middleBRoll(int x,int y,int ch)
{
    setPos();
    mouse_event(MOUSEEVENTF_WHEEL, x, y, ch, 0);
}

void YangWindowsMouse::rightBClick()
{
    setPos();
    mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, p.x, p.y, 0, 0);
}


void YangWindowsMouse::rightBDbClick()
{
    setPos();
    mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, p.x, p.y, 0, 0);
    mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, p.x, p.y, 0, 0);
}


void YangWindowsMouse::rightBDown()
{
    setPos();
    mouse_event(MOUSEEVENTF_RIGHTDOWN, p.x, p.y, 0, 0);
}


void YangWindowsMouse::rightBUp()
{
    setPos();
    mouse_event(MOUSEEVENTF_RIGHTUP, p.x, p.y, 0, 0);
}

#endif
