#include "yangjanus.h"
#include "ui_yangjanus.h"
#include <yangutil/sys/YangHttp.h>
#include <yangutil/sys/YangUrl.h>
#include <yangutil/sys/YangLog.h>
YangJanus::YangJanus(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::YangJanus)
{
    ui->setupUi(this);
}

YangJanus::~YangJanus()
{
    delete ui;
}

void YangJanus::on_m_b_rec_clicked()
{
    YangUrlData urlData;
    memset(&urlData,0,sizeof(YangUrlData));
    char* remoteSdp=(char*)yang_calloc(1,1024);

    if(yang_http_url_parse(Yang_IpFamilyType_IPV4,ui->m_url->text().toLatin1().data(),&urlData)!=Yang_Ok){
        ui->m_retInfo->setText("url format error!");
         yang_free(remoteSdp);
        return;
    }

    int32_t err=yang_http_post(yangfalse,Yang_IpFamilyType_IPV4,remoteSdp,urlData.server,
            urlData.port, (char*)"whip/create", (uint8_t*)ui->m_data->text().toLatin1().data(), yang_strlen(ui->m_data->text().toLatin1().data()));
    char* endp=yang_strstr(remoteSdp,"\r\n\r\n");
    ui->m_retInfo->setText(endp);
     yang_free(remoteSdp);
}

void YangJanus::on_m_b_rec_2_clicked()
{
    this->hide();
}
