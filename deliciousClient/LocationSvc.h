#pragma once

#include "../rclib/include/rclib/util.h"

//struct def for time
struct DateTime
{
    int year, month, day, 
        hour, minute, second,
        millisecond;
};

class Location
{
public:
    virtual double Latitude() const=0;
    virtual double Longtitude() const=0;
    virtual DateTime Time() const=0;
};

class LocationSvc
{
public:
    typedef void (*CallBackFunc)( void* context, LocationSvc* );

    static LocationSvc* create();

    LocationSvc(){};

    virtual Location* const getLocalLocation()=0;

public:
    virtual bool canGetLocalLocation()=0;

    virtual ~LocationSvc(void){}

    virtual bool registerCallbackForNewPosition(CallBackFunc func, void* context) = 0;
    virtual bool removeCallbackFroNewPosition(CallBackFunc func, void* context) = 0;
    virtual void clearCallbacks() = 0;


    DISABLE_COPY_SEMANTIC( LocationSvc );
};
