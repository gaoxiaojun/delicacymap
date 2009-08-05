#pragma once

#include <QtCore>
#include <QtNetwork>
#include <QtDebug>
#include "../protocol-buffer-src/MapProtocol.pb.h"
#include "twowaystream.h"
#include "simplerpccontroller.h"

#define PORT_NUM 24000

class ConnectionManager : public QObject
{
	Q_OBJECT
public:
	ConnectionManager();
	~ConnectionManager();
	::ProtocolBuffer::DMService::Stub* getStub();
protected:
	QTcpSocket *socket;
	::ProtocolBuffer::DMService::Stub *stub;
	::protorpc::TwoWayStream *channel;
	::protorpc::SimpleRpcController	controller;
};