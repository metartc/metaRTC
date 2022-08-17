//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef RTCMESSAGE_H
#define RTCMESSAGE_H
#include <QWidget>
#include <QLabel>
class RtcMessage:public QWidget
{
public:
    RtcMessage(QWidget *parent= nullptr);
    ~RtcMessage();
    QLabel *m_lab;
};

#endif // RTCMESSAGE_H
