TEMPLATE = app
TARGET = delicacyClient
DEPENDPATH += .
INCLUDEPATH += ./rclib/include \
    ../protocol-buffer-src \
    ../QtMobility/include
LIBPATH += ../protocol-buffer-src/lib
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
    QTProbufController.h \
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
    
    # LIBPATH += /usr/local/lib
    LIBS += -lprotobuf_linux_x86
}
wince* { 
    SOURCES += bluetoothmanager_wince.cpp \
        mapview_wince.cpp
    HEADERS += bluetoothmanager_wince.h \
        bluetoothmanager_win_common.h
    RESOURCES += mainwindow_480_800.qrc
    LIBS += bthutil.lib \
        ws2.lib \
        Gpsapi.lib \
        libprotobuf_wince_release.lib \
        ../QtMobility/lib_wince/QtBearer_tp.lib \
        ../QtMobility/lib_wince/QtLocation_tp.lib
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
    CONFIG(debug, debug|release):LIBS += libprotobuf_win32_debug.lib
    else:LIBS += libprotobuf_win32_release.lib
}
symbian { 
    target.capability = networkservices
    target.epocheapsize = 0x400000 \
        0x3000000
        
    INCLUDEPATH += ../protocol-buffer-src/src
    
    SOURCES += ../protocol-buffer-src/src/google/protobuf/descriptor.cc \
              ../protocol-buffer-src/src/google/protobuf/descriptor.pb.cc \
              ../protocol-buffer-src/src/google/protobuf/dynamic_message.cc \
              ../protocol-buffer-src/src/google/protobuf/generated_message_reflection.cc \
              ../protocol-buffer-src/src/google/protobuf/generated_message_util.cc \
              ../protocol-buffer-src/src/google/protobuf/message.cc \
              ../protocol-buffer-src/src/google/protobuf/message_lite.cc \
              ../protocol-buffer-src/src/google/protobuf/reflection_ops.cc \
              ../protocol-buffer-src/src/google/protobuf/repeated_field.cc \
              ../protocol-buffer-src/src/google/protobuf/service.cc \
              ../protocol-buffer-src/src/google/protobuf/text_format.cc \
              ../protocol-buffer-src/src/google/protobuf/unknown_field_set.cc \
              ../protocol-buffer-src/src/google/protobuf/wire_format.cc \
              ../protocol-buffer-src/src/google/protobuf/wire_format_lite.cc \
              ../protocol-buffer-src/src/google/protobuf/extension_set.cc \
              ../protocol-buffer-src/src/google/protobuf/extension_set_heavy.cc \
              ../protocol-buffer-src/src/google/protobuf/io\coded_stream.cc \
              ../protocol-buffer-src/src/google/protobuf/io\tokenizer.cc \
              ../protocol-buffer-src/src/google/protobuf/io\zero_copy_stream.cc \
              ../protocol-buffer-src/src/google/protobuf/io\zero_copy_stream_impl.cc \
              ../protocol-buffer-src/src/google/protobuf/io\zero_copy_stream_impl_lite.cc \
              ../protocol-buffer-src/src/google/protobuf/stubs\common.cc \
              ../protocol-buffer-src/src/google/protobuf/stubs\hash.cc \
              ../protocol-buffer-src/src/google/protobuf/stubs\once.cc \
              ../protocol-buffer-src/src/google/protobuf/stubs\strutil.cc \
              ../protocol-buffer-src/src/google/protobuf/stubs\structurally_valid.cc \
              ../protocol-buffer-src/src/google/protobuf/stubs\substitute.cc
}
