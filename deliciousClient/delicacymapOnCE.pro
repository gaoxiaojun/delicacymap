TEMPLATE = app
TARGET = delicacyClient
DEPENDPATH += .
INCLUDEPATH += ../rclib/include \
    ../protocol-buffer-src \
    ../QtMobility/include
LIBPATH += ../protocol-buffer-src/lib
CONFIG += mobility
CONFIG -= flat
DEFINES += TIXML_USE_TICPP
MOBILITY += location \
    bearer \
    systeminfo

# Input
win32:CONFIG += console
QT += network \
      sql
FORMS += mainwindow.ui \
    loginWindow.ui
HEADERS += md5.h \
    bluetoothmanager.h \
    Configurations.h \
    MapDataSource.h \
    MapController.h \
    mainwindow.h \
    LoginWindow.h \
    QTProtobufChannel.h \
	QTProtobufChannelDriver.h \
    QTProtobufWaitResponse.h \
    Session.h \
    ../rclib/include/TinyXMLPP/ticpp.h \
    ../rclib/include/TinyXMLPP/tinyxml.h \
    ../rclib/include/TinyXMLPP/tinystr.h \
    ../rclib/include/TinyXMLPP/ticpprc.h \
    ../protocol-buffer-src/Message.pb.h \
    ../protocol-buffer-src/MapProtocol.pb.h \
    OfflineMap/JSON/json_spirit_error_position.h \
    OfflineMap/JSON/json_spirit_reader_template.h \
    OfflineMap/JSON/json_spirit_writer_template.h \
    OfflineMap/JSON/json_spirit_value.h \
    OfflineMap/CoordsHelper.h \
    OfflineMap/MarkerItem.h \
    OfflineMap/MapServices.h \
    OfflineMap/Decorator.h \
    OfflineMap/ImageCache.h \
    OfflineMap/MapDecorators.h \
    OfflineMap/MapViewBase.h \
    OfflineMap/Downloader.h \
    OfflineMap/GeoCoord.h
SOURCES += main.cpp \
    md5.cpp \
    Configurations.cpp \
    bluetoothmanager.cpp \
    MapDataSource.cpp \
    MapController.cpp \
    mainwindow.cpp \
    LoginWindow.cpp \
    QTProtobufChannel.cpp \
	QTProtobufChannelDriver.cpp \
    QTProtobufWaitResponse.cpp \
    Session.cpp \
    ../rclib/include/TinyXMLPP/ticpp.cpp \
    ../rclib/include/TinyXMLPP/tinyxml.cpp \
    ../rclib/include/TinyXMLPP/tinystr.cpp \
    ../rclib/include/TinyXMLPP/tinyxmlparser.cpp \
    ../rclib/include/TinyXMLPP/tinyxmlerror.cpp \
    ../protocol-buffer-src/Message.pb.cc \
    ../protocol-buffer-src/MapProtocol.pb.cc \
    OfflineMap/EditableRouteMarker.cpp \
    OfflineMap/MarkerItem.cpp \
    OfflineMap/MapServices.cpp \
    OfflineMap/Decorator.cpp \
    OfflineMap/ImageCache.cpp \
    OfflineMap/MapDecorators.cpp \
    OfflineMap/MapViewBase.cpp \
    OfflineMap/Downloader.cpp \
    OfflineMap/GeoCoord.cpp

TRANSLATIONS = delicious_cn.ts

# DEPLOYMENT_PLUGIN += qsqlite qjpeg qgif qmng
linux-* { 
    RCC_DIR = ./objs
    MOC_DIR = ./objs
    OBJECTS_DIR = ./objs
    RESOURCES += mainwindow.qrc
    
    # LIBPATH += /usr/local/lib
    LIBS += -lprotobuf_linux_x86
}
wince* {
    RESOURCES += mainwindow_480_800.qrc
    configs.sources = configs.xml delicious_cn.qm
    DEPLOYMENT += configs
    contains(DEFINES, _WIN32_WCE=0x600) {
        myFiles.sources = F:\QT\delicacymap\msvcr90.dll
        DEPLOYMENT += myFiles
        LIBS += libprotobuf-lite_wince6_release.lib \
                ../QtMobility/lib_wince6/QtBearer_tp.lib 
    }else{
        SOURCES += bluetoothmanager_wince.cpp
        HEADERS += bluetoothmanager_wince.h \
            bluetoothmanager_win_common.h
        LIBS += bthutil.lib \
            ws2.lib \
            Gpsapi.lib \
            libprotobuf-lite_wince_release.lib \
            ../QtMobility/lib_wince/QtBearer.lib \
            ../QtMobility/lib_wince/QtLocation.lib \
            ../QtMobility/lib_wince/QtSystemInfo.lib
    }
}
win32:!wince* { 
    SOURCES += bluetoothmanager_win.cpp
    HEADERS += bluetoothmanager_wince.h \
        bluetoothmanager_win.h
    RESOURCES += mainwindow.qrc
    LIBS += Ws2_32.lib
    CONFIG(debug, debug|release):LIBS += libprotobuf-lite_win32_debug.lib \
                                         ../QtMobility/lib_win32/QtBearerd.lib \
                                         ../QtMobility/lib_win32/QtLocationd.lib \
                                         ../QtMobility/lib_win32/QtSystemInfod.lib
    else:LIBS += libprotobuf-lite_win32_release.lib \
                 ../QtMobility/lib_win32/QtBearer.lib \
                 ../QtMobility/lib_win32/QtLocation.lib \
                 ../QtMobility/lib_win32/QtSystemInfo.lib
}
symbian { 
    target.capability = networkservices
    target.epocheapsize = 0x400000 \
        0x3000000
}
macx {
    INCLUDEPATH += /opt/local/include
    LIBPATH += /opt/local/lib
    RCC_DIR = ./objs
    MOC_DIR = ./objs
    OBJECTS_DIR = ./objs

    CONFIG += x86_64
    LIBS += -lprotobuf -F../QtMobility/lib_macx -framework QtLocation -framework QtBearer -framework QtSystemInfo
    RESOURCES += mainwindow.qrc
}
