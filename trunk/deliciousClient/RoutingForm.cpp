#include "RoutingForm.h"
#include "ui_RoutingForm.h"
#include "Session.h"

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
    QString from,to;
    int uid,currentIndex;
    from=ui->lineEdit_from->text();
    to =ui->lineEdit_to->text();
    currentIndex=ui->direct_comboBox->currentIndex();
    uid=ui->direct_comboBox->itemData(currentIndex).toInt();
    if(currentIndex==0)  //用户导航
        uid=-1;
    emit doRoutingRequest(from ,to, uid);;//google 导航
    
}
void RoutingForm::setFriends()
{
    this->ui->direct_comboBox->clear();
    int uid;
    QList<ProtocolBuffer::User*> friendlist=this->getSession()->friends();

    for(int i=0;i<friendlist.count();i++)
    {
        uid=friendlist.value(i)->uid();
        this->ui->direct_comboBox->addItem("Google Direction");
        this->ui->direct_comboBox->addItem(friendlist.value(i)->nickname().c_str(),uid);
    }
    this->ui->direct_comboBox->setCurrentIndex(0);
}