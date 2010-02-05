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
#ifndef IMAGE_CACHE_H
#define IMAGE_CACHE_H
#include <QObject>
#include <QString>
#include <QMap>
#include <QImage>
#include <QHttp>
#include <QFile>
#include <QQueue>
#include "Downloader.h"

class ImageCache: public QObject{
Q_OBJECT
public:
	struct TileCoord{
		int x;
		int y;
		int zoom;
		TileCoord();
		TileCoord(const int x, const int y, const int zoom);
		TileCoord(const TileCoord& tc);
	};
	
	class Tile{
	public:
		enum {DefaultLife=16};
		inline int getLife(){return life;};
		inline void revive(){life = DefaultLife;};
		inline int countdown(){life--; return life;};
		QImage image;
		Tile();
		Tile(const QImage& image);
	protected:
		int life;
	};
	
	QImage* getImage(int x, int y, int zoom);
	void downloadImage(int x, int y, int zoom);
	void setDownloader(Downloader* newDownloader);
	void tryDownload(int x, int y, int zoom);
	void setCacheDir(const QString& cacheDir);
	const QString& getCacheDir();
	ImageCache();
	virtual ~ImageCache();
	void update();
	void clear();
	const QString& getFileNameTemplate();
	const QString& getUrlTemplate();
	
	void setFileNameTemplate(const QString& newFileNameTemplate);
	void setUrlTemplate(const QString &newUrlTemplate);
signals:
	void imageChanged();
protected:
	QMap<TileCoord, Tile> images;
	
	QString cacheDir;
	QImage errorImg;
	QImage loadingImg;
	Downloader* downloader;
	void loadImage(const TileCoord& tileCoord);
	QString getDownloadUrl(int x, int y, int zoom);
	QString getCacheName(int x, int y, int zoom);
	QString getCoordsQstr(int x, int y, int zoom);
	QString makeFileName(int x, int y, int zoom);
	
	QString urlTemplate;
	QString filenameTemplate;
	
	void paintLoadingImage();
	bool isLoading(const TileCoord& tileCoord);
};

bool operator< (const ImageCache::TileCoord& arg1, const ImageCache::TileCoord arg2);

#endif
