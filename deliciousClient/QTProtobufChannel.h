#pragma once
#include <string>
#include <QtCore>
#include <QThread>
#include <QtNetwork>
#include <google/protobuf/service.h>
#include "..\protocol-buffer-src\Message.pb.h"

struct CallEntry
{
	CallEntry(google::protobuf::Closure* _done = NULL, google::protobuf::Message* _response = NULL)
		: done(_done), response(_response)
	{
	}

	google::protobuf::Closure* done;
	google::protobuf::Message* response;
};

// light weight rpc channel, not intended to be used in multi threaded environment.
class QTProtobufChannel :
	public QThread,
	public google::protobuf::RpcChannel
{
	Q_OBJECT
public:
	QTProtobufChannel(const QHostAddress &serveraddr, unsigned short port);
	~QTProtobufChannel(void);

	void start();
	void close();

	// override RpcChannel::CallMethod
	void CallMethod(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller, const google::protobuf::Message* request, google::protobuf::Message* response, google::protobuf::Closure* done);

protected:
	// override QThread::run
	void run();

private:
	bool readMessage(google::protobuf::Message* m);
	void writeMessage(google::protobuf::Message* m);
private:
	QTcpSocket _tcps;
	QHostAddress _addr;
	unsigned short _port;
	int _callid;
	QHash<int,CallEntry> _currentCalls;
	protorpc::Message _reqholder;
	std::string _writebuffer;
	std::string _readbuffer;
	int _buffer_index, _msgsize;
};
