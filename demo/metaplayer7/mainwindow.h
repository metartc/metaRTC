//
// Copyright (c) 2019-2023 yanggaofeng
//
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QVBoxLayout>
#include <QMainWindow>
#include <string>

#include "video/yangrecordthread.h"
#include "yangplayer/YangPlayerHandle.h"
#include "yangstream/YangStreamType.h"
#include "yangplayer/YangPlayWidget.h"
#include "yangplayer/YangYuvPlayWidget.h"
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangSysMessageI.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow,public YangSysMessageI
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    YangRecordThread *m_videoThread;
    YangPlayerHandle *m_player;
#if Yang_OS_APPLE
    YangYuvPlayWidget *m_videoWin;
#else
    YangPlayWidget *m_videoWin;
#endif

    YangStreamConfig m_conf;
    void initVideoThread(YangRecordThread *prt);

    void success();
    void failure(int32_t errcode);

    signals:
       void RtcConnectFailure(int errcode);
protected:
    YangContext* m_context;

private slots:
    void connectFailure(int errcode);

void on_m_b_play_clicked();

void on_m_c_whep_clicked();

private:
    Ui::MainWindow *ui;

    QWidget *m_centerWdiget;
    QVBoxLayout *m_vb;
    QHBoxLayout *m_hb1;
    QHBoxLayout *m_hb2;
private:
    bool m_isStartplay;

};
#endif // MAINWINDOW_H
