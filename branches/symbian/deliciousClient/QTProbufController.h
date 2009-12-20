#ifndef __QTPROBUFCONTROLLER__H__INCLUDED__
#define __QTPROBUFCONTROLLER__H__INCLUDED__

#include <google/protobuf/service.h>

class QTProbufController :
	public google::protobuf::RpcController
{
public:
	QTProbufController(void);
	~QTProbufController(void);

	//client side
	std::string ErrorText() const;
	bool Failed() const;
	void StartCancel();
	void Reset();
	//server side
	void SetFailed(const std::string &reason);
	bool IsCanceled() const;
	void NotifyOnCancel(google::protobuf::Closure *cb);
	void startCancel();
};

#endif
