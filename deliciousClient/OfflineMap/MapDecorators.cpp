/*
QGMView - Qt Google Map Viewer
Copyright (C) 2007  Victor Eremin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

You can contact author using following e-mail addreses
erv255@googlemail.com 
ErV2005@rambler.ru
*/
#include "MapDecorators.h"
#include <QMouseEvent> 
#include <QPainter>
#include "MapViewBase.h"
#include "GeoCoord.h"
#include <stdio.h>

MapDecorator::MapDecorator(MapViewBase* mapView)
:target(mapView){
}

MoveDecorator::MoveDecorator(MapViewBase* mapView)
:MapDecorator(mapView), oldX(0), oldY(0), dragging(false){
}

void MoveDecorator::mousePressEvent(QMouseEvent *event){
	if (event->buttons() & Qt::LeftButton){
		oldX = event->x();
		oldY = event->y();
		dragging = true;
	}
	MapDecorator::mousePressEvent(event);
}

void MoveDecorator::mouseReleaseEvent(QMouseEvent *event){
	if ((event->buttons() & Qt::LeftButton) == 0){
		dragging = false;
	}
	MapDecorator::mouseReleaseEvent(event);
}

void MoveDecorator::mouseMoveEvent(QMouseEvent *event){
	if (dragging){
		int deltaX = event->x() - oldX;
		int deltaY = event->y() - oldY;
		oldX = event->x();
		oldY = event->y();
		target->moveBy(deltaX, deltaY);
	}
	else
		MapDecorator::mouseMoveEvent(event);	
}

ZoomDecorator::ZoomDecorator(MapViewBase *mapView)
:MapDecorator(mapView){
}

void ZoomDecorator::mouseDoubleClickEvent(QMouseEvent* event){
	if (event->buttons() & Qt::LeftButton)
		target->zoomInAt(event->x(), event->y());
	if (event->buttons() & Qt::RightButton)
		target->zoomOutAt(event->x(), event->y());
}

void DownloadDecorator::mousePressEvent(QMouseEvent *event){
	if ((event->buttons() & Qt::LeftButton)&&(event->buttons() & Qt::RightButton))
		target->downloadMissingImages();
	MapDecorator::mousePressEvent(event);
}

DownloadDecorator::DownloadDecorator(MapViewBase *mapView)
:MapDecorator(mapView){
}

void CrossDecorator::paintEvent(QPainter &painter){
	if (isEnabled){
		QRect window = painter.window();
		painter.setPen(QColor(255, 0, 0));
		int halfWidth = (window.left()+window.right())/2;
		int halfHeight = (window.top() + window.bottom())/2;
		painter.drawLine(window.left(), halfHeight, window.right(), halfHeight);
		painter.drawLine(halfWidth, window.top(), halfWidth, window.bottom());
	}
	MapDecorator::paintEvent(painter);
}

CrossDecorator::CrossDecorator(MapViewBase* mapView)
:MapDecorator(mapView), isEnabled(true){
}

bool CrossDecorator::enabled(){
	return isEnabled;
}

void CrossDecorator::enable(bool newValue){
	if (isEnabled != newValue){
		isEnabled = newValue;
		emit stateChanged();
	}
}

CoordsDecorator::CoordsDecorator(MapViewBase* mapView)
:MapDecorator(mapView), isEnabled(true){
}

bool CoordsDecorator::enabled(){
	return isEnabled;
}

void CoordsDecorator::enable(bool value){
	if (isEnabled != value){
		isEnabled = value;
		emit stateChanged();
	}
}

void CoordsDecorator::paintEvent(QPainter &painter){
	if (isEnabled){
		QRect window = painter.window();
		painter.setPen(QColor(255, 0, 0));
		QString str;
		QPoint coords = target->getCoords();
        GeoCoord lat, lng;
        target->getGeoCoords(lat, lng);
        str += QString(tr("coordinates:\n    x: %1\n    y: %2\ngeo coordinates:\n   lat: %4\n   lng: %5\nzoom level: %3\n"))
			.arg(coords.x()).arg(coords.y())
			.arg(target->getZoomLevel())
            .arg(lat.getDouble())
            .arg(lng.getDouble());
			
		QRect rect(
			window.left() + 10, 
			window.top() + 10, 
			window.width() - 20, 
			window.height() - 20
		);
		
		painter.drawText(rect, str);
	}
	
	MapDecorator::paintEvent(painter);
}
