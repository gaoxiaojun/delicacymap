#pragma once
#include <string>
#include <QtCore>
#include <QStack>
#include <QThread>
#include <QtNetwork>
#include <google/protobuf/service.h>
#include "../protocol-buffer-src/Message.pb.h"

struct CallEntry
{
	CallEntry(google::protobuf::Closure* _done = NULL, google::protobuf::Message* _response = NULL, protorpc::Message *_request = NULL)
		: done(_done), response(_response), request(_request)
	{
	}

	google::protobuf::Closure* done;
	google::protobuf::Message* response;
	protorpc::Message* request;
};

class QTProtobufChannel;

class QTProtobufChannelDriver : public QObject
{
	Q_OBJECT
public slots:
	void writeMessage(google::protobuf::Message* m);
	void start(QHostAddress *_addr, unsigned short _port);
private slots:
	void readMessage();
public:
	QTProtobufChannelDriver(QTProtobufChannel* parent, QHash<int,CallEntry> *currentCalls);
	~QTProtobufChannelDriver();

	bool started();
	QAbstractSocket::SocketError networkError() const;

	protorpc::Message response;
	std::string _writebuffer;
	std::string _readbuffer;
	QTProtobufChannel* parent;
	QTcpSocket *_tcps;
	QHash<int,CallEntry>* _currentCalls;
	int _buffer_index, _msgsize;
};

// light weight rpc channel, not intended to be used in multi threaded environment. however, it implement a call queue, so that continuous call wont fail
class QTProtobufChannel :
	public QThread,
	public google::protobuf::RpcChannel
{
	Q_OBJECT
public:
	QTProtobufChannel(const QHostAddress &serveraddr, unsigned short port);
	~QTProtobufChannel(void);

	void start();
	bool started();
	void close();
    QString errorString();

	// override RpcChannel::CallMethod
	void CallMethod(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller, const google::protobuf::Message* request, google::protobuf::Message* response, google::protobuf::Closure* done);

	void returnQueryBuffer(protorpc::Message*);

protected:
    // override QThread::run
    void run();

signals:
    void disconnected();
    void connected();
    void error();

	void writeMessage(google::protobuf::Message* m);
	void requetStart(QHostAddress *_addr, unsigned short _port);

private:
	bool readMessage(google::protobuf::Message* m);

	void needMoreReqs();
private:
	QStack<protorpc::Message*> reqs;
	QHash<int,CallEntry> _currentCalls;
	QHostAddress _addr;
	QTProtobufChannelDriver *_helper;
	int _callid;
	unsigned short _port;
};
