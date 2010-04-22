#ifndef USERINFOFORM_H
#define USERINFOFORM_H

#include <QWidget>

namespace Ui {
    class UserInfoForm;
}

class UserInfoForm : public QWidget {
    Q_OBJECT
public:
    UserInfoForm(QWidget *parent = 0);
    ~UserInfoForm();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::UserInfoForm *ui;
};

#endif // USERINFOFORM_H
