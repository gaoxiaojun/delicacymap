#include "RoutingForm.h"
#include "ui_RoutingForm.h"
#include "Session.h"
#include "OfflineMap/MapServices.h"

using namespace ProtocolBuffer;
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

void RoutingForm::setFromLocation(const QString &from)
{
    ui->lineEdit_from->setText(from);
    this->on_lineEdit_from_editingFinished();
}

void RoutingForm::setFromLocation( const QString&from, const GeoPoint& p )
{
    ui->lineEdit_from->setText(from);
    this->from = p;
}

void RoutingForm::setToLocation(const QString &to)
{
    ui->lineEdit_to->setText(to);
    this->on_lineEdit_to_editingFinished();
}

void RoutingForm::setToLocation( const QString&to, const GeoPoint& p )
{
    ui->lineEdit_to->setText(to);
    this->to = p;
}

void RoutingForm::on_pushButton_switch_clicked()
{
    QString tmp = ui->lineEdit_from->text();
    std::swap(from, to);
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
    if (this->from != this->to)
    {
        int currentIndex=ui->direct_comboBox->currentIndex();
        int uid=ui->direct_comboBox->itemData(currentIndex).toInt();
        if(currentIndex==0)  //google 导航
            uid=-1;
        emit doRoutingRequest(from, to, uid);
    }
}
void RoutingForm::setFriends()
{
    this->ui->direct_comboBox->clear();
    QList<ProtocolBuffer::User*> friendlist=this->getSession()->friends();
    this->ui->direct_comboBox->addItem("Google Direction");

    for(int i=0;i<friendlist.count();i++)
    {
        int uid=friendlist.value(i)->uid();
        this->ui->direct_comboBox->addItem(friendlist.value(i)->nickname().c_str(),uid);
    }
    this->ui->direct_comboBox->setCurrentIndex(0);
}

void RoutingForm::on_lineEdit_from_editingFinished()
{
    if (ui->lineEdit_from->text().isEmpty())
        from = GeoPoint();
    else
    {
        getService()->GeoCode(ui->lineEdit_from->text(), from, google::protobuf::NewCallback(google::protobuf::DoNothing));
    }
}

void RoutingForm::on_lineEdit_to_editingFinished()
{
    if (ui->lineEdit_to->text().isEmpty())
        to = GeoPoint();
    else
    {
        getService()->GeoCode(ui->lineEdit_to->text(), to, google::protobuf::NewCallback(google::protobuf::DoNothing));
    }
}
