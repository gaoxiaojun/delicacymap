#include "CommentItemDelegate.h"
#include "../protocol-buffer-src/MapProtocol.pb.h"

#include <QPainter>
#include <QString>
#include <QtGui/QLabel>
#include <QtGui/QTextDocument>
#include <QtGui/QLineEdit>
#include <QtGui/QGroupBox>
#include <QtGui/QTextItem>
#include <QApplication>
#include <QStyleOptionGroupBox>
#include <QStyle>
#include <QColor>

CommentItemDelegate::CommentItemDelegate(void)
{
}

CommentItemDelegate::~CommentItemDelegate(void)
{
}

void CommentItemDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QVariant data = index.data(Qt::UserRole);
    ProtocolBuffer::Comment* r = (ProtocolBuffer::Comment*)data.value<void*>();
    
    painter->save();

    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);
    
    painter->drawText(option.rect, QString::fromLocal8Bit("italic ˵:"));
    //painter->drawText(option.rect, QString::fromUtf8(r->content().c_str()));
    //text->setText(QString::fromUtf8(r->name().c_str()));
    //QStyle *s = QApplication::style();
    //s->drawComplexControl(QStyle::CC_GroupBox, &style, painter, &box);
    //s->drawPrimitive(QStyle::PE_Widget, &option, painter, &box);

    painter->restore();
}

QSize CommentItemDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}
