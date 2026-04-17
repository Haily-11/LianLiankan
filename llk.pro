QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    basicmode.cpp \
    itemgrid.cpp \
    main.cpp \
    gamewindow.cpp \
    startmenu.cpp

HEADERS += \
    basicmode.h \
    gamewindow.h \
    itemgrid.h \
    startmenu.h

FORMS += \
    basicmode.ui \
    gamewindow.ui \
    startmenu.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc \
    imgt.qrc \
    res.qrc

DISTFILES += \
    .gitignore
