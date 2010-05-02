#include "user.h"
#include "Session.h"
using namespace ProtocolBuffer;

usr::usr(QWidget *parent, Qt::WFlags flags)
    : QWidget(parent, flags),ui(new Ui::UserClass)
{
    ui->setupUi(this);
    //disconnectall();
}
void usr:: disconnectall()
{
    //disconnect(ui->addComment_Button,SIGNAL(clicked()),this,);
    disconnect(ui->addFriend_Button,SIGNAL(clicked()),this,SLOT(addFriend()));
    disconnect(ui->addFriend_Button,SIGNAL(clicked()),this,SLOT(deleteFriend()));
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
    disconnectall();
    uid=_uid;
    ui->usrname->setText(name);
    ui->mail->setText(mail);
    ui->joinTime->setText(addtime);
    if(this->getSession()->isfriend(uid))
    {
        ui->addFriend_Button->setText(tr("deleteFriend"));
        connect(ui->addFriend_Button,SIGNAL(clicked()),this,SLOT(deleteFriend()));
    }
    else
    {
        ui->addFriend_Button->setText(tr("addFriend"));
        connect(ui->addFriend_Button,SIGNAL(clicked()),this,SLOT(addFriend()));
    }
    if(this->getSession()->isSubscribedToUser(uid))
        ui->addComment_Button->setText(tr("Subscribe"));
    else
        ui->addComment_Button->setText(tr("unSubscribe"));
}
void usr::SubscribeToUser()
{
    this->getSession()->SubscribeToUser(uid);
    ui->addComment_Button->setText("È¡ÏûÆÀÂÛ");
}
void usr::deleteFriend()
{
    this->getSession()->setRelationWith(uid,Unspecified);
    ui->addFriend_Button->setText(tr("addFriend"));
    this->close();
}
void usr::addFriend()
{
    this->getSession()->setRelationWith(uid,Friend);
    ui->addFriend_Button->setText(tr("deleteFriend"));
    this->close();
}