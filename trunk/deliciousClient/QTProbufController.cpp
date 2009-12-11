#include "QTProbufController.h"

QTProbufController::QTProbufController(void)
{
}

QTProbufController::~QTProbufController(void)
{
}

std::string QTProbufController::ErrorText() const
{
	return "";
}

bool QTProbufController::Failed() const
{
	return false;
}

void QTProbufController::Reset()
{

}

void QTProbufController::StartCancel()
{

}

void QTProbufController::SetFailed( const std::string &reason )
{

}

bool QTProbufController::IsCanceled() const
{
	return false;
}

void QTProbufController::NotifyOnCancel( google::protobuf::Closure *cb )
{

}

void QTProbufController::startCancel()
{

}
