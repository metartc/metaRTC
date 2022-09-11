//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGSWIN_H
#define YANGSWIN_H

#include <QWidget>
class YangRecordVideoWin : public QWidget
{
    Q_OBJECT
public:
    explicit YangRecordVideoWin(QWidget *parent = nullptr);
     ~YangRecordVideoWin();
public:
    QPaintEngine* paintEngine()const;
    int32_t sid;

private:

private:


//signals:

};

#endif // YANGSWIN_H
