//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "recordmainwindow.h"
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include <QApplication>
#include <yangpush/YangPushFactory.h>
#include <video/yangrecordthread.h>
#include <yangutil/sys/YangSysMessageHandle.h>


#include <QTextCodec>
int main(int argc, char *argv[])
{
#if defined (__APPLE__)
    QSurfaceFormat format;
    format.setVersion(4,1);
    format.setProfile(QSurfaceFormat::CoreProfile);
   QSurfaceFormat::setDefaultFormat(format);
#endif
    QApplication a(argc, argv);

#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");

    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
#endif


    RecordMainWindow w;
    YangPushFactory mf;

    YangSysMessageHandle *sysmessage=mf.createPushMessageHandle(w.m_hasAudio,false,w.m_videoType,&w.m_screenInfo,&w.m_outInfo,w.m_context,&w,&w);


    w.m_message=sysmessage;
    sysmessage->start();

   YangRecordThread videoThread;
    w.initVideoThread(&videoThread);
    videoThread.start();
    w.show();
    QThread::msleep(200);
    w.initPreview();

    /**
    YangScreenSocket socketServer;
    w.m_screenServer=&socketServer;

    socketServer.m_context=w.m_ini;
    socketServer.m_screenEvent=&w;
    socketServer.m_localPort=12000;
    socketServer.start();
    **/
    return a.exec();
}
