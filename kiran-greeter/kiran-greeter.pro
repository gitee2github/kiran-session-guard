#依赖的Qt模块
QT += core gui x11extras
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
message("kiran-greeter: "$$DESTDIR)

#编译目标名
TARGET = lightdm-kiran-greeter

#生成Makefile模板
TEMPLATE = app

#宏定义
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += TEST

#配置项
CONFIG += c++11
CONFIG += link_pkgconfig

#头文件包含路径
INCLUDEPATH += /usr/include/lightdm-qt5-3/
INCLUDEPATH += ../public/

#源文件
SOURCES += \
    ../public/log.cpp \
    ../public/disabledeselectlistwidget.cpp\
    ../public/useravatarwidget.cpp \
    capslocksnoop.cpp \
    greeterbackground.cpp \
    greeterkeyboard.cpp \
    greeterlineedit.cpp \
    greeterloginwindow.cpp \
    greeterscreenmanager.cpp \
    greetersetting.cpp \
    main.cpp \
    synclockstatus.cpp \
    userlistitem.cpp \
    userlistwidget.cpp \
    greetermenuitem.cpp \
    scalinghelper.cpp \
    cursorhelper.cpp \
    loginbutton.cpp \
    shadowlabel.cpp \
    greeterpromptmsgmanager.cpp

#头文件
HEADERS += \
    ../public/log.h \
    ../public/greeter-setting-define.h \
    ../public/disabledeselectlistwidget.h\
    ../public/useravatarwidget.h \
    capslocksnoop.h \
    greeterbackground.h \
    greeterkeyboard.h \
    greeterlineedit.h \
    greeterloginwindow.h \
    greeterscreenmanager.h \
    greetersetting.h \
    synclockstatus.h \
    userlistitem.h \
    userlistwidget.h \
    userinfo.h \
    greetermenuitem.h \
    scalinghelper.h \
    cursorhelper.h \
    loginbutton.h \
    shadowlabel.h \
    greeterpromptmsgmanager.h

#界面文件
FORMS += \
    greeterlineedit.ui \
    greeterloginwindow.ui \
    userlistwidget.ui \
    userlistitem.ui \
    loginbutton.ui

#依赖库
LIBS += \
    -llightdm-qt5-3 \
    -lX11 \
    -lXtst \
    -lXrandr \
    -lXcursor \
    -lXfixes

#资源文件
RESOURCES += \
    image.qrc \
    themes.qrc

#其他文件 (只是为了QtCreator能加载)
OTHER_FILES += \
    translations/lightdm-kiran-greeter.zh_CN.ts \
    config/lightdm-kiran-greeter.conf \
    config/99-lightdm-kiran-greeter.conf \
    config/lightdm-kiran-greeter.desktop

#翻译文件目录
TRANSLATIONS = \
    translations/lightdm-kiran-greeter.zh_CN.ts

#安装选项
target_translation.files = ./translations/lightdm-kiran-greeter.zh_CN.qm
target_translation.path = $$DESTDIR/usr/share/lightdm-kiran-greeter/translations/

target_config.files = ./config/lightdm-kiran-greeter.conf
target_config.path = $$DESTDIR/etc/lightdm/

target_sysconfig.files = ./config/99-lightdm-kiran-greeter.conf
target_sysconfig.path =  $$DESTDIR/usr/share/lightdm/lightdm.conf.d/

xgreeter_config.files = ./config/lightdm-kiran-greeter.desktop
xgreeter_config.path = $$DESTDIR/usr/share/xgreeters/

target.path = $$DESTDIR/usr/sbin/
INSTALLS = target target_translation target_config target_sysconfig xgreeter_config
