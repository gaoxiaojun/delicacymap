#include "UserInfoForm.h"
#include "ui_UserInfoForm.h"
#include "OfflineMap/MapServices.h"
#include "Session.h"
#include <QCloseEvent>
#include <google/protobuf/stubs/common.h>

UserInfoForm::UserInfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserInfoForm)
{
    ui->setupUi(this);
    svc = new MapServices();
    pendingOperations = 0;
}

UserInfoForm::~UserInfoForm()
{
    delete svc;
    delete ui;
}

void UserInfoForm::changeEvent(QEvent *e)
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

void UserInfoForm::setLocation(GeoPoint p)
{
    svc->ReverseGeoCode(p.lat.getDouble(), p.lng.getDouble(), streetName, google::protobuf::NewCallback(this, &UserInfoForm::_locationResolved));
}

void UserInfoForm::_locationResolved()
{
    streetName = streetName.mid(2);
    QMetaObject::invokeMethod(this, "locationResolved");
}

void UserInfoForm::locationResolved()
{
    --pendingOperations;
    ui->label_location->setText(this->streetName);
    if (pendingOperations == 0 && !isVisible())
    {
        this->deleteLater();
    }
}

void UserInfoForm::setUID(int uid)
{
    this->uid = uid;
    if (uid == getSession()->getUser()->uid() || !getSession()->isSharingLocationWith(uid))
    {
        ui->label_location->resize(this->width(), this->height());
        ui->btnSub->hide();
    }
    else if (getSession()->isSubscribedToUser(uid))
    {
        ui->btnSub->setText(tr("UnSubscribe"));
    }
}

void UserInfoForm::on_btnSub_clicked()
{
    if (getSession()->isSubscribedToUser(uid))
    {
        getSession()->UnSubscribeFromUser(uid);
        ui->btnSub->setText(tr("Subscribe"));
    }
    else
    {
        getSession()->SubscribeToUser(uid);
        ui->btnSub->setText(tr("UnSubscribe"));
    }
}

void UserInfoForm::closeEvent( QCloseEvent *event )
{
    if (this->pendingOperations > 0)
    {
        event->ignore();
        this->hide();
    }
}
