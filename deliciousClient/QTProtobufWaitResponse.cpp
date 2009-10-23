#include "QTProtobufWaitResponse.h"

QTProtobufWaitResponse::QTProtobufWaitResponse(void)
{
}

QTProtobufWaitResponse::~QTProtobufWaitResponse(void)
{
}

void QTProtobufWaitResponse::wait()
{
	QMutexLocker lock(&_mutex);
	bool ret = _wait.wait(&_mutex);
}

google::protobuf::Closure * QTProtobufWaitResponse::getClosure()
{
	return google::protobuf::NewCallback(this, &QTProtobufWaitResponse::done);
}

void QTProtobufWaitResponse::done()
{
	QMutexLocker lock(&_mutex);
	_wait.wakeAll();
}
