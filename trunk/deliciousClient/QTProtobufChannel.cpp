#include "QTProtobufChannel.h"
#include <google/protobuf/descriptor.h>

QTProtobufChannel::QTProtobufChannel( const QHostAddress &serveraddr, unsigned short port )
:_addr(serveraddr), _port(port)
{
	_callid = 0;
	_reqholder.set_type(protorpc::REQUEST);
	_buffer_index = -1;
}
QTProtobufChannel::~QTProtobufChannel(void)
{
	close();
}

void QTProtobufChannel::start()
{
	if (_tcps.state() == QTcpSocket::UnconnectedState)
	{
		_tcps.connectToHost(_addr, _port);
		if (!_tcps.waitForConnected(3000))
		{
			throw _tcps.error();
		}
	}
	if (!this->isRunning())
	{
		QThread::start();
	}
}

void QTProtobufChannel::close()
{
	_tcps.disconnect();
	wait();
}

void QTProtobufChannel::CallMethod( const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller, const google::protobuf::Message* request, google::protobuf::Message* response, google::protobuf::Closure* done )
{
	start();

	_reqholder.set_id(_callid);
	_reqholder.set_name(method->name());
	_reqholder.set_buffer(request->SerializeAsString());
	_currentCalls.insert(_callid++, CallEntry(done, response));
	writeMessage(&_reqholder);
}

void QTProtobufChannel::run()
{
	// prequist: socket is connected
	protorpc::Message response;
	while (_tcps.state() == QTcpSocket::ConnectedState)
	{
		if (readMessage(&response))
		{
			switch (response.type())
			{
			case protorpc::RESPONSE:
				if (_currentCalls.contains(response.id()))
				{
					CallEntry entry = _currentCalls.value(response.id());
					entry.response->ParseFromString(response.buffer());
					_currentCalls.remove(response.id());
					entry.done->Run();
				}
				break;
			default:
				qDebug()<<"Client RPC Only expect Response!";
			}
		}
	}
}

void QTProtobufChannel::writeMessage( google::protobuf::Message* m )
{
	int msgsize = m->ByteSize();
	char sizebuf[4];
	sizebuf[0] = (msgsize&0xFF000000) >> 24;
	sizebuf[1] = (msgsize&0x00FF0000) >> 16;
	sizebuf[2] = (msgsize&0x0000FF00) >> 8;
	sizebuf[3] = (msgsize&0x000000FF);

	if(_tcps.write(sizebuf,4) < 0)
	{
		throw _tcps.errorString().toUtf8().constData();
	}
	m->SerializeToString(&_writebuffer);
	if(_tcps.write(_writebuffer.c_str(), msgsize) < 0)
		throw _tcps.error();
}

bool QTProtobufChannel::readMessage( google::protobuf::Message* m )
{
	// uses _buffer_index and _msgsize and _readbuffer
	// this does not use multiple waits, so the run time is predictable
	// which will help what will happen when the application terminates and destruct this object
	// the down side is, we might not be able to receive the whole msg in one call.
	// so _buffer_index indicates where to store the new income data.
	if (_tcps.waitForReadyRead(500))
	{
		if (_buffer_index == -1 && _tcps.bytesAvailable() >= 4)
		{
			_tcps.read((char*)&_msgsize, 4);
			_readbuffer.reserve(_msgsize);
			_buffer_index = 0;
		}
		if (_buffer_index >= 0)
		{
			_buffer_index += _tcps.read((char*)_readbuffer.c_str() + _buffer_index, _msgsize - _buffer_index);
			if (_buffer_index == _msgsize)
			{
				//full msg received
				m->ParseFromArray(_readbuffer.c_str(), _msgsize);
				_buffer_index = -1;
				return true;
			}
		}
	}
	return false;
}
