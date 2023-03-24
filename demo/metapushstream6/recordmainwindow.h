//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>


#include <video/yangrecordthread.h>
#include <video/yangrecordvideowin.h>
#include <video/YangPlayWidget.h>
#include <yangutil/yangavinfotype.h>
//#include <yangpush/YangPushHandle.h>
#include <yangutil/sys/YangSysMessageI.h>
//#include <yangutil/sys/YangWindowsMouse.h>
#include <yangutil/sys/YangSysMessageHandle.h>
#include <yangpush/YangPushFactory.h>

QT_BEGIN_NAMESPACE
namespace Ui { class RecordMainWindow; }
QT_END_NAMESPACE
#define Yang_SendVideo_ 0
class RecordMainWindow : public QMainWindow,public YangSysMessageI,public YangSysMessageHandleI
{
    Q_OBJECT

public:
    RecordMainWindow(QWidget *parent = nullptr);
    ~RecordMainWindow();

   // YangPushHandle *m_rec;
    YangContext *m_context;
    YangRecordThread* m_rt;
    YangPlayWidget *m_win0;
    QHBoxLayout *m_hb0;
    int m_videoType;
     bool m_hasAudio;

    YangVideoInfo m_screenInfo;
    YangVideoInfo m_outInfo;

    YangSysMessageHandle *m_message;


public:
    void initPreview();
    void success();
    void failure(int32_t errcode);
    void receiveSysMessage(YangSysMessage *psm, int32_t phandleRet);
    void initVideoThread(YangRecordThread *prt);
    void closeEvent( QCloseEvent * event );


private slots:
    void on_m_b_rec_clicked();
private:
    void init();
    void initRecord();
    bool m_initRecord;
    Ui::RecordMainWindow *ui;

    void closeAll();

private:
    int m_screenWidth;
    int m_screenHeight;
    int32_t m_isStartpush;
    bool m_isStartRecord;
    int32_t m_isVr;
    bool m_isDrawmouse;
    string m_url;
    int32_t m_screenInternal;
    YangPushFactory m_pushfactory;

};
#endif // MAINWINDOW_H
