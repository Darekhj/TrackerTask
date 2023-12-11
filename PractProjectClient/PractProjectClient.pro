QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    authorizwindow.cpp \
    filterdialog.cpp \
    givetask.cpp \
    main.cpp \
    mainwindow.cpp \
    mytasks.cpp \
    othertasks.cpp

HEADERS += \
    authorizwindow.h \
    filterdialog.h \
    givetask.h \
    mainwindow.h \
    mytasks.h \
    othertasks.h

FORMS += \
    authorizwindow.ui \
    givetask.ui \
    mainwindow.ui \
    mytasks.ui \
    othertasks.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
