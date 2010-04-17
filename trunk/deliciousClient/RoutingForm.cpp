#include "RoutingForm.h"
#include "ui_RoutingForm.h"

RoutingForm::RoutingForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoutingForm)
{
    ui->setupUi(this);
}

RoutingForm::~RoutingForm()
{
    delete ui;
}

void RoutingForm::changeEvent(QEvent *e)
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

void RoutingForm::on_pushButton_switch_clicked()
{
    QString tmp = ui->lineEdit_from->text();
    ui->lineEdit_from->setText(ui->lineEdit_to->text());
    ui->lineEdit_to->setText(tmp);
}

void RoutingForm::setLocalPositionIfEmpty(QLineEdit *w)
{
    if (w->text().isEmpty())
    {
        w->setForegroundRole( QPalette::Light );
        w->setText(tr("[My current location]"));
    }
    else
    {
        w->setForegroundRole( QPalette::Text );
        //w->setText("");
    }
}

void RoutingForm::on_pushButton_confirm_clicked()
{
    emit doRoutingRequest(ui->lineEdit_from->text(), ui->lineEdit_to->text());
}
