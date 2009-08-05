#pragma once
#include "locationsvc.h"

#include <Winbase.h>
#include <Gpsapi.h>
#include <vector>


class GPSLocationSvc_wince :
    public LocationSvc
{
public:
    GPSLocationSvc_wince(void);
    ~GPSLocationSvc_wince(void);

    virtual Location* const getLocalLocation();
    virtual bool canGetLocalLocation();

    virtual bool registerCallbackForNewPosition(CallBackFunc func, void* context);
    virtual bool removeCallbackFroNewPosition(CallBackFunc func, void* context);
    virtual void clearCallbacks();

private:
    typedef std::pair<CallBackFunc, void*> CallBackPair;
    HANDLE newpositionavail;//, devicestatechange;
    HANDLE gpsdevice;
    HANDLE newposthread;
    bool termflag;

    std::vector<CallBackPair> callbackfuncs;

    friend DWORD waitfornewposition(void* env);
};
