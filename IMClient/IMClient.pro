QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -lWs2_32

SOURCES += \
    Net/ClientHeartbeat.cpp \
    Net/TcpClient.cpp \
    Net/TcpServer.cpp \
    Net/Udp.cpp \
    chatdialog.cpp \
    ckernel.cpp \
    frienditem.cpp \
    friendlist.cpp \
    main.cpp \
    logindialog.cpp \
    mediator/INetMediator.cpp \
    mediator/TcpClientMediator.cpp \
    mediator/TcpServerMediator.cpp \
    mediator/UdpMediator.cpp

HEADERS += \
    Net/ClientHeartbeat.h \
    Net/INet.h \
    Net/TcpClient.h \
    Net/TcpServer.h \
    Net/Udp.h \
    Net/def.h \
    chatdialog.h \
    ckernel.h \
    frienditem.h \
    friendlist.h \
    logindialog.h \
    mediator/INetMediator.h \
    mediator/TcpClientMediator.h \
    mediator/TcpServerMediator.h \
    mediator/UdpMediator.h

FORMS += \
    chatdialog.ui \
    frienditem.ui \
    friendlist.ui \
    logindialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
