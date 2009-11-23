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
    connect(dialog->okButton, SIGNAL(clicked(bool)), this, SLOT(login()));
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

void LoginWindow::login()
{
    if (!session)
        session = new Session();
    MD5 md5(dialog->lineEdit_password->text());
    std::string email = dialog->lineEdit_username->text().toStdString();
    std::string pwd   = md5.toString().toStdString();
    session->getDataSource().UserLogin(email, pwd, session->getUser(), logincallback);
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
    accept();
}

void LoginWindow::failed()
{
    this->setResult(QDialog::Rejected);
}
