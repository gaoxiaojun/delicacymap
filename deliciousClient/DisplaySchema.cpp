#include "DisplaySchema.h"
#include "../protocol-buffer-src/MapProtocol.pb.h"

QString DisplaySchemas::RestaurantInfoWindowSchema( const ProtocolBuffer::Restaurant  * const r )
{
    static const QString schema(QString::fromLocal8Bit(
        "<B>%1</B><br>"
        "<HR>"
        "���֣�%2<br>ƽ�����ѣ�%3 Ԫ<br>"
        "<a href=\"javascript:mapListener.moreCommentsClicked(%4)\">����%5������</a><br>"
        "<a href=\"javascript:\">�Ӵ˳���</a>&nbsp;&nbsp;<a href=\"javascript:\">����</a>"));
    return schema.arg(QString::fromUtf8(r->name().c_str(), r->name().length()))
                 .arg(r->rating())
                 .arg(r->averageexpense().amount())
                 .arg(r->rid())
                 .arg(r->commentcount());
}