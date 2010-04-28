#include "SearchResultForm.h"
#include "OfflineMap/MapViewBase.h"
#include "OfflineMap/MarkerItem.h"
#include "MapProtocol.pb.h"
#include "ui_SearchResultForm.h"

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
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(itemClicked(QListWidgetItem*)));
    for (int i=0;i<result->restaurants().restaurants_size();i++)
    {
        QListWidgetItem* item = new QListWidgetItem(QString::fromUtf8( result->restaurants().restaurants(i).name().c_str() ));
        item->setData( Qt::UserRole, i );
        ui->listWidget->addItem(item);
    }

    if (!result->has_restaurants() && !result->has_users())
    {
        QListWidgetItem* item = new QListWidgetItem(tr("     <Not Found>"));
        item->setData( Qt::UserRole, -1 );
        ui->listWidget->addItem(item);
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
    const ProtocolBuffer::Restaurant& r = result->restaurants().restaurants(item->data( Qt::UserRole ).toInt());
    int rid = r.rid();
    if (rid == -1)
        return;
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
