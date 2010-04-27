/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QGEOAREAMONITOR_H
#define QGEOAREAMONITOR_H

#include "qmobilityglobal.h"
#include "qgeocoordinate.h"

#include <QObject>

QT_BEGIN_HEADER

#ifdef Q_QDOC
//normally we would use macro only but this causes some other compilation issues
QTM_BEGIN_NAMESPACE
#else
namespace QtMobility
{
#endif


class QGeoPositionInfo;
class QGeoAreaMonitorPrivate;
class Q_LOCATION_EXPORT QGeoAreaMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)

public:
    explicit QGeoAreaMonitor(QObject *parent);
    virtual ~QGeoAreaMonitor() = 0;

    virtual void setCenter(const QGeoCoordinate &coordinate);
    QGeoCoordinate center() const;

    virtual void setRadius(qreal radius);
    qreal radius() const;

    static QGeoAreaMonitor *createDefaultMonitor(QObject *parent);

Q_SIGNALS:
    void areaEntered(const QGeoPositionInfo &update);
    void areaExited(const QGeoPositionInfo &update);

private:
    Q_DISABLE_COPY(QGeoAreaMonitor)
    QGeoAreaMonitorPrivate *d;
};

}

QT_END_HEADER

#endif
