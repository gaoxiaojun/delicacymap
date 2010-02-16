#pragma once

#include "GeoCoord.h"
#include <QPoint>
#include <math.h>

class CoordsHelper
{
public:
    enum {MaxZoomLevel = 17, TilePower2 = 8, TileSize=256};

    static const GeoCoord maxLatitude;//(90, 0, 0, 0);//(85.0511287798066);//(90, 0, 0, 0);
    static const GeoCoord maxLongitude;//(180, 0, 0, 0);
    static const double pi;// = 3.1415926535897932384626433832795028841971693993751;

    static int fitToPow2(int num, int power){
        int val = 1 << power;
        int mask = val - 1;
        if (num < 0)
            num = val - ((-num)&mask);		
        return num & mask;
    }

    static int remapToPow2(int num, int oldPow, int newPow){
        return (oldPow > newPow) ? 
            num >> (oldPow - newPow):
            num << (newPow - oldPow); 
    } 

    static QPoint InternalGeoCoordToCoord( const GeoCoord& latitude, const GeoCoord& longitude, int zoomlevel = MaxZoomLevel )
    {
        QPoint coords;

        //Mercator projection (this is the one used by Google)
        qint32 halfSize = (1 << (MaxZoomLevel + TilePower2 - 1));
        double x = (double)longitude.getValue()/(double)maxLongitude.getValue();
        double y = (double)latitude.getValue()/(double)maxLatitude.getValue();

        x = x * halfSize + halfSize;
        y = y * pi/2.0;
        y = log(tan(y) + 1.0/cos(y));
        y = y/(pi);

        y = halfSize - y * halfSize;

        coords.setX(remapToPow2(x, MaxZoomLevel, zoomlevel));
        coords.setY(remapToPow2(y, MaxZoomLevel, zoomlevel));
        return coords;
    }

    static void InternalCoordToGeoCoord( QPoint coord, int zoomLevel, GeoCoord &latitude, GeoCoord &longitude )
    {
        qint32 halfSize = (1 << (zoomLevel + TilePower2 - 1));
        double x = (double)coord.x() / (double)halfSize - 1;
        double y = 1- (double)coord.y() / (double)halfSize;
        longitude.setDouble(x * 180.0);	
        y *= pi;
        y = 2.0 * atan(exp(y)) - pi/2.0;
        latitude.setDouble(y * 180 / pi);
    }
};
