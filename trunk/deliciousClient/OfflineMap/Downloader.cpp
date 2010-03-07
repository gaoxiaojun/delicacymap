#include "Downloader.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSqlQuery>

bool operator== (const Downloader::Request& arg1, const Downloader::Request& arg2){
	return arg1.tile == arg2.tile;
}

Downloader::Downloader()
:network(this){
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
    TileCoord tile = replytofile[reply];
    if (reply->isFinished())
    {
        const QVariant &header = reply->header(QNetworkRequest::ContentTypeHeader);
        if (header.isValid() && header.toString().contains("image", Qt::CaseInsensitive))
        {
            QSqlQuery query;
            query.prepare(QString("INSERT INTO [%1] VALUES(:x, :y, :data)").arg(tile.zoom));
            query.bindValue(0, tile.x);
            query.bindValue(1, tile.y);
            query.bindValue(2, reply->readAll());
            query.exec();
        }
    }
    replytofile.remove(reply);
    reply->deleteLater();
}

void Downloader::addDownload(const QString& url, int x, int y, int zoom){
	Request request(url, x, y, zoom);
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

Downloader::Request::Request(const QString& _url, int x, int y, int zoom)
:url(_url)
{
    tile.x = x;
    tile.y = y;
    tile.zoom = zoom;
}

void Downloader::nextRequest(){
	while (!requests.empty()){  //为啥要用while循环呢...,还不如直接用if
		Request& cur = requests.head();
        QNetworkRequest request(cur.url);
		QNetworkReply* reply = network.get(request);
        replytofile.insert(reply, cur.tile);
        requests.dequeue();
		break;
	}
	
	if (requests.empty())
		emit downloadsFinished();
}

bool Downloader::isLoadingUrl(const QString& url) const
{
	for (int i = 0; i < requests.count(); i++){
		if (requests[i].url == url) 
            return true;
	}
	return false;
}

bool Downloader::isLoadingTile( int x, int y, int zoom ) const
{
    TileCoord requesttile = {x, y, zoom};
    for (int i = 0; i < requests.count(); i++){
        if (requests[i].tile == requesttile) 
            return true;
    }
    return false;
}