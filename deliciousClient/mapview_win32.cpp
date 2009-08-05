//////////////////////////////////Platform Dependent Methods-Win32///////////////////////////////////////
#ifdef WIN32

#include "mapview.h"
#include <QWebFrame>

void mapview::setupmapconfiguration()
{
    page()->mainFrame()->evaluateJavaScript("map.enableDoubleClickZoom();"
                                            "map.enableContinuousZoom();"
                                            "map.enableScrollWheelZoom();"
                                            "map.disableGoogleBar();"
                                            );
}

#endif