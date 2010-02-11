#ifndef DECORATOR_H
#define DECORATOR_H

class QMouseEvent;
class QEvent;
class QKeyEvent;
class QPaintEvent;
class QPainter;

class Decorator{
public:
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent *event);
	virtual void paintEvent(QPainter& painter);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);
	virtual void leaveEvent(QEvent *event);
	void setDecorator(Decorator* decorator = 0);
	void insertDecorator(Decorator* decorator);
	void appendDecorator(Decorator* decorator);
	Decorator();
	virtual ~Decorator();
protected:
	Decorator* child;
};
#endif
