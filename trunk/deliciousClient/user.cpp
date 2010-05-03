#include "user.h"
#include "Session.h"
using namespace ProtocolBuffer;

usr::usr(QWidget *parent, Qt::WFlags flags)
    : QWidget(parent, flags),ui(new Ui::UserClass)
{
    ui->setupUi(this);
    connect(ui->locationShare_Button,SIGNAL(clicked()),this,SLOT(ShareLocation()));
    ui->locationShare_Button->setText(tr("ShareLocation"));
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
    if(_uid==this->getSession()->getUser()->uid())
    {
        ui->addComment_Button->setVisible(false);
        ui->addFriend_Button->setVisible(false);
        ui->locationShare_Button->setVisible(false);
    }
    if(!this->getSession()->isFriend(_uid))
    {
        ui->addComment_Button->setVisible(false);
        ui->locationShare_Button->setVisible(false);
    }
    disconnectall();
    uid=_uid;
    ui->usrname->setText(QString::fromUtf8(name));
    ui->mail->setText(QString::fromUtf8(mail));
    ui->joinTime->setText(QString::fromUtf8(addtime));
    if(this->getSession()->isFriend(uid))
    {
        ui->addFriend_Button->setText(tr("Delete friend"));
        connect(ui->addFriend_Button,SIGNAL(clicked()),this,SLOT(deleteFriend()));
        disconnect(ui->addFriend_Button,SIGNAL(clicked()),this,SLOT(addFriend()));
    }
    else
    {
        ui->addFriend_Button->setText(tr("Add as friend"));
        connect(ui->addFriend_Button,SIGNAL(clicked()),this,SLOT(addFriend()));
        disconnect(ui->addFriend_Button,SIGNAL(clicked()),this,SLOT(deleteFriend()));
    }
    if(this->getSession()->isSubscribedToUser(uid))
    {
        ui->addComment_Button->setText(tr("UnSubscribe"));
        disconnect(ui->addComment_Button,SIGNAL(clicked()),this,SLOT(SubscribeToUser()));
        connect(ui->addComment_Button,SIGNAL(clicked()),this,SLOT(UnSubscribeToUser()));
    }
    else
    {
        ui->addComment_Button->setText(tr("Subscribe"));
        connect(ui->addComment_Button,SIGNAL(clicked()),this,SLOT(SubscribeToUser()));
        disconnect(ui->addComment_Button,SIGNAL(clicked()),this,SLOT(UnSubscribeToUser()));
    }
}

void usr::UnSubscribeToUser()
{
    this->getSession()->UnSubscribeFromUser(uid);
    ui->addComment_Button->setText(tr("Subscribe"));
    this->close();
}
void usr::SubscribeToUser()
{
    this->getSession()->SubscribeToUser(uid);
    ui->addComment_Button->setText(tr("UnSubscribe"));
    this->close();
}
void usr::deleteFriend()
{
    this->getSession()->setRelationWith(uid,Unspecified);
    ui->addFriend_Button->setText(tr("Add as friend"));
    this->close();
}

void usr::addFriend()
{
    this->getSession()->setRelationWith(uid,Friend);
    ui->addFriend_Button->setText(tr("Delete friend"));
    this->close();
}
void usr::ShareLocation()
{
    this->getSession()->ShareMyLocationWith(uid);
    ui->locationShare_Button->setText(tr("UnShareLocation"));
    disconnect(ui->locationShare_Button,SIGNAL(clicked()),this,SLOT(ShareLocation()));
    connect(ui->locationShare_Button,SIGNAL(clicked()),this,SLOT(nShareLocation()));
    this->close();
}
void usr::nShareLocation()
{
    this->getSession()->UnShareMyLocationWith(uid);   
    ui->locationShare_Button->setText(tr("ShareLocation"));
    connect(ui->locationShare_Button,SIGNAL(clicked()),this,SLOT(ShareLocation()));
    disconnect(ui->locationShare_Button,SIGNAL(clicked()),this,SLOT(nShareLocation()));
    this->close();
}