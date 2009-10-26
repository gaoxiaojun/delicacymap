//////////////////////////////////Platform Dependent Methods-Win32///////////////////////////////////////
#ifdef _WIN32_WCE

#include "mapview.h"
#include <QWebFrame>

void mapview::setupmapconfiguration()
{
    page()->mainFrame()->evaluateJavaScript("map.disableDoubleClickZoom();"
                                            "map.disableContinuousZoom();"
                                            "map.disableScrollWheelZoom();"
                                            "map.disableGoogleBar();"
                                            );
	setZoomFactor(2);
	resize(this->width(), this->height());
}

#endif
