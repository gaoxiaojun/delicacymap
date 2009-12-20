#include "DisplaySchema.h"
#include "../protocol-buffer-src/MapProtocol.pb.h"

QString DisplaySchemas::RestaurantInfoWindowSchema( const ProtocolBuffer::Restaurant  * const r )
{
    static const QString schema(QString::fromLocal8Bit(
        "<B>%1</B><br>"
        "<HR>"
        "评分：%2<br>平均消费：%3 元<br>"
        "<a href=\"javascript:mapListener.moreCommentsClicked(%4)\">共有%5条评论</a><br>"
        "<a href=\"javascript:\">从此出发</a>&nbsp;&nbsp;<a href=\"javascript:\">到此</a>"));
    return schema.arg(QString::fromUtf8(r->name().c_str(), r->name().length()))
                 .arg(r->rating())
                 .arg(r->averageexpense().amount())
                 .arg(r->rid())
                 .arg(r->commentcount());
}
