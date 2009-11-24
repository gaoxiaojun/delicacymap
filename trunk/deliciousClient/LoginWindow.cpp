#include "LoginWindow.h"
#include "Session.h"
#include "md5.h"
#include "../protocol-buffer-src/MapProtocol.pb.h"
#include "ui_loginWindow.h"

LoginWindow::LoginWindow(void)
: dialog(new Ui::loginWindow)
{
    dialog->setupUi(this);
    session = NULL;
    logincallback = google::protobuf::NewPermanentCallback(this, &LoginWindow::loginResponse);
    connect(dialog->okButton, SIGNAL(clicked(bool)), this, SLOT(login_step1()));
    connect(this, SIGNAL(loginSuccess()), this, SLOT(success()));
    connect(this, SIGNAL(loginFailed()), this, SLOT(failed()));
}

LoginWindow::~LoginWindow(void)
{
    delete dialog;
    if (!sessionreturned)
        delete session;
    delete logincallback;
}

void LoginWindow::login_step1()
{
    dialog->label_status->setText(QString::fromLocal8Bit("正在连接服务器...."));
    if (!session)
    {
        session = new Session();
        connect(&session->getDataSource(), SIGNAL(ready(bool)), this, SLOT(login_step2(bool)));
    }

    session->getDataSource().connect();
}

Session* LoginWindow::getSession()
{
    sessionreturned = true;
    return session;
}

void LoginWindow::loginResponse()
{
    if (session->getUser()->has_uid())
        emit loginSuccess();
    else
        emit loginFailed();
}

void LoginWindow::success()
{
    dialog->label_status->setText(QString::fromLocal8Bit("登陆成功...."));
    accept();
}

void LoginWindow::failed()
{
    dialog->label_status->setText(QString::fromLocal8Bit("登陆失败，用户名密码不匹配！...."));
}

void LoginWindow::login_step2(bool connected)
{
    if (connected)
    {
        dialog->label_status->setText(QString::fromLocal8Bit("正在登陆...."));
        MD5 md5(dialog->lineEdit_password->text());
        std::string email = dialog->lineEdit_username->text().toStdString();
        std::string pwd   = md5.toString().toStdString();
        session->getDataSource().UserLogin(email, pwd, session->getUser(), logincallback);
    }
    else
    {
        dialog->label_status->setText(QString::fromLocal8Bit("连接服务器失败：").append(session->getDataSource().error()));
    }
}
