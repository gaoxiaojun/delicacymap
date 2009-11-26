TEMPLATE = app
TARGET = delicacyClient
DEPENDPATH += .
INCLUDEPATH += ./rclib/include \
               ../protocol-buffer-src
LIBPATH += ../protocol-buffer-src/lib

# Input
win32:CONFIG += console
QT += webkit \
      network
FORMS += mainwindow.ui \
         loginWindow.ui
		 
HEADERS += mapview.h \
           MapListener.h \
           MapListenerPrivate.h \
           md5.h \
           bluetoothmanager.h \
           MapDataSource.h \
           LocationSvc.h \
           mainwindow.h \
           LoginWindow.h \
           QTProtobufChannel.h \
           QTProbufController.h \
           QTProtobufWaitResponse.h \
           DisplaySchema.h \
           Session.h \
           ../protocol-buffer-src/Message.pb.h \
           ../protocol-buffer-src/MapProtocol.pb.h

SOURCES += main.cpp \
           MapListener.cpp \
           MapListenerPrivate.cpp \
           md5.cpp \
           mapview.cpp \
           bluetoothmanager.cpp \
           MapDataSource.cpp \
           LocationSvc.cpp \
           mainwindow.cpp \
           LoginWindow.cpp \
           QTProtobufChannel.cpp \
           QTProbufController.cpp \
           QTProtobufWaitResponse.cpp \
           DisplaySchema.cpp \
           Session.cpp \
           ../protocol-buffer-src/Message.pb.cc \
           ../protocol-buffer-src/MapProtocol.pb.cc

RESOURCES += webpage.qrc

# DEPLOYMENT_PLUGIN += qsqlite qjpeg qgif qmng

linux-* {
    OBJECTS_DIR = obj/
    SOURCES += mapview_linux.cpp
    RESOURCES += mainwindow.qrc
#    LIBPATH += /usr/local/lib
    LIBS += -lprotobuf_linux_x86
}

wince* {
    SOURCES += bluetoothmanager_wince.cpp \
               mapview_wince.cpp \
               GPSLocationSvc_wince.cpp
    HEADERS += bluetoothmanager_wince.h \
			   bluetoothmanager_win_common.h \
               GPSLocationSvc_wince.h
    RESOURCES += mainwindow_480_800.qrc
    LIBS += bthutil.lib \
            ws2.lib \
            Gpsapi.lib \
            libprotobuf_wince_release.lib
}

win32:!wince* {
    SOURCES += mapview_win32.cpp \
	           bluetoothmanager_win.cpp
    HEADERS += bluetoothmanager_wince.h \
			   bluetoothmanager_win.h
    RESOURCES += mainwindow.qrc
	LIBS += Ws2_32.lib
    CONFIG(debug, debug|release) {
        LIBS += libprotobuf_win32_debug.lib
    } else {
        LIBS += libprotobuf_win32_release.lib
    }
}
