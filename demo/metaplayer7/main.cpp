//
// Copyright (c) 2019-2022 yanggaofeng
//
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
#if defined (__APPLE__)
    QSurfaceFormat format;
    format.setVersion(4,1);
    format.setProfile(QSurfaceFormat::CoreProfile);
   QSurfaceFormat::setDefaultFormat(format);
#endif
    QApplication a(argc, argv);
    MainWindow w;
    YangRecordThread videoThread;

    w.m_videoThread=&videoThread;
    w.initVideoThread(&videoThread);
    w.show();
    videoThread.start();
    return a.exec();
}
