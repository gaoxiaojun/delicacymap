#ifndef IMAGE_CACHE_H
#define IMAGE_CACHE_H
#include <QObject>
#include <QString>
#include <QBasicTimer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMap>
#include <QPixmap>
#include <QThread>

class Downloader;

class AsyncLoader : public QThread
{
    Q_OBJECT
public:
    AsyncLoader();
    void setDB(QSqlDatabase *);
public slots:
    void loadImage(int x, int y, int zoom);
signals:
    void imageLoaded(int x, int y, int zoom, QPixmap* img);
protected:
    void prepareStatements();
    void run();
private:
    QSqlQuery queries[18];
    QSqlDatabase *db;
};

class ImageCache: public QObject
{
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
        enum {DefaultLife=5};
        inline int getLife(){return life;};
        inline void revive(){life = DefaultLife;};
        inline int countdown(){life--; return life;};
        QPixmap image;
        Tile();
        Tile(const QPixmap& image);
    protected:
        int life;
    };
	
    QPixmap* getImage(int x, int y, int zoom);
    void setDownloader(Downloader* newDownloader);
    void downloadImage(int x, int y, int zoom);
    void tryDownload(int x, int y, int zoom);
    void setCacheDBPath(const QString& cacheDir);
    const QString& getCacheDBPath();
    ImageCache();
    ~ImageCache();
    void tick();
    void clear();
    const QString& getUrlTemplate();
    void setUrlTemplate(const QString &newUrlTemplate);
private slots:
    void imageLoaded(int x, int y, int zoom, QPixmap* img);
signals:
    void imageChanged(int x, int y, int zoom, QPixmap* img);
    void needLoadImage(int x, int y, int zoom);

protected:
    void timerEvent(QTimerEvent *);
    void loadImage(const TileCoord& tileCoord, int possibility);
    QString getDownloadUrl(int x, int y, int zoom);
    QString getCoordsQstr(int x, int y, int zoom);
    void paintLoadingImage();
    bool isLoading(const TileCoord& tileCoord);

    AsyncLoader asyncLoader;
    QMap<TileCoord, Tile> images;

    QString dbpath;
    QSqlDatabase db;
    QPixmap loadingImg;
    Downloader* downloader;
    QString urlTemplate;
    QBasicTimer timer;
    int ticks;
};

bool operator< (const ImageCache::TileCoord& arg1, const ImageCache::TileCoord arg2);

#endif
