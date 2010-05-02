#include "SearchResultForm.h"
#include "OfflineMap/MapViewBase.h"
#include "OfflineMap/MarkerItem.h"
#include "MapProtocol.pb.h"
#include "ui_SearchResultForm.h"
#include "user.h"

SearchResultForm::SearchResultForm(ProtocolBuffer::SearchResult* result, MapViewBase *t, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchResultForm)
{
    Q_ASSERT(result);
    panel = NULL;
    ui->setupUi(this);
    this->result = result;
    this->target = t;
    ui->listWidget->setFocus();
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(itemClicked(QListWidgetItem*)));
    for (int i=0;i<result->restaurants().restaurants_size();i++)
    {
        QListWidgetItem* item = new QListWidgetItem(QString::fromUtf8( result->restaurants().restaurants(i).name().c_str() ));
        item->setData( Qt::UserRole, i );
        ui->listWidget->addItem(item);
    }

    if (result->has_restaurants() && result->has_users())
    {
        ui->listWidget->addItem("-----------------");
    }

    for (int i=0;i<result->users().users_size();i++)
    {
        QListWidgetItem* item = new QListWidgetItem(QString::fromUtf8( result->users().users(i).nickname().c_str() ));
        item->setData( Qt::UserRole+1, i );
        ui->listWidget->addItem(item);
    }

    if (!result->has_restaurants() && !result->has_users())
    {
        ui->listWidget->addItem(tr("     <Not Found>"));
    }
}

SearchResultForm::~SearchResultForm()
{
    delete ui;
    delete result;
}

void SearchResultForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SearchResultForm::itemClicked(QListWidgetItem *item)
{
    if (item->data(Qt::UserRole).isValid())
    {
        const ProtocolBuffer::Restaurant& r = result->restaurants().restaurants(item->data( Qt::UserRole ).toInt());
        int rid = r.rid();
        Q_ASSERT( rid>0 );
        RestaurantMarkerItem *marker = target->getRestaurantMarker(rid);
        if (!marker) // this item is not in the view yet!
        {
            target->addRestaurantMarker( new ProtocolBuffer::Restaurant(r) );
            marker = target->getRestaurantMarker(rid);
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            Q_ASSERT( marker );
        }
        target->centerOn(marker, true);
        if (panel)
        {
            panel->tie(marker);
            target->ensureVisible(panel);
        }
    }
    else if (item->data(Qt::UserRole+1).isValid())
    {
        const ProtocolBuffer::User& user = result->users().users(item->data(Qt::UserRole+1).toInt());
        usr* usrWindow = new usr();
        usrWindow->setSession(this->getSession());

        usrWindow->setusr(user.uid(), user.nickname().c_str(), user.emailaddress().c_str(), user.jointime().timestamp().c_str());
        usrWindow->show();
    }
}
