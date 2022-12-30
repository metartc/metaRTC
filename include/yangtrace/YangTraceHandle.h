//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YangTraceHandle_H
#define YangTraceHandle_H
#include <stdio.h>
#include <stdlib.h>
#include <yangtrace/YangCameraControl.h>
#include <yangtrace/YangTraceUdp.h>
#include <iostream>
#define Yang_Trace_WhiteBoardStart  "08090802"
#define Yang_Trace_Teacher  "08090801"
#define Yang_Trace_Student  "08090803"
#define	Command_Trace_Start "Auto|END"
#define Command_Trace_Stop "Manual|END"
#define Command_Mouse_Move "MouseMove|END"
#define Command_Mouse_Stop "MouseStop|END"
#define Command_Teacher_Close "08090801" //jin jing
#define Command_Whiteboard_Start "08090802"
#define Command_Student_Close "08090803" //jin jing
#define Command_Whiteboard_Stop "08090804"
#define Command_Student_Down "08090805" //zuo xia
#define Command_Teacher_Move "08090806"
#define Command_Student_Multi_Up "08090807"
#define Command_Teacher_Disappear "08090808"
#define Command_Teacher_Full "08090809"
#define Command_Teacher_Move_Full "08090810"

enum YangTraceType{
	Yang_Trace_Start,
	Yang_Trace_Stop,
	Yang_Trace_Teacher_Move,
	Yang_Trace_Teacher_Pause,
	Yang_Trace_Teacher_Full,
	Yang_Trace_Teacher_Disappear,
	Yang_Trace_Student_Single_Up,
	Yang_Trace_Student_Single_Down,
	Yang_Trace_Student_Multi_Up,
	Yang_Trace_Student_Multi_Down,
	Yang_Trace_Whiteboard_Start,
	Yang_Trace_Whiteboard_Stop
};
class YangTraceHandle
{
public:
    YangTraceHandle();
    virtual ~YangTraceHandle();
    YangCameraControl *m_camCtl;
    void init(const char *ip,const int32_t port);
    void startTrace();
    void stopTrace();
    void proc(const char* command);
    void procCommand(YangTraceType evt);
    void manualCyc(string str);
private:
    void change(const char *s);
    int32_t m_isStartTrace;
    YangTraceUdp m_udp;

    int32_t m_whiteBoStat;
    int32_t m_studentStat;

    std::string m_ip;
};

#endif // AUTOTRACEI_H
