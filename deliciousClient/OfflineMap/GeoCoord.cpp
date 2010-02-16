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
#include "GeoCoord.h"
#include <QStringList>
#include <QRegExp>

qint16 GeoCoord::getDegrees() const{
	return data/degreesMultiplier;
}

quint8 GeoCoord::getMinutes() const{
	return (abs(data) % degreesMultiplier)/minutesMultiplier;
}

quint8 GeoCoord::getSeconds() const{
	return (abs(data) % minutesMultiplier)/secondsMultiplier;
}

quint8 GeoCoord::getFracSeconds() const{
	return abs(data) % secondsMultiplier;
}

void GeoCoord::setDegrees(const qint16 degrees){
	setValue(degrees, getMinutes(), getSeconds(), getFracSeconds());
}

void GeoCoord::setMinutes(const quint8 minutes){
	setValue(getDegrees(), minutes, getSeconds(), getFracSeconds());
}

void GeoCoord::setSeconds(const quint8 seconds){
	setValue(getDegrees(), getMinutes(), seconds, getFracSeconds());
}

void GeoCoord::setFracSeconds(const quint8 fracSeconds){
	setValue(getDegrees(), getMinutes(), getSeconds(), fracSeconds);
}

void GeoCoord::setValue(const qint16 degrees, const quint8 minutes, const quint8 seconds, const quint8 fracSeconds){
	data = abs(degrees)*degreesMultiplier + minutes*minutesMultiplier + seconds*secondsMultiplier + fracSeconds;
	if (degrees < 0) 
		data = - data;
}


qint32 GeoCoord::getValue() const{
	return data;
}

void GeoCoord::setValue(const qint32 value){
	data = value;
}

QString GeoCoord::getString(){
	return QString("%1 %2 %3.%4")
		.arg(getDegrees())
		.arg((int)getMinutes(), 2, 10, QChar('0'))
		.arg((int)getSeconds(), 2, 10, QChar('0'))
		.arg((int)getFracSeconds(), 2, 10, QChar('0'));
}

void GeoCoord::setString(const QString& str){
	QStringList strings = str.split(QRegExp("[\\. ]+"));
	if (strings.size() >= 4)
		setValue(strings[0].toInt(), strings[1].toInt(), strings[2].toInt(), strings[3].toInt());
}

GeoCoord::GeoCoord()
:data(0){
}

GeoCoord::GeoCoord(const qint16 degrees, const quint8 minutes, const quint8 seconds, const quint8 fracSeconds){
	setValue(degrees, minutes, seconds, fracSeconds);
}

GeoCoord::GeoCoord(const double degrees){
	setDouble(degrees);
}

double GeoCoord::getDouble() const{
	return (double)data*(double)maxDegrees/(double)maxValue;
}

void GeoCoord::setDouble(const double value){
	data = (quint32)(value*(double)maxValue/(double)maxDegrees);
}
