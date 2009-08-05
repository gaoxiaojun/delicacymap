#include "ConnectionManager.h"

ConnectionManager::ConnectionManager()
{
	socket = new QTcpSocket((QObject*)this);
	QString serverIP = "127.0.0.l";
	QHostAddress address(serverIP);
	socket->connectToHost(address,PORT_NUM);

	channel = new ::protorpc::TwoWayStream(socket,NULL);
	stub = new ::ProtocolBuffer::DMService::Stub(channel);
}

ConnectionManager::~ConnectionManager()
{
	socket->close();
	delete socket;
	delete channel;
	delete stub;
}

::ProtocolBuffer::DMService::Stub* ConnectionManager::getStub()
{
	return stub;
}