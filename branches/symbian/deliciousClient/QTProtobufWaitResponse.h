#ifndef __QTPROTOBUFWAITRESPONSE__H__INCLUDED__
#define __QTPROTOBUFWAITRESPONSE__H__INCLUDED__

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

#endif
