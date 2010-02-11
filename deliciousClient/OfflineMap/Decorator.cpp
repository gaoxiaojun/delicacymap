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
