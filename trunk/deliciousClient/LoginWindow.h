#ifndef __LOGINWINDOW__H__INCLUDED__
#define __LOGINWINDOW__H__INCLUDED__

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
    void on_btnRegister_clicked();
    void login_step1();
    void login_step2(bool);
    void register_step2(bool);
    void success();
    void failed();

private:
    void loginResponse();
    void registerResponse();

private:
    Session* session;
    Ui::loginWindow* dialog;
    google::protobuf::Closure *logincallback;
    bool sessionreturned;
};

#endif
