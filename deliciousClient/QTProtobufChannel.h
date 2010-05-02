#ifndef __QTPROTOBUFCHANNEL__H__INCLUDED__
#define __QTPROTOBUFCHANNEL__H__INCLUDED__

#include <string>
#include <QtCore>
#include <QStack>
#include <QThread>
#include <QtNetwork>
#include "Message.pb.h"

struct CallEntry
{
	CallEntry(google::protobuf::Closure* _done = NULL, google::protobuf::MessageLite* _response = NULL, protorpc::Message *_request = NULL)
		: done(_done), response(_response), request(_request)
	{
	}

	google::protobuf::Closure* done;
	google::protobuf::MessageLite* response;
	protorpc::Message* request;
};

class QTProtobufChannelDriver;

namespace ProtocolBuffer{
    class DMessage;
}

// light weight rpc channel, not intended to be used in multi threaded environment. however, it implement a call queue, so that continuous call wont fail
class QTProtobufChannel :
	public QThread
{
	Q_OBJECT
public:
	QTProtobufChannel(const QHostAddress &serveraddr, unsigned short port);
	~QTProtobufChannel(void);

	void start();
	bool started();
	void close();
    QString errorString();
    const QString& lastRPCErrorString();

    void CallMethod( protorpc::FunctionID method_id, const google::protobuf::MessageLite* request, google::protobuf::MessageLite* response, google::protobuf::Closure* done );

    void SendMessage( ProtocolBuffer::DMessage* );

    void returnQueryBuffer(protorpc::Message*);

    void returnDMessageBuffer( const ProtocolBuffer::DMessage* );

protected:
    // override QThread::run
    void run();

signals:
    void disconnected();
    void connected();
    void error(QAbstractSocket::SocketError);
    void messageReceived(const google::protobuf::MessageLite*);

    void writeMessage(protorpc::Message* m);
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

#endif
