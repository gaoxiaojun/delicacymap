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
#include "ImageCache.h"
#include <QFile>
#include <QColor>
#include <QPainter>

ImageCache::~ImageCache(){
}

void ImageCache::setDownloader(Downloader* newDownloader){
	downloader = newDownloader;
}

void ImageCache::setCacheDir(const QString& newCacheDir){
	if (cacheDir != newCacheDir){
		cacheDir = newCacheDir;
		if (cacheDir.size())
			if (cacheDir[cacheDir.size()-1] != QChar('/'))
				cacheDir.append("/");
		clear();
		emit imageChanged();
	}
}

const QString& ImageCache::getCacheDir(){
	return cacheDir;
}

ImageCache::ImageCache()
:loadingImg(256, 256, QImage::Format_RGB32), downloader(0),
urlTemplate("http://mt2.google.com/vt/lyrs=m@116&hl=en&x=%1&y=%2&z=%3"),
filenameTemplate("x%1y%2z%3.png"){
	paintLoadingImage();	
}

QImage* ImageCache::getImage(int x, int y, int zoom){
	TileCoord tileCoord(x, y, zoom);
	if (isLoading(tileCoord))
		return &loadingImg;
	
	if (!images.contains(tileCoord))
		loadImage(tileCoord);
	
	if (images.contains(tileCoord)){
		Tile& tile = images[tileCoord];
		tile.revive();
		return &tile.image;
	}
	return 0;
}

ImageCache::TileCoord::TileCoord()
:x(0), y(0), zoom(0){
}

ImageCache::TileCoord::TileCoord(const int _x, const int _y, const int _zoom)
:x(_x), y(_y), zoom(_zoom){
}

ImageCache::TileCoord::TileCoord(const TileCoord& tc)
:x(tc.x), y(tc.y), zoom(tc.zoom){
}

bool operator< (const ImageCache::TileCoord& arg1, const ImageCache::TileCoord arg2){
	if (arg1.zoom < arg2.zoom) return true;
	if (arg1.y < arg2.y) return true;
	if (arg1.x < arg2.x) return true;
	return false;
}

ImageCache::Tile::Tile()
:life(DefaultLife){
}

ImageCache::Tile::Tile(const QImage& _image)
:image(_image), life(DefaultLife){
}

void ImageCache::loadImage(const TileCoord& tileCoord){
	QString filename = getCacheName(tileCoord.x, tileCoord.y, tileCoord.zoom);
	if (QFile::exists(filename)){
		images[tileCoord] = Tile(QImage());
		if (!images[tileCoord].image.load(filename)){
			if (!isLoading(tileCoord))
				QFile::remove(filename);//should work with broken files
			images.remove(tileCoord);
		}
	}
}

void ImageCache::downloadImage(int x, int y, int zoom){
	//setLoadingState(x, y, zoom);
	if (downloader)
		downloader->addDownload(
			getDownloadUrl(x, y, zoom),
			getCacheName(x, y, zoom)
		);
	else
		printf("no downloader!\n");
}

void ImageCache::setFileNameTemplate(const QString& newFileNameTemplate){
	if (filenameTemplate != newFileNameTemplate){
		filenameTemplate = newFileNameTemplate;
		clear();
		emit imageChanged();
	}
}

void ImageCache::setUrlTemplate(const QString &newUrlTemplate){
	if (urlTemplate != newUrlTemplate){
		urlTemplate = newUrlTemplate;
		clear();
		emit imageChanged();
	}
}

//TODO: maybe need better templateConstruction routine....
QString ImageCache::getDownloadUrl(int x, int y, int zoom){
	return QString(urlTemplate)
		.replace("%1", QString::number(x))
		.replace("%2", QString::number(y))
		.replace("%3", QString::number(zoom))
		.replace("%4", getCoordsQstr(x, y, zoom));
}

QString ImageCache::getCacheName(int x, int y, int zoom){
	return QString("%1%2").arg(cacheDir)
		.arg(makeFileName(x, y, 17-zoom));
}

QString ImageCache::makeFileName(int x, int y, int zoom){
	return QString("%3/%4/%1/%5/%2.png")
		.replace("%1", QString::number(x%1024))
		.replace("%2", QString::number(y%1024))
		.replace("%3", QString::number(zoom))
		//.replace("%4", getCoordsQstr(x, y, zoom))
        .replace("%4", QString::number(x/1024))
        .replace("%5", QString::number(y/1024));
//     return QString("x%1y%2z%3.png")
//         .arg(x)
//         .arg(y)
//         .arg(zoom);
}

void ImageCache::paintLoadingImage(){
	QPainter painter(&loadingImg);
	painter.setBrush(QColor(0, 0, 0));
	painter.setPen(QColor(255, 255, 255));
	painter.drawRect(0, 0, 255, 255);
	painter.drawText(0, 0, 255, 255, Qt::AlignHCenter|Qt::AlignVCenter,	QString(tr("Downloading...")));
}

void ImageCache::update(){
	for (QMap<TileCoord, Tile>::iterator i = images.begin(); i != images.end();){
		QMap<TileCoord, Tile>::iterator cur = i; i++;
		cur->countdown();
		if (cur->getLife() <= 0)
			images.erase(cur);
	}
}

bool ImageCache::isLoading(const TileCoord& tileCoord){
	if (downloader){
		QString str = getCacheName(tileCoord.x, tileCoord.y, tileCoord.zoom);
		return (downloader->isLoadingFile(str));
	}
	return false;
}

void ImageCache::tryDownload(int x, int y, int zoom){
	printf("try download(%d, %d, %d)\n", x, y, zoom);
	QString filename = getCacheName(x, y, zoom);
	if (!QFile::exists(filename)){
		if (!isLoading(TileCoord(x, y, zoom))){
			downloadImage(x, y, zoom);
		}
		else
			printf("file is loading\n");
	}
	else
		printf("file exists\n");		
}

void ImageCache::clear(){
	images.clear();
}

QString ImageCache::getCoordsQstr(int x, int y, int zoom){
	static const char table[2][2] = {
		{'q', 'r'},
		{'t', 's'}
	};
	QString tmp("");
	for (;zoom > 0; zoom--){
		tmp.prepend(table[y&1][x&1]);
		x = x>>1;
		y = y>>1;
	}
	return tmp;
}
