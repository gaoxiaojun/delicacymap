#include "bluetoothmanager_win.h"
#include "bluetoothmanager_win_common.h"
#include <cassert>

bluetoothmanager_win::bluetoothmanager_win( void )
:m_callback(NULL)
{

}

bluetoothmanager_win::~bluetoothmanager_win( void )
{

}

bool bluetoothmanager_win::IsUseable()
{
    return false;
}

bool bluetoothmanager_win::IsDiscoverable()
{
    return false;
}

int bluetoothmanager_win::Discover()
{
    return NumOfDevices();
}

int bluetoothmanager_win::NumOfDevices()
{
    return m_devices.size();
}

bluetoothdevice* bluetoothmanager_win::GetDevice( int id )
{
    assert(id >= 0);
    return id >= NumOfDevices() ? NULL : m_devices[id];
}

bluetoothdevice* bluetoothmanager_win::GetLocalDevice()
{
    return NULL;
}

int bluetoothmanager_win::SendToDevice( bluetoothdevice* device, void* data, size_t cb )
{
    return 0;
}

RECVFUNC bluetoothmanager_win::SetRecvCallback( RECVFUNC func )
{
    std::swap(func, m_callback);
    return func;
}
