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
#ifndef GEO_COORD_H
#define GEO_COORD_H

#include <QString>

class GeoCoord{
public:
	qint16 getDegrees() const;
	quint8 getMinutes() const;
	quint8 getSeconds() const;
	quint8 getFracSeconds() const;
	void setDegrees(const qint16 value);
	void setMinutes(const quint8 value);
	void setSeconds(const quint8 value);
	void setFracSeconds(const quint8 value);
	double getDouble() const;
	void setDouble(const double value);
	qint32 getValue() const;
	void setValue(const qint32 value);
	void setValue(const qint16 degrees, const quint8 minutes, const quint8 seconds, const quint8 fracSeconds);
	QString getString();
	void setString(const QString& str);
	GeoCoord();
	GeoCoord(const qint16 degrees, const quint8 minutes, const quint8 seconds, const quint8 fracSeconds);
	GeoCoord(const double degrees);
protected:
	enum {
		secondsMultiplier = 100, minutesMultiplier = secondsMultiplier*60, 
		degreesMultiplier = minutesMultiplier*60, maxDegrees = 180, maxValue=degreesMultiplier*maxDegrees
	};
	qint32 data;
};

#endif
