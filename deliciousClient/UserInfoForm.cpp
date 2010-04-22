#include "UserInfoForm.h"
#include "ui_UserInfoForm.h"

UserInfoForm::UserInfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserInfoForm)
{
    ui->setupUi(this);
}

UserInfoForm::~UserInfoForm()
{
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
