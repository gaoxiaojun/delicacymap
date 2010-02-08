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
#include "Downloader.h"
#include <QUrl>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>

bool operator== (const Downloader::Request& arg1, const Downloader::Request& arg2){
	return (arg1.url == arg2.url) && (arg1.filename == arg2.filename);
}

Downloader::Downloader()
:aborted(false), lastRequestId(0), network(this){
    connect(&network, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
}

void Downloader::setProxy(const QString& host, int port, const QString& userName, const QString& password){
	//FIXME: switching proxy tested. better switch to Qt 4.3
// 	if (host.size() == 0){
// 		http->abort();
// 		requests.clear();
// 		delete http;
// 		http = new QHttp(this);
// 	}		
// 	else
// 		http->setProxy(host, port, userName, password);
}

void Downloader::requestFinished(QNetworkReply * reply){
// 	if (lastRequestId == id){
//         reply->readAll();
// 		printf("request finished (%d, %d)\n", id, status);
// 		file.close();
// 		if (!status){
// 			printf("warning! request was not sucessfull.\n");
// 			QHttpResponseHeader response = http->lastResponse();
// 			if (response.hasContentType()){
// 				if (!response.contentType().contains("image", Qt::CaseInsensitive))
// 					file.remove();
// 			}
// 			else
// 				if (file.size()==0)
// 					file.remove();
// 		}
// 		if (status)
// 			emit successfullDownload();
// 		if (!aborted){
// 			if (!requests.empty())
// 				requests.dequeue();
// 			nextRequest();
// 		}
// 	}
    QString& filename = replytofile[reply];
    if (reply->isFinished())
    {
        QVariant &header = reply->header(QNetworkRequest::ContentTypeHeader);
        if (header.isValid() && header.toString().contains("image", Qt::CaseInsensitive))
        {
            QFile output(filename);
            if (!output.open(QIODevice::WriteOnly))
            {
     			qDebug()<<"couldn't open file "<<filename;
            }
            output.write(reply->readAll());
        }
    }
    replytofile.remove(reply);
    reply->deleteLater();
}

void Downloader::addDownload(const QString& url, const QString& filename){
	qDebug()<<"addDownload("<<url<<", "<<filename;
	aborted = false;
	Request request(url, filename);
	if (!inProcessing(request)){
		requests.enqueue(request);
		if (requests.count() == 1){
			emit downloadStarted();
			nextRequest();
		}
	}
}

bool Downloader::inProcessing(const Downloader::Request& request){
	for (int i = 0; i < requests.count(); i++){
		if (requests[i] == request) return 
			true;
	}
	return false;
}

Downloader::Request::Request(const QString& _url, const QString& _filename)
:url(_url), filename(_filename){
}

void Downloader::nextRequest(){
	qDebug()<<"next request\n";
	while (!requests.empty()){
		Request& cur = requests.head();
//		file.setFileName(cur.filename);
        QNetworkRequest request(cur.url);
		QNetworkReply* reply = network.get(request);
        replytofile.insert(reply, cur.filename);
        requests.dequeue();
// 		if (!file.open(QIODevice::WriteOnly)){
// 			qDebug<<"couldn't open file "<<cur.filename;
// 			//cur.callback->execute(false);
// 			requests.dequeue();
// 			continue;
// 		}
		
// 		printf("file to retrieve: %s\n", (const char*)(cur.url.toAscii()));
// 		lastRequestId = http->get(cur.url, &file);
		break;
	}
	
	if (requests.empty())
		emit downloadsFinished();
}

bool Downloader::isLoadingUrl(const QString& url){
	for (int i = 0; i < requests.count(); i++){
		if (requests[i].url == url) return 
			true;
	}
	return false;
}

bool Downloader::isLoadingFile(const QString& filename){
	for (int i = 0; i < requests.count(); i++){
		if (requests[i].filename == filename) return 
			true;
	}
	return false;
}
