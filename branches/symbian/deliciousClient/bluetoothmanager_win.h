#pragma once
#include "bluetoothmanager.h"
#include <vector>
#include <WinSock2.h>

class bluetoothdevice_win;

class bluetoothmanager_win :
    public bluetoothmanager
{
public:
    bluetoothmanager_win(void);

    bool IsUseable();
    bool IsDiscoverable();

    int Discover();
    int NumOfDevices();
    bluetoothdevice* GetDevice(int id);
    bluetoothdevice* GetLocalDevice();

    int SendToDevice(bluetoothdevice* device, void* data, size_t cb);
    RECVFUNC SetRecvCallback(RECVFUNC func);

    virtual ~bluetoothmanager_win(void);

private:
    std::vector<bluetoothdevice_win*> m_devices;
    RECVFUNC m_callback;
    HANDLE m_hReadThread;
    SOCKET m_listensock;
};
