#include "stdafx.h"
#include "AsioRpcController.h"
#include <boost/foreach.hpp>

AsioRpcController::AsioRpcController()
{
    Reset();
}

AsioRpcController::~AsioRpcController(void)
{
}

void AsioRpcController::Reset()
{
    reason.clear();
    hasFailed=false;
    canceled=false;
    oncancel.clear();
}

bool AsioRpcController::Failed() const
{
    return hasFailed;
}

std::string AsioRpcController::ErrorText() const
{
    return reason;
}

void AsioRpcController::StartCancel()
{
    canceled = true;
    BOOST_FOREACH(google::protobuf::Closure* c, oncancel)
        c->Run();
}

void AsioRpcController::SetFailed( const std::string& reason )
{
    this->reason = reason;
    hasFailed = true;
}

bool AsioRpcController::IsCanceled() const
{
    return canceled;
}

void AsioRpcController::NotifyOnCancel( google::protobuf::Closure* callback )
{
    oncancel.push_back(callback);
}
