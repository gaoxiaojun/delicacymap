#include "QTProtobufChannel.h"
#include "QTProtobufChannelDriver.h"

static const int CallBufferIncrease = 4;

QTProtobufChannel::QTProtobufChannel( const QHostAddress &serveraddr, unsigned short port )
:_addr(serveraddr), _port(port)
{
    QThread::start();
    _callid = 0;
    _helper = NULL;
    QThread::msleep(300);
}

QTProtobufChannel::~QTProtobufChannel(void)
{
    close();
    for (QHash<int, CallEntry>::const_iterator it=_currentCalls.begin();it!=_currentCalls.end();++it)
    {
        returnQueryBuffer(it.value().request);
    }
    while (!reqs.empty())
        delete reqs.pop();
}

void QTProtobufChannel::start()
{
    emit requetStart(&_addr, _port);
}

void QTProtobufChannel::close()
{
    exit();
    wait();
}

void QTProtobufChannel::CallMethod( protorpc::FunctionID method_id, const google::protobuf::MessageLite* request, google::protobuf::MessageLite* response, google::protobuf::Closure* done )
{
    if (!started())
    {
        // siliently fail
        delete done;
        delete response;
        return;
    }

    if (reqs.empty())
        needMoreReqs();
    protorpc::Message* reqholder = reqs.pop();
    reqholder->Clear();
    reqholder->set_type(protorpc::REQUEST);
    reqholder->set_id(_callid);
    reqholder->set_method_id(method_id);
    reqholder->set_buffer(request->SerializeAsString());
    _currentCalls.insert(_callid++, CallEntry(done, response, reqholder));
    emit writeMessage(reqholder);
}

void QTProtobufChannel::SendMessage( ProtocolBuffer::DMessage* m )
{
    if (!started())
        return;

    if (reqs.empty())
        needMoreReqs();
    protorpc::Message* reqholder = reqs.pop();
    reqholder->Clear();
    reqholder->set_type(protorpc::MESSAGE);
    reqholder->set_buffer(m->SerializeAsString());
    emit writeMessage(reqholder);
}


void QTProtobufChannel::run()
{
    _helper = new QTProtobufChannelDriver(this, &_currentCalls);

    connect(this, SIGNAL(writeMessage(protorpc::Message*)), _helper, SLOT(writeMessage( protorpc::Message*  )));
    connect(this, SIGNAL(requetStart(QHostAddress*, unsigned short )), _helper, SLOT(start(QHostAddress*, unsigned short)));
    connect(_helper, SIGNAL(MessageReceived(const google::protobuf::MessageLite*)), this, SIGNAL(messageReceived(const google::protobuf::MessageLite*)));
    connect(_helper->_tcps, SIGNAL(connected()), this, SIGNAL(connected()), Qt::DirectConnection);
    connect(_helper->_tcps, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(error()), Qt::DirectConnection);
    connect(_helper->_tcps, SIGNAL(disconnected()), this, SIGNAL(disconnected()), Qt::DirectConnection);

    exec();

    delete _helper;
}

bool QTProtobufChannel::started()
{
    return _helper != NULL && _helper->started();
}

void QTProtobufChannel::needMoreReqs()
{
    for (int i=0;i<CallBufferIncrease;++i)
    {
        reqs.push(new protorpc::Message);
    }
}

void QTProtobufChannel::returnQueryBuffer( protorpc::Message* m )
{
    reqs.push(m);
}

void QTProtobufChannel::returnDMessageBuffer(const ProtocolBuffer::DMessage *el)
{
    _helper->freeMessage(el);
}

QString QTProtobufChannel::errorString()
{
    if (_helper)
    {
        return _helper->_tcps->errorString();
    }
    return QString();
}
