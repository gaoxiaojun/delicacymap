#include "RestaurantInfoForm.h"
#include "ui_RestaurantInfoForm.h"
#include "MapProtocol.pb.h"
#include "Session.h"
#include <QMovie>

RestaurantInfoForm::RestaurantInfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RestaurantInfoForm)
{
    ui->setupUi(this);
    loading = NULL;
    s = NULL;
    res = NULL;
}

RestaurantInfoForm::~RestaurantInfoForm()
{
    delete loading;
    delete ui;
}

void RestaurantInfoForm::setRestaurant(const ProtocolBuffer::Restaurant* info)
{
    ui->label_RestaurantName->setText(QString::fromUtf8(info->name().c_str(), info->name().size()));
    ui->label_price->setText(QString::number(info->averageexpense().amount()));
    res = info;
}

void RestaurantInfoForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void RestaurantInfoForm::on_btnClose_clicked()
{
    this->close();
}

void RestaurantInfoForm::on_btnShow_clicked()
{
    this->resize(width(), ui->listComment->geometry().bottom() + 5);
    loading = new QMovie(":/Icons/loading.gif");
    ui->label_spin->setMovie(loading);
    ui->label_spin->setGeometry(
            this->width()/2 - 16,
            (this->height() - ui->btnAdd->geometry().bottom()) / 2 - 16 + ui->btnAdd->geometry().bottom(),
            32, 32);
    loading->start();

    if (getSession() && res)
    {
        ProtocolBuffer::CommentList* commentlist=new ProtocolBuffer::CommentList();
        google::protobuf::Closure* commentDataArrive = google::protobuf::NewCallback(this, &RestaurantInfoForm::commentsResponse, commentlist);
        getSession()->getDataSource().GetLastestCommentsOfRestaurant(res->rid(), 20, commentlist, commentDataArrive);
    }
}

void RestaurantInfoForm::on_btnAdd_clicked()
{

}

void RestaurantInfoForm::commentsResponse(ProtocolBuffer::CommentList *list)
{
    delete loading;
    loading = NULL;
    ui->label_spin->hide();

    for (int i=0;i<list->comments_size();++i)
    {
        const ProtocolBuffer::Comment& c = list->comments(i);
        QString item = QString("%1 : %2").arg(QString::fromUtf8(c.userinfo().nickname().c_str()), QString::fromUtf8(c.content().c_str()));
        ui->listComment->addItem(item);
    }
    delete list;
}
