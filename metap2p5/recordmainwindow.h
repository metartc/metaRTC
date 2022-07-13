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

#include <yangutil/sys/YangSysMessageI.h>

#include <yangutil/sys/YangSysMessageHandle.h>
#include <yangp2p/YangP2pFactory.h>

QT_BEGIN_NAMESPACE
namespace Ui { class RecordMainWindow; }
QT_END_NAMESPACE
#define Yang_SendVideo_ 0
class RecordMainWindow : public QMainWindow,public YangSysMessageI,public YangSysMessageHandleI,public YangStreamStateNotify
{
    Q_OBJECT

public:
    RecordMainWindow(QWidget *parent = nullptr);
    ~RecordMainWindow();
    YangContext *m_context;
    YangRecordThread* m_render;
    YangPlayWidget *m_win0,*m_win1;
    QHBoxLayout *m_hb0;

    bool m_hasAudio;
    YangSysMessageHandle *m_message; 

public:
    void setRecvText(char* data,int32_t nb);
    void initPreview();
    void streamStateNotify(int32_t puid,YangStreamOptType opt,bool isConnect);
    void success();
    void failure(int32_t errcode);
    void receiveSysMessage(YangSysMessage *psm, int32_t phandleRet);
    void initVideoThread(YangRecordThread *prt);
     void closeEvent( QCloseEvent * event );
private slots:
     void on_m_b_pushplay_clicked();
  //  void on_m_b_play_clicked();

     void on_m_b_senddata_clicked();

private:
    void init();
    Ui::RecordMainWindow *ui;
    void closeAll();
    void read_ip_address();

private:
    bool m_isStartpushplay;
   // bool m_isStartplay;
    string m_url; 
    YangP2pFactory m_p2pfactory;

     //YangPlayerHandle *m_player;
};
#endif // MAINWINDOW_H
