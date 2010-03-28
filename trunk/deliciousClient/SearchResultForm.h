#ifndef SEARCHRESULTFORM_H
#define SEARCHRESULTFORM_H

#include <QWidget>

namespace Ui {
    class SearchResultForm;
}

namespace ProtocolBuffer{
    class SearchResult;
}

class MapViewBase;
class QListWidgetItem;

class SearchResultForm : public QWidget {
    Q_OBJECT
public:
    SearchResultForm(ProtocolBuffer::SearchResult* r, MapViewBase*, QWidget *parent = 0);
    ~SearchResultForm();

protected:
    void changeEvent(QEvent *e);

private slots:
    void itemClicked(QListWidgetItem*);

private:
    Ui::SearchResultForm *ui;
    ProtocolBuffer::SearchResult* result;
    MapViewBase* target;
};

#endif // SEARCHRESULTFORM_H
