#ifndef DOWNLOADER_H
#define DOWNLOADER_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QSqlDatabase>
#include <QQueue>
#include <QHash>

class Downloader: public QObject{
Q_OBJECT
public:
    struct TileCoord
    {
        bool operator==(const TileCoord& other) const{ return x == other.x && y == other.y && zoom == other.zoom; }
        int x, y, zoom;
    };
	class Request{
	public:
		QString url;
		TileCoord tile;
		Request(const QString& url, int x, int y, int zoom);
	};

	void addDownload(const QString& url, int x, int y, int zoom);
	void setProxy(const QString& host, int port, const QString& userName = QString(), const QString& password = QString());
    void setDB(const QSqlDatabase& db) { this->db = db; }
	Downloader();
	bool isLoadingUrl(const QString& url) const;
    bool isLoadingTile(int x, int y, int zoom) const;
signals:
	void downloadsFinished();
	void downloadStarted();
protected:
	QSqlDatabase db;
	QQueue<Request> requests;
    QHash<QNetworkReply*, TileCoord> replytofile;
	QNetworkAccessManager network;
	void nextRequest();
	bool inProcessing(const Request& request);
protected slots:
	void requestFinished(QNetworkReply * reply);
};

#endif
