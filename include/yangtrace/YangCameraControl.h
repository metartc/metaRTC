#ifndef YangCameraControl_H
#define YangCameraControl_H
#include <yangtrace/YangTraceUdp.h>
#include <iostream>
#include <string>

using namespace std;

class YangCameraControl
{
public:
    YangCameraControl(void);
    ~YangCameraControl(void);

    void init(const char *teacherip,uint32_t  teacherport,const char *studentip,uint32_t  studentport);	//分别 初始化老师和学生定位摄像机的ip
    void Up(std::string type,const char *speed);
    void Down(string type,const char *speed);
    void Left(string type,const char *speed);
    void Right(string type,const char *speed);
    void Home(string type);		//原点
    void Tele(string type);		//近焦		长焦镜头
    void Wide(string type);		//远焦		广角镜头
    void tiltStop(string type);
    void zoomStop(string type);

    void StopAuto();
    void StartAuto();

private:

    void sendmsg(string type,char *visca,int32_t len);

    YangTraceUdp m_studentudp;					//学生手动
    YangTraceUdp m_teacherudp;					//老师手动

    int32_t m_isManual;
};

#endif
