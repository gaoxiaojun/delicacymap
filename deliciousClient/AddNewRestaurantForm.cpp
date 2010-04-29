#include "AddNewRestaurantForm.h"
#include "ui_AddNewRestaurantForm.h"
#include "MapProtocol.pb.h"

AddNewRestaurantForm::AddNewRestaurantForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddNewRestaurantForm)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
}

AddNewRestaurantForm::~AddNewRestaurantForm()
{
    delete ui;
}

void AddNewRestaurantForm::changeEvent(QEvent *e)
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

void AddNewRestaurantForm::setRestaurant(ProtocolBuffer::Restaurant *res)
{
    this->r = res;
    if (r->has_name())
        ui->lineEdit_name->setText(QString::fromUtf8(r->name().c_str()));
    if (r->has_averageexpense())
        ui->spinBox_price->setValue(r->averageexpense().amount());
    if (r->has_type())
        ui->comboBox_type->setCurrentIndex(r->type().tid());
}

void AddNewRestaurantForm::on_lineEdit_name_editingFinished()
{
    Q_ASSERT(r);
    r->set_name(ui->lineEdit_name->text().toUtf8().constData(), ui->lineEdit_name->text().toUtf8().size());
}

void AddNewRestaurantForm::on_spinBox_price_editingFinished()
{
    Q_ASSERT(r);
    r->mutable_averageexpense()->set_amount(ui->spinBox_price->value());
}

void AddNewRestaurantForm::on_comboBox_type_currentIndexChanged(int index)
{
    Q_ASSERT(r);
    r->mutable_type()->set_tid(index);
    r->mutable_type()->set_name(ui->comboBox_type->itemText(index).toUtf8().constData());
}
