#include "UserInfoForm.h"
#include "ui_UserInfoForm.h"
#include "OfflineMap/MapServices.h"
#include <google/protobuf/stubs/common.h>

UserInfoForm::UserInfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserInfoForm)
{
    ui->setupUi(this);
    svc = new MapServices();
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
    QMetaObject::invokeMethod(this, "locationResolved");
}

void UserInfoForm::locationResolved()
{
    ui->label_location->setText(this->streetName);
}
