#ifndef YangPlayReceive_H
#define YangPlayReceive_H
//#include "mytype.h"
#include <yangstream/YangStreamHandle.h>
#include <yangutil/yangavinfotype.h>
#include <string>
#include "yangutil/buffer/YangAudioEncoderBuffer.h"
#include "yangutil/buffer/YangVideoDecoderBuffer.h"
#include "yangutil/sys/YangThread.h"
using namespace std;
class YangPlayReceive: public YangThread, public YangReceiveCallback
{
    public:
        YangPlayReceive(YangContext* pcontext);
        virtual ~YangPlayReceive();
    	void receiveAudio(YangFrame* audioFrame);
    	void receiveVideo(YangFrame* videoFrame);
        int32_t init(int32_t nettype,string server,int32_t pport,string stream);
        void setBuffer(YangAudioEncoderBuffer *al,YangVideoDecoderBuffer *vl);
        void disConnect();
        void play(char* pserverStr,char *streamName);
        YangStreamHandle *m_recv;
    	int32_t isReceived; //,isHandled;
    	int32_t isReceiveConvert; //,isHandleAllInvoke;
    	int32_t m_isStart;
    	void stop();
    protected:
    	void run();
    	void startLoop();

    private:
    	int32_t m_headLen;
    	YangAudioEncoderBuffer *m_out_audioBuffer;
    	YangVideoDecoderBuffer *m_out_videoBuffer;
    	YangContext* m_context;
};

#endif // VIDEOMEETING_H
