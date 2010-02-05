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
#ifndef DOWNLOADER_H
#define DOWNLOADER_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QQueue>
#include <QHash>
#include <QFile>

class Downloader: public QObject{
Q_OBJECT
public:
	class Request{
	public:
		QString url;
		QString filename;
		Request(const QString& url, const QString& filename);
	};

	void addDownload(const QString& url, const QString& fileName);
	void setProxy(const QString& host, int port, const QString& userName = QString(), const QString& password = QString());
	Downloader();
	bool isLoadingUrl(const QString& url);
	bool isLoadingFile(const QString& filename);
signals:
	void downloadsFinished();
	void downloadStarted();
protected:
	enum {MaxDownloads = 8};

	QFile file;
	bool aborted;
	int lastRequestId;
	QQueue<Request> requests;
    QHash<QNetworkReply*, QString> replytofile;
	QNetworkAccessManager network;
	void nextRequest();
	bool inProcessing(const Request& request);
protected slots:
	void requestFinished(QNetworkReply * reply);
};

#endif
