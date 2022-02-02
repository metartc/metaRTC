
#ifndef TRACE_YangTraceServer_H_
#define TRACE_YangTraceServer_H_

#include <iostream>
#include "YangCameraControl.h"
#include "YangTraceHandle.h"
#include "yangutil/sys/YangThread.h"
#include "YangTraceIni.h"
class YangTraceServer:public YangThread
{
public:
    YangTraceServer(YangTraceIni* pcontext);
    virtual ~YangTraceServer(void);

    void init();
    void startLoop();

    int32_t m_isStart;
    int32_t m_isConvert;
    void stop();
    void run();
private:
    YangTraceHandle *m_trace;
    YangCameraControl m_camCtl;
    YangTraceIni* m_context;

};

#endif
