#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    YangRecordThread videoThread;

    w.m_videoThread=&videoThread;
    w.initVideoThread(&videoThread);
    w.show();
    videoThread.start();
    return a.exec();
}
