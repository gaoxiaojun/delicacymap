#pragma once

#include "../rclib/include/rclib/util.h"
#include <functional>
#include <string>

class bluetoothdevice
{
public:
    virtual const std::string& Name() const = 0;
    virtual ~bluetoothdevice(){}
};

typedef std::pointer_to_unary_function<void*, void> RECVFUNC;

class bluetoothmanager
{
public:
    bluetoothmanager();

    static bluetoothmanager& GetInstance();
    static void Destroy();

    virtual bool IsUseable(){return false;}
    virtual bool IsDiscoverable(){return false;}

    virtual int Discover()=0;
    virtual int NumOfDevices()=0;
    virtual bluetoothdevice* GetDevice(int id)=0;
    virtual bluetoothdevice* GetLocalDevice()=0;

    bluetoothdevice* operator[](int id){return GetDevice(id);}

    virtual int SendToDevice(bluetoothdevice* device, void* data, size_t cb)=0;
    virtual RECVFUNC SetRecvCallback(RECVFUNC func)=0;

    virtual ~bluetoothmanager(){};
private://data member
    static bluetoothmanager* volatile ms_bt;


    DISABLE_COPY_SEMANTIC( bluetoothmanager );
};
