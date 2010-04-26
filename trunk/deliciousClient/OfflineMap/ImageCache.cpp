#include "ImageCache.h"
#include "Downloader.h"
#include <QColor>
#include <QVariant>
#include <QPainter>
#include <QTimer>
#include <stdlib.h>

ImageCache::~ImageCache(){
}

void ImageCache::setDownloader(Downloader* newDownloader){
	downloader = newDownloader;
    downloader->setDB(db);
}

void ImageCache::setCacheDBPath(const QString &newCachePath)
{
    if (newCachePath != dbpath)
    {
        dbpath = newCachePath;
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbpath);
        db.open();
        downloader->setDB(db);
        asyncLoader.setDB(&db);
    }
}

const QString& ImageCache::getCacheDBPath()
{
    return dbpath;
}

ImageCache::ImageCache()
:downloader(0),
loadingImg(256, 256),
urlTemplate("http://mt2.google.com/vt/lyrs=m@116&hl=zh&x=%1&y=%2&z=%3")
{
    ticks = 0;
    this->paintLoadingImage();
    connect(this, SIGNAL(needLoadImage(int,int,int)), &asyncLoader, SLOT(loadImage(int,int,int)), Qt::QueuedConnection);
    connect(&asyncLoader, SIGNAL(imageLoaded(int,int,int,QPixmap*)), this, SLOT(imageLoaded(int,int,int,QPixmap*)), Qt::QueuedConnection);
}

QPixmap* ImageCache::getImage(int x, int y, int zoom){
    TileCoord tileCoord(x, y, zoom);
    if (isLoading(tileCoord))
        return &loadingImg;

    if (!images.contains(tileCoord))
    {
        loadImage(tileCoord, 100);
        return &loadingImg;
    }

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

ImageCache::Tile::Tile(const QPixmap& _image)
:image(_image), life(DefaultLife){
}


#define LOADIMAGE(X, Y) tryload.x = (X);tryload.y = (Y);loadImage(tryload, possibility/2);
void ImageCache::loadImage(const TileCoord& tileCoord, int possibility)
{
//     if (possibility < 5 || images.contains(tileCoord) || tileCoord.x < 0 || tileCoord.y < 0 || tileCoord.x >= (1<<tileCoord.zoom) || tileCoord.y >= (1<<tileCoord.zoom))
//         return;
//     if (possibility > 90 || (rand() % 100)  < possibility)
    {
        emit needLoadImage(tileCoord.x, tileCoord.y, tileCoord.zoom);
//         TileCoord tryload;
//         tryload.zoom = tileCoord.zoom;
//         LOADIMAGE(tileCoord.x-1, tileCoord.y-1);
//         LOADIMAGE(tileCoord.x-1, tileCoord.y);
//         LOADIMAGE(tileCoord.x-1, tileCoord.y+1);
//         LOADIMAGE(tileCoord.x, tileCoord.y-1);
//         LOADIMAGE(tileCoord.x, tileCoord.y+1);
//         LOADIMAGE(tileCoord.x+1, tileCoord.y-1);
//         LOADIMAGE(tileCoord.x+1, tileCoord.y);
//         LOADIMAGE(tileCoord.x+1, tileCoord.y+1);
    }
}
#undef LOADIMAGE

// As this method is queued to run in the same thread, there is no race condition
void ImageCache::imageLoaded(int x, int y, int zoom, QPixmap *img)
{
    if (img != NULL)
    {
        ImageCache::Tile newtile;
        newtile.image = *img;
        delete img;
        images.insert(TileCoord(x, y, zoom), newtile);
        emit imageChanged(x, y, zoom, &newtile.image);
    }
}

void ImageCache::downloadImage(int x, int y, int zoom){
    if (downloader)
        downloader->addDownload(
            getDownloadUrl(x, y, zoom),
            x, y, zoom);
}

void ImageCache::setUrlTemplate(const QString &newUrlTemplate){
    if (urlTemplate != newUrlTemplate){
        urlTemplate = newUrlTemplate;
        clear();
    }
}

//TODO: maybe need better templateConstruction routine....
QString ImageCache::getDownloadUrl(int x, int y, int zoom){
    return QString(urlTemplate)
        .replace("%1", QString::number(x))
        .replace("%2", QString::number(y))
        .replace("%3", QString::number(zoom));
}

void ImageCache::paintLoadingImage(){
    QPainter painter(&loadingImg);
    painter.setBrush(QColor(128, 128, 128));
    painter.setPen(QColor(255, 255, 255));
    painter.drawRect(0, 0, 255, 255);
    painter.drawText(0, 0, 255, 255, Qt::AlignHCenter|Qt::AlignVCenter,	tr("Loading..."));
}

void ImageCache::timerEvent(QTimerEvent *ev)
{
    for (QMap<TileCoord, Tile>::iterator i = images.begin(); i != images.end();){
        QMap<TileCoord, Tile>::iterator cur = i; 
        ++i;
        for (int cnt=0;cnt<ticks;++cnt)
            cur->countdown();
        if (cur->getLife() <= 0)
            images.erase(cur);
    }
    ticks = 0;
    timer.stop();
}

bool ImageCache::isLoading(const TileCoord& tileCoord)
{
    if (downloader){
        QString str = getDownloadUrl(tileCoord.x, tileCoord.y, tileCoord.zoom);
        return (downloader->isLoadingUrl(str));
    }
    return false;
}

void ImageCache::tryDownload(int x, int y, int zoom)
{
    if (!isLoading(TileCoord(x, y, zoom))){
        downloadImage(x, y, zoom);
    }
}

void ImageCache::clear(){
    images.clear();
}

QString ImageCache::getCoordsQstr(int x, int y, int zoom)
{
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

void ImageCache::tick()
{
    ++ticks;
    if (!timer.isActive())
        timer.start(100, this);
}

AsyncLoader::AsyncLoader()
{
    this->moveToThread(this);
    this->start();
}

void AsyncLoader::setDB(QSqlDatabase *db)
{
    this->db = db;
    prepareStatements();
}

void AsyncLoader::prepareStatements()
{
    for (int i=0;i< sizeof(queries) / sizeof(queries[0]);i++)
    {
        queries[i] = QSqlQuery();
        queries[i].prepare(QString("SELECT data FROM [%1] WHERE x=? AND y=? ").arg(i));
        queries[i].setForwardOnly(true);
    }
}

void AsyncLoader::loadImage(int x, int y, int zoom)
{
    QSqlQuery &query = queries[zoom];
    query.bindValue(0, x);
    query.bindValue(1, y);
    QPixmap *image = new QPixmap;
    if (query.exec() && query.next() && image->loadFromData(query.value(0).toByteArray()))
    {
        emit this->imageLoaded(x, y, zoom, image);
    }
    else
        delete image;
}

void AsyncLoader::run()
{
    exec();
}
