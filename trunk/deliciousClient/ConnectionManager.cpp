#include "ConnectionManager.h"

ConnectionManager::ConnectionManager()
{
	channel = new ::protorpc::TwoWayStream(QHostAddress::LocalHost, PORT_NUM, true);
	stub = new ::ProtocolBuffer::DMService::Stub(channel);
}

ConnectionManager::~ConnectionManager()
{
	delete channel;
	delete stub;
}

::ProtocolBuffer::DMService::Stub* ConnectionManager::getStub()
{
	return stub;
}