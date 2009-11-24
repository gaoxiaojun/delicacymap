#include "QTProtobufChannel.h"
#include <google/protobuf/descriptor.h>

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

void QTProtobufChannel::CallMethod( const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller, const google::protobuf::Message* request, google::protobuf::Message* response, google::protobuf::Closure* done )
{
    if (!started())
        throw std::exception("Channel not started!");

    if (reqs.empty())
        needMoreReqs();
    protorpc::Message* reqholder = reqs.pop();
    reqholder->set_type(protorpc::REQUEST);
    reqholder->set_id(_callid);
    reqholder->set_name(method->name());
    reqholder->set_buffer(request->SerializeAsString());
    _currentCalls.insert(_callid++, CallEntry(done, response, reqholder));
    emit writeMessage(reqholder);
}

void QTProtobufChannel::run()
{
    _helper = new QTProtobufChannelDriver(this, &_currentCalls);

    connect(this, SIGNAL(writeMessage(google::protobuf::Message*)), _helper, SLOT(writeMessage( google::protobuf::Message*  )));
    connect(this, SIGNAL(requetStart(QHostAddress*, unsigned short )), _helper, SLOT(start(QHostAddress*, unsigned short)));
    connect(_helper->_tcps, SIGNAL(connected()), this, SIGNAL(connected()), Qt::DirectConnection);
    connect(_helper->_tcps, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(error()), Qt::DirectConnection);

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

QString QTProtobufChannel::errorString()
{
    if (_helper)
    {
        return _helper->_tcps->errorString();
    }
    return QString();
}

void QTProtobufChannelDriver::readMessage()
{
    // uses _buffer_index and _msgsize and _readbuffer
    // this does not use multiple waits, so the run time is predictable
    // which will help what will happen when the application terminates and destruct this object
    // the down side is, we might not be able to receive the whole msg in one call.
    // so _buffer_index indicates where to store the new income data.

    if (_buffer_index == -1 && _tcps->bytesAvailable() >= 4)
    {
        _tcps->read((char*)&_msgsize, 4);
        _readbuffer.reserve(_msgsize);
        _buffer_index = 0;
    }
    if (_buffer_index >= 0)
    {
        _buffer_index += _tcps->read((char*)_readbuffer.c_str() + _buffer_index, _msgsize - _buffer_index);
        if (_buffer_index == _msgsize)
        {
            //full msg received
            response.ParseFromArray(_readbuffer.c_str(), _msgsize);
            _buffer_index = -1;
            switch (response.type())
            {
            case protorpc::RESPONSE:
                if (_currentCalls->contains(response.id()))
                {
                    CallEntry entry = _currentCalls->value(response.id());
                    entry.response->ParseFromString(response.buffer());
                    parent->returnQueryBuffer(entry.request);
                    _currentCalls->remove(response.id());
                    entry.done->Run();
                }
                break;
            case protorpc::RESPONSE_FAILED:
                if (_currentCalls->contains(response.id()))
                {
                    CallEntry entry = _currentCalls->value(response.id());
                    entry.response->Clear();
                    parent->returnQueryBuffer(entry.request);
                    _currentCalls->remove(response.id());
                    entry.done->Run();
                }
                break;
            default:
                qDebug()<<"Unexpected RPC Response type: "<<response.type();
            }
        }
    }
}

QTProtobufChannelDriver::QTProtobufChannelDriver(QTProtobufChannel* parent, QHash<int,CallEntry> *currentCalls)
{
    this->parent = parent;
    this->_currentCalls = currentCalls;
    _buffer_index = -1;
    _tcps = new QTcpSocket();

    connect(_tcps, SIGNAL(readyRead()), this, SLOT(readMessage()), Qt::DirectConnection);
}

QTProtobufChannelDriver::~QTProtobufChannelDriver()
{
    //_tcps->abort();
    _tcps->close();
    delete _tcps;
}

void QTProtobufChannelDriver::writeMessage( google::protobuf::Message* m )
{
    int msgsize = m->ByteSize();
    char sizebuf[4];
    sizebuf[0] = (msgsize&0xFF000000) >> 24;
    sizebuf[1] = (msgsize&0x00FF0000) >> 16;
    sizebuf[2] = (msgsize&0x0000FF00) >> 8;
    sizebuf[3] = (msgsize&0x000000FF);

    if(_tcps->write(sizebuf,4) < 0)
    {
        throw _tcps->errorString().toUtf8().constData();
    }
    m->SerializeToString(&_writebuffer);
    if(_tcps->write(_writebuffer.c_str(), msgsize) < 0)
        throw _tcps->error();
}

void QTProtobufChannelDriver::start( QHostAddress *_addr, unsigned short _port )
{
    if (_tcps->state() == QTcpSocket::UnconnectedState)
    {
        _tcps->connectToHost(*_addr, _port);
    }
}

bool QTProtobufChannelDriver::started()
{
    return _tcps->state() == QTcpSocket::ConnectedState;
}

QAbstractSocket::SocketError QTProtobufChannelDriver::networkError() const
{
    return _tcps->error();
}
