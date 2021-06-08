//
// Created by lxh on 2020/12/30.
//

#include "kiran-greeter-prefs.h"

#include <QDBusConnection>
#include <QSettings>

KiranGreeterPrefs *KiranGreeterPrefs::instance()
{
    static QMutex mutex;
    static QScopedPointer<KiranGreeterPrefs> pInst;

    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new KiranGreeterPrefs);
        }
    }

    return pInst.data();
}

KiranGreeterPrefs::KiranGreeterPrefs()
    : GreeterDBusInterface(GreeterDBusInterface::staticInterfaceName(),
                           GreeterDBusInterface::staticInterfacePath(),
                           QDBusConnection::systemBus())
{
    QDBusConnection::systemBus().connect(GreeterDBusInterface::staticInterfaceName(),
                                         GreeterDBusInterface::staticInterfacePath(),
                                         "org.freedesktop.DBus.Properties",
                                         "PropertiesChanged",
                                         this, SLOT(handlePropertiesChanged(QDBusMessage)));

    QSettings settings("/usr/share/lightdm-kiran-greeter/greeter.ini",QSettings::IniFormat);
    settings.beginGroup("Common");
    auto hiddenSesion = settings.value("hidden-sessions");
    m_hiddenSessions = hiddenSesion.toStringList();
}

KiranGreeterPrefs::~KiranGreeterPrefs()
{
}

void KiranGreeterPrefs::handlePropertiesChanged(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
    for (auto iter = changedProps.begin(); iter != changedProps.end(); iter++)
    {
        emit propertyChanged(iter.key(), iter.value());
    }
}

QStringList KiranGreeterPrefs::hiddenSessions()
{
    return m_hiddenSessions;
}
