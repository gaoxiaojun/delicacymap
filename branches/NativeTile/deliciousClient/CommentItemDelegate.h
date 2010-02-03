#ifndef __COMMENTITEMDELEGATE__H__INCLUDED__
#define __COMMENTITEMDELEGATE__H__INCLUDED__

#include <QStyledItemDelegate>

class CommentItemDelegate :public QStyledItemDelegate
{
    Q_OBJECT
public:
    CommentItemDelegate(void);
    ~CommentItemDelegate(void);

    // overridden implementations
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif
