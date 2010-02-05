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
#include "Decorator.h"

Decorator::Decorator()
:child(0){
}

Decorator::~Decorator(){
	delete child;
}

void Decorator::mouseMoveEvent(QMouseEvent *event){
	if (child)
		child->mouseMoveEvent(event);
}

void Decorator::mousePressEvent(QMouseEvent *event){
	if (child)
		child->mousePressEvent(event);
}

void Decorator::mouseReleaseEvent(QMouseEvent *event){
	if (child)
		child->mouseReleaseEvent(event);
}

void Decorator::mouseDoubleClickEvent(QMouseEvent *event){
	if (child)
		child->mouseDoubleClickEvent(event);
}

void Decorator::paintEvent(QPainter& painter){
	if (child)
		child->paintEvent(painter);
}

void Decorator::keyPressEvent(QKeyEvent *event){
	if (child)
		child->keyPressEvent(event);
}

void Decorator::keyReleaseEvent(QKeyEvent *event){
	if (child)
		child->keyReleaseEvent(event);
}

void Decorator::leaveEvent(QEvent *event){
	if (child)
		child->leaveEvent(event);
}

void Decorator::setDecorator(Decorator* newDecorator){
	delete child;
	child = newDecorator;
}

void Decorator::insertDecorator(Decorator* newDecorator){
	if (newDecorator){
		newDecorator->appendDecorator(child);
		child = newDecorator;
	}
}

void Decorator::appendDecorator(Decorator* newDecorator){
	if (child)
		child->appendDecorator(newDecorator);
	else
		child = newDecorator;
}
