/*
 *  QTProtobufChannelDriver.cpp
 *  delicacyClient
 *
 *  Created by R.C on 10-1-25.
 *
 */

#include "QTProtobufChannelDriver.h"

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
        throw _tcps->errorString().toUtf8().constData();
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
