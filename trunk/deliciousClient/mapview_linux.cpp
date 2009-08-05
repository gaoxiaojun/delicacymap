//////////////////////////////////Platform Dependent Methods-Linux///////////////////////////////////////
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

