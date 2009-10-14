# #####################################################################
# Automatically generated by qmake (2.01a) ? ?? 2 19:54:24 2009
# #####################################################################
TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . \
    ./rclib/include \
    ../protocol-buffer-src
LIBPATH += ../protocol-buffer-src/lib

# Input
win32:CONFIG += console
QT += webkit \
    network
FORMS += mapview.ui \
    mainwindow.ui
HEADERS += mapview.h \
           bluetoothmanager.h \
           ConnectionManager.h \
           LocationSvc.h \
           mainwindow.h \
           callentry.h \
           prototools.h \
           simplerpccontroller.h \
           streamcallbackinfo.h \
           twowayrpccontroller.h \
           twowaystream.h \
           ../protocol-buffer-src/MapProtocol.pb.h

SOURCES += main.cpp \
           mapview.cpp \
           bluetoothmanager.cpp \
           ConnectionManager.cpp \
           LocationSvc.cpp \
           mainwindow.cpp \
           ../protocol-buffer-src/MapProtocol.pb.cc

RESOURCES += webpage.qrc  #for the convinience of development

# DEPLOYMENT_PLUGIN += qsqlite qjpeg qgif qmng

linux-* {
    OBJECTS_DIR = obj/
    SOURCES += mapview_linux.cpp
    LIBS += -lprotobuf_linux_x86
}

wince* {
    SOURCES += bluetoothmanager_wince.cpp \
               mapview_wince.cpp \
               GPSLocationSvc_wince.cpp
    HEADERS += bluetoothmanager_wince.h \
			   bluetoothmanager_win_common.h \
               GPSLocationSvc_wince.h
    LIBS += bthutil.lib \
            ws2.lib \
            Gpsapi.lib \
            libprotobuf_wince_release.lib \
            protorpcpp_wince_release.lib
}

win32:!wince* {
    SOURCES += mapview_win32.cpp \
	           bluetoothmanager_win.cpp
    HEADERS += bluetoothmanager_wince.h \
			   bluetoothmanager_win.h
	LIBS += Ws2_32.lib \
            protorpcpp_win32_release.lib
    CONFIG(debug, debug|release) {
        LIBS += libprotobuf_win32_debug.lib
    } else {
        LIBS += libprotobuf_win32_release.lib
    }
}
