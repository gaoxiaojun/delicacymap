#include "RestaurantInfoForm.h"
#include "ui_RestaurantInfoForm.h"
#include "MapProtocol.pb.h"
#include "Session.h"
#include <QMovie>
#include <QTimeLine>

static const int WidgetMargin = 3;

RestaurantInfoForm::RestaurantInfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RestaurantInfoForm)
{
    ui->setupUi(this);
    loading = NULL;
    s = NULL;
    res = NULL;
    timeline = new QTimeLine(200, this);
    connect(timeline, SIGNAL(frameChanged(int)), SLOT(frameChange(int)));
    qRegisterMetaType<ProtocolBuffer::CommentList*>("ProtocolBuffer::CommentList*");
    commentsShown = false;
    addShown = false;
    ui->label_spin->hide();
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

void RestaurantInfoForm::frameChange(int p)
{
    this->resize(this->width(), p);
}

void RestaurantInfoForm::UIAnimation_ShowComments(bool show)
{
    if (show == commentsShown)
        return;
    ui->listComment->setVisible( show );
    if (!commentsShown)
    {
        if (!addShown)
        {
            timeline->setFrameRange(this->height(), ui->listComment->geometry().bottom() + WidgetMargin);
            ui->label_spin->show();
            loading = new QMovie(":/Icons/loading.gif");
            ui->label_spin->setMovie(loading);
            ui->label_spin->setGeometry(
                    ui->listComment->geometry().center().x() - 16,
                    ui->listComment->geometry().center().y() - 16,
                    32, 32);
            loading->start();

        }
        else
        {
            ui->btnCommit->move(ui->btnCommit->x(), ui->listComment->geometry().bottom() + WidgetMargin);
            ui->txtComment->move(ui->txtComment->x(), ui->listComment->geometry().bottom() + WidgetMargin);
            timeline->setFrameRange(this->height(), ui->txtComment->geometry().bottom() + WidgetMargin);
        }
    }
    else
    {
        if (!addShown)
        {
            timeline->setFrameRange(this->height(), ui->btnAdd->geometry().bottom() + WidgetMargin);
        }
        else
        {
            ui->btnCommit->move(ui->btnCommit->x(), ui->listComment->geometry().bottom() + WidgetMargin);
            ui->txtComment->move(ui->txtComment->x(), ui->listComment->geometry().bottom() + WidgetMargin);
            timeline->setFrameRange(this->height(), ui->btnCommit->geometry().bottom() + WidgetMargin);
        }
    }
    timeline->start();
}

void RestaurantInfoForm::UIAnimation_ShowAdd(bool show)
{
    if (show == addShown)
        return;
    ui->btnCommit->setVisible( show );
    ui->txtComment->setVisible( show );
    if (!commentsShown)
    {
        if (!addShown)
        {
            ui->btnCommit->move(ui->btnCommit->x(), ui->btnAdd->geometry().bottom() + WidgetMargin);
            ui->txtComment->move(ui->txtComment->x(), ui->btnAdd->geometry().bottom() + WidgetMargin);
            timeline->setFrameRange(this->height(), ui->txtComment->geometry().bottom() + WidgetMargin);
        }
        else
        {
            timeline->setFrameRange(this->height(), ui->btnAdd->geometry().bottom() + WidgetMargin);
        }
    }
    else
    {
        if (!addShown)
        {
            ui->btnCommit->move(ui->btnCommit->x(), ui->listComment->geometry().bottom() + WidgetMargin);
            ui->txtComment->move(ui->txtComment->x(), ui->listComment->geometry().bottom() + WidgetMargin);
            timeline->setFrameRange(this->height(), ui->txtComment->geometry().bottom() + WidgetMargin);
        }
        else
        {
            timeline->setFrameRange(this->height(), ui->listComment->geometry().bottom() + WidgetMargin);
        }
    }
    timeline->start();
}

void RestaurantInfoForm::on_btnClose_clicked()
{
    this->close();
}

void RestaurantInfoForm::on_btnShow_clicked()
{
    UIAnimation_ShowComments(!commentsShown);
    commentsShown = !commentsShown;
    if (commentsShown && getSession() && res)
    {
        ProtocolBuffer::CommentList* commentlist=new ProtocolBuffer::CommentList();
        google::protobuf::Closure* commentDataArrive = google::protobuf::NewCallback(this, &RestaurantInfoForm::commentsResponse, commentlist);
        getSession()->getDataSource().GetLastestCommentsOfRestaurant(res->rid(), 20, commentlist, commentDataArrive);
    }
}

void RestaurantInfoForm::on_btnAdd_clicked()
{
    UIAnimation_ShowAdd(!addShown);
    addShown = !addShown;
}

void RestaurantInfoForm::on_btnCommit_clicked()
{

}

void RestaurantInfoForm::commentsResponse(ProtocolBuffer::CommentList *list)
{
    QMetaObject::invokeMethod(this, "handleCommentList", Q_ARG(ProtocolBuffer::CommentList*, list));
}

void RestaurantInfoForm::handleCommentList( ProtocolBuffer::CommentList* list )
{
    delete loading;
    loading = NULL;
    //ui->label_spin->hide();

    // This may crash, because we might have closed the form before the closure is ran
    // To fix this problem, we have to implement cancelation mechanism in rpc.
    ui->listComment->clear();
    for (int i=0;i<list->comments_size();++i)
    {
        const ProtocolBuffer::Comment& c = list->comments(i);
        QString item = QString("%1 : %2").arg(QString::fromUtf8(c.userinfo().nickname().c_str()), QString::fromUtf8(c.content().c_str()));
        ui->listComment->addItem(item);
    }
    delete list;
}
