#include "LocationSvc.h"
#include <stddef.h>

#ifdef _WIN32_WCE
#include "GPSLocationSvc_wince.h"
#endif

LocationSvc* LocationSvc::create()
{
#ifdef _WIN32_WCE
    return new GPSLocationSvc_wince;
#endif

    return NULL;
}
