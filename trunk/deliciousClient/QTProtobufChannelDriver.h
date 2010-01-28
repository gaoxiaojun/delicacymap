/*
 *  QTProtobufChannelDriver.h
 *  delicacyClient
 *
 *  Created by R.C on 10-1-25.
 *
 */

#ifndef __QTPROTOBUFCHANNELDRIVER__H__INCLUDED__
#define __QTPROTOBUFCHANNELDRIVER__H__INCLUDED__

#include "QTProtobufChannel.h"
#include "MapProtocol.pb.h"

class QTProtobufChannelDriver : public QObject
{
	Q_OBJECT
	friend class QTProtobufChannel;
signals:
    void MessageReceived(const google::protobuf::MessageLite*);
private slots:
	void writeMessage(google::protobuf::MessageLite* m);
	void start(QHostAddress *_addr, unsigned short _port);
	void readMessage();
public:
	QTProtobufChannelDriver(QTProtobufChannel* parent, QHash<int,CallEntry> *currentCalls);
	~QTProtobufChannelDriver();
	
	bool started();
	QAbstractSocket::SocketError networkError() const;
	
    ProtocolBuffer::DMessage dmessage;
	protorpc::Message response;
	std::string _writebuffer;
	std::string _readbuffer;
	QTProtobufChannel* parent;
	QTcpSocket *_tcps;
	QHash<int,CallEntry>* _currentCalls;
	int _buffer_index, _msgsize;
};

#endif
