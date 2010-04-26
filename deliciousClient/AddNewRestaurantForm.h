#ifndef ADDNEWRESTAURANTFORM_H
#define ADDNEWRESTAURANTFORM_H

#include <QWidget>

namespace Ui {
    class AddNewRestaurantForm;
}

namespace ProtocolBuffer{
    class Restaurant;
}

class AddNewRestaurantForm : public QWidget {
    Q_OBJECT
public:
    AddNewRestaurantForm(QWidget *parent = 0);
    ~AddNewRestaurantForm();

    void setRestaurant(ProtocolBuffer::Restaurant*);
    ProtocolBuffer::Restaurant* rinfo() const {return r;}

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AddNewRestaurantForm *ui;
    ProtocolBuffer::Restaurant *r;

private slots:
    void on_comboBox_type_currentIndexChanged(int index);
    void on_spinBox_price_editingFinished();
    void on_lineEdit_name_editingFinished();
};

#endif // ADDNEWRESTAURANTFORM_H
