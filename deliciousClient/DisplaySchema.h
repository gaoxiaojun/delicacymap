#pragma once
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
