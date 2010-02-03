#include <stddef.h>
#include <stdlib.h>

#include "bluetoothmanager.h"

#ifdef _WIN32_WCE
    #include "bluetoothmanager_wince.h"
#elif defined( WIN32 )
    #include "bluetoothmanager_win.h"
#endif

bluetoothmanager* volatile bluetoothmanager::ms_bt = NULL;

void bluetoothmanager::Destroy()
{
    delete ms_bt;
    ms_bt = NULL;
}

bluetoothmanager::bluetoothmanager()
{
    atexit(&bluetoothmanager::Destroy);
}

//ill implementation of singleton
//suitable only for single threaded access to this objects
bluetoothmanager& bluetoothmanager::GetInstance()
{
    if (ms_bt == NULL)
    {
#ifdef _WIN32_WCE
#if _WIN32_WCE >= 0x500 && _WIN32_WCE < 0x600
        //Window Mobile
        ms_bt = new bluetoothmanager_wince;
#endif
#elif defined( WIN32 )
        ms_bt = new bluetoothmanager_win;
#endif
    }
    return *ms_bt;
}
