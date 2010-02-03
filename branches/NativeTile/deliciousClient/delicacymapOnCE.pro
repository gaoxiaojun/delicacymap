TEMPLATE = app
TARGET = delicacyClient
DEPENDPATH += .
INCLUDEPATH += ./rclib/include \
    ../protocol-buffer-src \
    ../QtMobility/include
LIBPATH += ../protocol-buffer-src/lib
OBJECTS_DIR = ./objs
CONFIG += mobility
MOBILITY += location \
    bearer

# Input
win32:CONFIG += console
QT += webkit \
    network
FORMS += mainwindow.ui \
    loginWindow.ui
HEADERS += mapview.h \
    MapListener.h \
    MapListenerPrivate.h \
    CommentItemDelegate.h \
    md5.h \
    bluetoothmanager.h \
    MapDataSource.h \
    mainwindow.h \
    LoginWindow.h \
    QTProtobufChannel.h \
	QTProtobufChannelDriver.h \
    QTProtobufWaitResponse.h \
    DisplaySchema.h \
    Session.h \
    ../protocol-buffer-src/Message.pb.h \
    ../protocol-buffer-src/MapProtocol.pb.h
SOURCES += main.cpp \
    MapListener.cpp \
    MapListenerPrivate.cpp \
    CommentItemDelegate.cpp \
    md5.cpp \
    mapview.cpp \
    bluetoothmanager.cpp \
    MapDataSource.cpp \
    mainwindow.cpp \
    LoginWindow.cpp \
    QTProtobufChannel.cpp \
	QTProtobufChannelDriver.cpp \
    QTProtobufWaitResponse.cpp \
    DisplaySchema.cpp \
    Session.cpp \
    ../protocol-buffer-src/Message.pb.cc \
    ../protocol-buffer-src/MapProtocol.pb.cc
RESOURCES += webpage.qrc

# DEPLOYMENT_PLUGIN += qsqlite qjpeg qgif qmng
linux-* { 
    RCC_DIR = ./objs
    MOC_DIR = ./objs
    SOURCES += mapview_linux.cpp
    RESOURCES += mainwindow.qrc
    
    # LIBPATH += /usr/local/lib
    LIBS += -lprotobuf_linux_x86
}
wince* {
    RESOURCES += mainwindow_480_800.qrc
    contains(DEFINES, _WIN32_WCE=0x600) {
        myFiles.sources = F:\QT\delicacymap\msvcr90.dll
        myFiles.path = %CSIDL_PROGRAM_FILES%\delicacyClient
        DEPLOYMENT += myFiles
        SOURCES += mapview_wince.cpp
        LIBS += libprotobuf-lite_wince6_release.lib \
                ../QtMobility/lib_wince6/QtBearer_tp.lib 
    }else{
        SOURCES += bluetoothmanager_wince.cpp \
            mapview_wince.cpp
        HEADERS += bluetoothmanager_wince.h \
            bluetoothmanager_win_common.h
        LIBS += bthutil.lib \
            ws2.lib \
            Gpsapi.lib \
            libprotobuf_wince_release.lib \
            ../QtMobility/lib_wince/QtBearer_tp.lib \
            ../QtMobility/lib_wince/QtLocation_tp.lib
    }
}
win32:!wince* { 
    SOURCES += mapview_win32.cpp \
        bluetoothmanager_win.cpp
    HEADERS += bluetoothmanager_wince.h \
        bluetoothmanager_win.h
    RESOURCES += mainwindow.qrc
    LIBS += Ws2_32.lib \
        ../QtMobility/lib_win32/QtBearer_tp.lib \
        ../QtMobility/lib_win32/QtLocation_tp.lib
    CONFIG(debug, debug|release):LIBS += libprotobuf-lite_win32_debug.lib
    else:LIBS += libprotobuf-lite_win32_release.lib
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

    CONFIG += x86_64
    LIBS += -lprotobuf -F../QtMobility/lib_macx -framework QtLocation_tp -framework QtBearer_tp
    SOURCES += mapview_linux.cpp
    RESOURCES += mainwindow.qrc
}