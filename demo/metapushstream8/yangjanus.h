#ifndef YANGJANUS_H
#define YANGJANUS_H

#include <QDialog>

namespace Ui {
class YangJanus;
}

class YangJanus : public QDialog
{
    Q_OBJECT

public:
    explicit YangJanus(QWidget *parent = nullptr);
    ~YangJanus();

private slots:
    void on_m_b_rec_clicked();

    void on_m_b_rec_2_clicked();

private:
    Ui::YangJanus *ui;
};

#endif // YANGJANUS_H
