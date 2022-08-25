//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "recordmainwindow.h"
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include <QApplication>
#include <yangp2p/YangP2pFactory.h>
#include <video/yangrecordthread.h>
#include <yangutil/sys/YangSysMessageHandle.h>

#include <QTextCodec>
int main(int argc, char *argv[])
{
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
    YangP2pFactory mf;

    YangSysMessageHandle *sysmessage=mf.createP2pMessageHandle(w.m_hasAudio,w.m_context,&w,&w,&w.m_factory);


    w.m_message=sysmessage;
    sysmessage->start();

   YangRecordThread videoThread;
    w.initVideoThread(&videoThread);
    videoThread.start();
    w.show();
    QThread::msleep(200);
    w.initPreview();


    return a.exec();
}
