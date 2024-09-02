QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -lWs2_32

SOURCES += \
    chatdialog.cpp \
    ckernel.cpp \
    logindialog.cpp \
    main.cpp \
    friendlist.cpp \
    mediator/IMediator.cpp \
    mediator/TcpClientMediator.cpp \
    net/TcpClient.cpp \
    useritem.cpp

HEADERS += \
    chatdialog.h \
    ckernel.h \
    friendlist.h \
    logindialog.h \
    mediator/IMediator.h \
    mediator/TcpClientMediator.h \
    net/TcpClient.h \
    net/def.h \
    net/inet.h \
    useritem.h

FORMS += \
    chatdialog.ui \
    friendlist.ui \
    logindialog.ui \
    useritem.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resourse.qrc
