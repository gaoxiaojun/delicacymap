#pragma once
#include "google/protobuf/service.h"
#include <string>
#include <vector>

class naked_conn;

class AsioRpcController
    : public google::protobuf::RpcController
{
public://constructor, destructor
    AsioRpcController();
    ~AsioRpcController();


    //////////////////////////////RpcController impl////////////////////////////////////////////
public:
    //client side

    virtual string ErrorText() const;

    virtual void Reset();

    virtual bool Failed() const;

    virtual void StartCancel();

    //server side

    virtual void SetFailed(const string& reason);

    virtual bool IsCanceled() const;

    virtual void NotifyOnCancel(google::protobuf::Closure* callback);

private://fields
    std::vector<google::protobuf::Closure*> oncancel;
    std::string reason;
    bool hasFailed;
    bool canceled;
};
