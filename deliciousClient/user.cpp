#include "user.h"
#include "Session.h"
using namespace ProtocolBuffer;

usr::usr(QWidget *parent, Qt::WFlags flags)
    : QWidget(parent, flags),ui(new Ui::UserClass)
{
    ui->setupUi(this);
    connect(ui->addComment_Button,SIGNAL(clicked()),this,SLOT(SubscribeToUser()));
    connect(ui->addFriend_Button,SIGNAL(clicked()),this,SLOT(deleteFriend()));
}

usr::~usr()
{
    
}
void usr::setSession(Session *s)
{
    session=s;
}
Session* usr::getSession()
{
    return session;
}
void usr::setusr(int _uid,const char * name,const char * mail,const char * addtime)
{
    uid=_uid;
    ui->usrname->setText(name);
    ui->mail->setText(mail);
    ui->joinTime->setText(addtime);
}
void usr::SubscribeToUser()
{
    this->getSession()->SubscribeToUser(uid);
}
void usr::deleteFriend()
{
    this->getSession()->setRelationWith(uid,Unspecified);
}