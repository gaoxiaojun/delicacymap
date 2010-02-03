#ifndef __DISPLAYSCHEMA__H__INCLUDED__
#define __DISPLAYSCHEMA__H__INCLUDED__

#include <QString>

namespace ProtocolBuffer
{
	class Restaurant;
}

class DisplaySchemas
{
public:

	static QString RestaurantInfoWindowSchema( const ProtocolBuffer::Restaurant  * const );
};

#endif
