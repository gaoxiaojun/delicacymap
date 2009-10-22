/* Copyright (C) 2009 Frederik M.J.V

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * LGPL is available on the internet at
 * http://www.gnu.org/licenses/lgpl-2.1.html and from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
*/

#ifndef TWOWAYSTREAM_H
#define TWOWAYSTREAM_H

#include <QtCore>
#include <QtNetwork>
#include "callentry.h"
#include "streamcallbackinfo.h"
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>

namespace protorpc{
class TwoWayStream : public QThread, public google::protobuf::RpcChannel
{
    Q_OBJECT
protected:
    QTcpSocket *iodev;
    QMutex streamlock;
    QMutex devlock;
    bool connected;
	volatile bool stopped;
    //QWaitCondition initcond;
    google::protobuf::Closure* shutdownCallback;
private:
    google::protobuf::Service *service;
    uint32_t callnum;
    bool spawnCallers;

	QHostAddress server;
	int port;
    QHash<uint32_t,CallEntry> currentCalls;
public:
    TwoWayStream(QHostAddress, int,bool autostart=false,google::protobuf::Closure *shutdownClosure=NULL);
    ~TwoWayStream();
    //void callMethod(const google::protobuf::MethodDescriptor * method, google::protobuf::RpcController * controller, const google::protobuf::Message * request, google::protobuf::Message * response, google::protobuf::Closure * done);
    void CallMethod(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller, const google::protobuf::Message* request, google::protobuf::Message* response, google::protobuf::Closure* done);
    void response(StreamCallbackInfo *entry);
    void start();
    void shutdown(bool closeStreams);
    bool cancelMethodCall(google::protobuf::Message* response);
protected:
    void writeMessage(google::protobuf::MessageLite* m);
    void callMethodThreaded(const google::protobuf::MethodDescriptor * method, google::protobuf::RpcController * controller, const google::protobuf::Message * request, google::protobuf::Message * response, google::protobuf::Closure * done);
    bool fillMessage(google::protobuf::MessageLite &type);
    void requestServiceDescriptor(google::protobuf::Closure *cb,google::protobuf::RpcController *ctrl);
    void run();
    void cleanup();
signals:
    void channelBroken(google::protobuf::RpcChannel *chan);
};
};
//#include "twowaystream.moc"
#endif // TWOWAYSTREAM_H


