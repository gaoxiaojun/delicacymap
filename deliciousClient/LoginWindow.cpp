#include "LoginWindow.h"
#include "Session.h"
#include "md5.h"
#include "MapProtocol.pb.h"
#include "Configurations.h"
#include "ui_loginWindow.h"

LoginWindow::LoginWindow(void)
: dialog(new Ui::loginWindow)
{
    dialog->setupUi(this);
    dialog->label_nickname->hide();
    dialog->lineEdit_nickname->hide();
    session = new Session();;
    sessionreturned = false;
    logincallback = google::protobuf::NewPermanentCallback(this, &LoginWindow::loginResponse);
    connect(dialog->okButton, SIGNAL(clicked(bool)), this, SLOT(login_step1()));
    connect(this, SIGNAL(loginSuccess()), this, SLOT(success()));
    connect(this, SIGNAL(loginSuccess()), session, SLOT(loginMessenger()));
    connect(this, SIGNAL(loginFailed()), this, SLOT(failed()));
    if (Configurations::Instance().EnableAutoLogin())
    {
        this->dialog->lineEdit_username->setText( QString::fromUtf8(Configurations::Instance().AutoLogin_Username().c_str()) );
        this->dialog->lineEdit_password->setText( QString::fromUtf8(Configurations::Instance().AutoLogin_Password().c_str()) );
        login_step1();
    }
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
    dialog->label_status->setText(tr("Connecting server...."));
    connect(&session->getDataSource(), SIGNAL(ready(bool)), this, SLOT(login_step2(bool)));

    session->getDataSource().connect();
}

void LoginWindow::login_step2(bool connected)
{
    disconnect(this, SLOT(login_step2(bool)));
    if (connected)
    {
        dialog->label_status->setText(tr("Verifing username/password...."));
        MD5 md5(dialog->lineEdit_password->text());
        std::string email(dialog->lineEdit_username->text().toUtf8().constData());
        std::string pwd(md5.toString().toUtf8().constData());
        session->getDataSource().UserLogin(email, pwd, session->getUser(), logincallback);
    }
    else
    {
        dialog->label_status->setText(tr("Error connecting server.").append(session->getDataSource().error()));
    }
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
    dialog->label_status->setText(tr("Login succeeded...."));
    accept();
}

void LoginWindow::failed()
{
    QString errorText;
    if (session->getDataSource().isReady())
        errorText = session->getDataSource().lastRPCError();
    else
        errorText = session->getDataSource().error();
    dialog->label_status->setText(tr("Login failed! ") + errorText);
}

void LoginWindow::on_btnRegister_clicked()
{
    if (!dialog->label_nickname->isVisible())
    {
        dialog->label_nickname->show();
        dialog->lineEdit_nickname->show();
    }
    else if (dialog->lineEdit_username->text().isEmpty() || dialog->lineEdit_nickname->text().isEmpty())
    {
        dialog->label_status->setText(tr("Email address and Nickname needed!"));
    }
    else
    {
        dialog->label_status->setText(tr("Connecting server...."));
        connect(&session->getDataSource(), SIGNAL(ready(bool)), this, SLOT(register_step2(bool)));

        session->getDataSource().connect();
    }
}

void LoginWindow::register_step2(bool connected)
{
    disconnect(this, SLOT(register_step2(bool)));
    if (connected)
    {
        dialog->label_status->setText(tr("Registering..."));

        MD5 md5(dialog->lineEdit_password->text());
        std::string nickname(dialog->lineEdit_nickname->text().toUtf8().constData());
        std::string email(dialog->lineEdit_username->text().toUtf8().constData());
        std::string pwd(md5.toString().toUtf8().constData());
        google::protobuf::Closure *closure = google::protobuf::NewCallback(this, &LoginWindow::registerResponse);
        session->getDataSource().RegisterUser(email, pwd, nickname, session->getUser(), closure);
    }
    else
    {
        dialog->label_status->setText(tr("Error connecting server.").append(session->getDataSource().error()));
    }
}

void LoginWindow::registerResponse()
{
    if (session->getUser()->has_uid())
        emit loginSuccess();
    else
    {
        dialog->label_status->setText( tr("Register failed. ") + session->getDataSource().lastRPCError() );
    }
}
