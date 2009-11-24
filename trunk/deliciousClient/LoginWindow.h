#pragma once
#include <QtGui/QDialog>

namespace Ui{
    class loginWindow;
}

namespace google{
    namespace protobuf{
        class Closure;
    }
}

class Session;

class LoginWindow :
    public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(LoginWindow)
public:
    LoginWindow(void);
    ~LoginWindow(void);
    Session* getSession();

signals:
    void loginSuccess();
    void loginFailed();

private slots:
    void login_step1();
    void login_step2(bool);
    void success();
    void failed();

private:
    void loginResponse();

private:
    Session* session;
    Ui::loginWindow* dialog;
    google::protobuf::Closure *logincallback;
    bool sessionreturned;
};
