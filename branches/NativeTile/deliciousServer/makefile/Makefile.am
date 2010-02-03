AUTOMAKE_OPTIONS = foreign
bin_PROGRAMS = deliciousServer
deliciousServer_SOURCES = AsioRpcController.h \
                          ../DBResult.h \
                          ../naked_conn.h \
                          ../targetver.h \
                          ../connectionpool.h \
                          ../deliciousDataAdapter.h \
                          ../sqlite3.h \
                          ../tcpserver.h \
                          ../DBContext.h \
                          ../DMServiceLocalDBImpl.h \
                          ../ProtubufDBRowConversion.h \
                          ../Messenger.h \
                          ../stdafx.h \
                          ../stdafx.cpp \
                          ../AsioRpcController.cpp \
                          ../deliciousDataAdapter.cpp  \
                          ../naked_conn.cpp \
                          ../DBContext.cpp	\
                          ../deliciousServer.cpp \
                          ../DBResult.cpp \
                          ../DMServiceLocalDBImpl.cpp \
                          ../ProtubufDBRowConversion.cpp \
                          ../Messenger.cpp \
                          ../implicit_link.c \
                          ../sqlite3.c \
                          ../../protocol-buffer-src/MapProtocol.pb.cc \
                          ../../protocol-buffer-src/Message.pb.cc

deliciousServer_CPPFLAGS= -I$(PANTHEIOS)/include \
                          -I$(STLSOFT)/include \
                          -I../../protocol-buffer-src

deliciousServer_LDFLAGS = -L$(PANTHEIOS)/lib \
                          -L../../protocol-buffer-src/lib

deliciousServer_LDADD   = -lpthread \
                          -ldl \
                          -lprotobuf_linux_x86 \
                          -lboost_system-mt \
                          -lboost_thread-mt \
                          -lboost_program_options-mt \
                          -lpantheios.1.core.gcc44.mt \
                          -lpantheios.1.fe.simple.gcc44.mt \
                          -lpantheios.1.be.lrsplit.gcc44.mt \
                          -lpantheios.1.bel.fprintf.gcc44.mt \
                          -lpantheios.1.ber.file.gcc44.mt \
                          -lpantheios.1.bec.file.gcc44.mt \
                          -lpantheios.1.util.gcc44.mt

