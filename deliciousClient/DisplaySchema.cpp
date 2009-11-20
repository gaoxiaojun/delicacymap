#include "DisplaySchema.h"
#include "../protocol-buffer-src/MapProtocol.pb.h"

QString DisplaySchemas::RestaurantInfoWindowSchema( const ProtocolBuffer::Restaurant  * const r )
{
	static const QString schema(QString::fromLocal8Bit(
		"<B>%1</B><br>"
		"<HR>"
		"���֣�%2<br>ƽ�����ѣ�%3 Ԫ<br>"
		"<a href=\"function(){mapListener.markerClicked();}\">����%4������</a>"));
	return schema.arg(QString::fromUtf8(r->name().c_str(), r->name().length()))
				 .arg(r->rating())
				 .arg(r->averageexpense().amount())
				 .arg(r->commentcount());
}
