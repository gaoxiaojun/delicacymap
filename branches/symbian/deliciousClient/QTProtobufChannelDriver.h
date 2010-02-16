#ifndef QTPROTOBUFCHANNELDRIVER_H
#define QTPROTOBUFCHANNELDRIVER_H

#include "QTProtobufChannel.h"

class QTProtobufChannelDriver : public QObject
{
        Q_OBJECT
        friend class QTProtobufChannel;
private slots:
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

#endif // QTPROTOBUFCHANNELDRIVER_H