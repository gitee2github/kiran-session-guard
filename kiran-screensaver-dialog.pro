#依赖的Qt模块
QT += core gui x11extras dbus
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#目标文件名
TARGET = kiran-screensaver-dialog

#生成Application Makefile模板
TEMPLATE = app

#宏定义
DEFINES += QT_DEPRECATED_WARNINGS

#配置项 g++
CONFIG += c++11

#包含头文件目录 -I
INCLUDEPATH += \
    /usr/include/glib-2.0/ \
    /usr/lib64/glib-2.0/include/ \
    /usr/include/accountsservice-1.0/ \
    /usr/include/security/

#源文件
SOURCES += \
    main.cpp \
    log.cpp \
    xlibhelper.cpp \
    lockplug.cpp \
    screensaverdialog.cpp \
    useravatarwidget.cpp \
    greeterlineedit.cpp \
    gsettingshelper.cpp \
    greeterkeyboard.cpp \
    dbusapihelper.cpp \
    pamauthproxy.cpp \
    tool.cpp

#头文件
HEADERS += \
    log.h \
    xlibhelper.h \
    lockplug.h \
    screensaverdialog.h \
    useravatarwidget.h \
    greeterlineedit.h \
    gsettingshelper.h \
    greeterkeyboard.h \
    dbusapihelper.h \
    pamauthproxy.h \
    tool.h

#界面文件
FORMS += \
    screensaverdialog.ui \
    greeterlineedit.ui

#链接库
LIBS += \
    -lX11 \
    -lglib-2.0 \
    -lgio-2.0 \
    -lgobject-2.0 \
    -lpam

#资源文件
RESOURCES += \
    image.qrc \
    styles.qrc

#翻译文件
TRANSLATIONS = \
    translations/kiran-screensaver-dialog.zh_CN.ts

#其他文件，仅只是为了加入到Creator
OTHER_FILES += \
    README.md\
    translations/kiran-screensaver-dialog.zh_CN.ts

#安装选项
target_translation.files = ./translations/kiran-screensaver-dialog.zh_CN.qm
target_translation.path = $$DESTDIR/usr/share/kiran-screensaver-dialog/translations/

target.path = $$DESTDIR/usr/libexec/

INSTALLS = target target_translation
