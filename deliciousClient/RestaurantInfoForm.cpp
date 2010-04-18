#include "RestaurantInfoForm.h"
#include "ui_RestaurantInfoForm.h"
#include "MapProtocol.pb.h"
#include "Session.h"
#include "RoutingForm.h"
#include <QMovie>
#include <QTimeLine>

static const int WidgetMargin = 3;

static inline
void HideWidget(QWidget* widget, bool doHide = true)
{
    if (doHide)
        widget->move(widget->x(), 1000);
}

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
    HideWidget(ui->label_spin);
    HideWidget(ui->listComment);
    HideWidget(ui->btnCommit);
    HideWidget(ui->txtComment);
}

RestaurantInfoForm::~RestaurantInfoForm()
{
    delete loading;
    delete ui;
}

QWidget* RestaurantInfoForm::getInputWidget()
{
    return ui->txtComment;
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
    HideWidget(ui->listComment, show);
    if (!commentsShown)
    {
        ui->listComment->move(0, ui->btnShow->geometry().bottom() + WidgetMargin);
        if (!addShown)
        {
            timeline->setFrameRange(this->height(), ui->listComment->geometry().bottom() + WidgetMargin);
            loading = new QMovie(":/Icons/loading.gif");
            ui->label_spin->show();
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
        HideWidget(ui->listComment);
        if (!addShown)
        {
            timeline->setFrameRange(this->height(), ui->btnAdd->geometry().bottom() + WidgetMargin);
        }
        else
        {
            ui->btnCommit->move(ui->btnCommit->x(), ui->btnAdd->geometry().bottom() + WidgetMargin);
            ui->txtComment->move(ui->txtComment->x(), ui->btnAdd->geometry().bottom() + WidgetMargin);
            timeline->setFrameRange(this->height(), ui->btnCommit->geometry().bottom() + WidgetMargin);
        }
    }
    timeline->start();
}

void RestaurantInfoForm::UIAnimation_ShowAdd(bool show)
{
    if (show == addShown)
        return;
    HideWidget(ui->btnCommit, show);
    HideWidget(ui->txtComment, show);
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

void RestaurantInfoForm::on_btnRouting_clicked()
{
    emit RequestByRestaurant(res);
}
void RestaurantInfoForm::on_btnCommit_clicked()
{
     QString content = ui->txtComment->text();
     std::string usrname = getSession()->getUser()->nickname();
     if (!content.isEmpty())
     {
         ui->txtComment->clear();
         ProtocolBuffer::Comment *newComment=new ProtocolBuffer::Comment();

         google::protobuf::Closure* commentadded = google::protobuf::NewCallback(this, &RestaurantInfoForm::addCommentToList, const_cast<const ProtocolBuffer::Comment*>(newComment), true);
         QByteArray utf8Content = content.toUtf8();
         std::string contentStr(utf8Content.constData(), utf8Content.size());
         this->getSession()->getDataSource().AddCommentForRestaurant(
             res->rid(),
             this->getSession()->getUser()->uid(),
             contentStr,
             newComment,
             commentadded);
     }
}

void RestaurantInfoForm::commentsResponse(ProtocolBuffer::CommentList *list)
{
    QMetaObject::invokeMethod(this, "handleCommentList", Q_ARG(ProtocolBuffer::CommentList*, list));
}

void RestaurantInfoForm::addCommentToList(const ProtocolBuffer::Comment *c, bool releaseComment)
{
    QString item = QString("%1 : %2").arg(QString::fromUtf8(c->userinfo().nickname().c_str()), QString::fromUtf8(c->content().c_str()));
    ui->listComment->addItem(item);
    if (releaseComment)
        delete c;
}

void RestaurantInfoForm::handleCommentList( ProtocolBuffer::CommentList* list )
{
    delete loading;
    loading = NULL;

    // This may crash, because we might have closed the form before the closure is ran
    // To fix this problem, we have to implement cancelation mechanism in rpc.
    ui->listComment->clear();
    for (int i=0;i<list->comments_size();++i)
    {
        const ProtocolBuffer::Comment& c = list->comments(i);
        addCommentToList(&c);
    }
    delete list;
}
