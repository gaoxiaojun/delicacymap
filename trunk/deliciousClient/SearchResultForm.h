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
class PanelWidget;
class Session;

class SearchResultForm : public QWidget {
    Q_OBJECT
public:
    SearchResultForm(ProtocolBuffer::SearchResult* r, MapViewBase*, QWidget *parent = 0);
    ~SearchResultForm();

    void setPanel(PanelWidget* panel) { this->panel = panel; }
    void setSession(Session* s) {this->s = s;}
    Session* getSession() const {return s;}

protected:
    void changeEvent(QEvent *e);

private slots:
    void itemClicked(QListWidgetItem*);

private:
    Ui::SearchResultForm *ui;
    ProtocolBuffer::SearchResult* result;
    MapViewBase* target;
    Session* s;
    PanelWidget *panel;
};

#endif // SEARCHRESULTFORM_H
