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
#ifndef MAP_DECORATORS_H
#define MAP_DECORATORS_H
#include "Decorator.h"
#include <QObject>
#include <QPointF>
#include <QPoint>

class MapViewBase;

class MapDecorator: public Decorator{
public:
	MapDecorator(MapViewBase* mapView);
protected:
	MapViewBase *target;
};

class MoveDecorator: public QObject, public MapDecorator{
    Q_OBJECT
public:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
    void timerEvent(QTimerEvent *e);
	MoveDecorator(MapViewBase* mapView);
protected:
    QPointF m_speed;
	QPoint m_mouse_pos;
	bool dragging;
};

class ZoomDecorator: public MapDecorator{
public:
	void mouseDoubleClickEvent(QMouseEvent *event);
	ZoomDecorator(MapViewBase* mapView);
};

class DownloadDecorator: public MapDecorator{
public:
	void mousePressEvent(QMouseEvent *event);
	DownloadDecorator(MapViewBase* mapView);
public:
};

class CrossDecorator: public QObject, public MapDecorator{
Q_OBJECT
protected:
	bool isEnabled;
public:
	bool enabled();
	void paintEvent(QPainter &painter);
	CrossDecorator(MapViewBase* mapView);
public slots:
	void enable(bool enabled);
signals:
	void stateChanged();
};

class CoordsDecorator: public QObject, public MapDecorator{
Q_OBJECT
protected:
	bool isEnabled;
public:
	bool enabled();
	void paintEvent(QPainter &painter);
	CoordsDecorator(MapViewBase* mapView);
public slots:
	void enable(bool enable);
signals:
	void stateChanged();
};

#endif
