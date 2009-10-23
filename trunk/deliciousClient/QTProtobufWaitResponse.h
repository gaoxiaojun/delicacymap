#pragma once
#include <QMutex>
#include <QWaitCondition>
#include <google/protobuf/service.h>

class QTProtobufWaitResponse
{
public:
	QTProtobufWaitResponse(void);
	~QTProtobufWaitResponse(void);

	google::protobuf::Closure *getClosure();
	void wait();
private:
	void done();
	QWaitCondition _wait;
	QMutex _mutex;
};
