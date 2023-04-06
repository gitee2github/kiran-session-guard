/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-session-guard is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#include "prefs.h"
#include <QMutex>
#include <QScopedPointer>
#include <QSettings>

namespace Kiran
{
namespace SessionGuard
{
namespace Locker
{
Prefs::Prefs()
{
}

void Prefs::init()
{
    QSettings settings("/usr/share/kiran-session-guard/screensaver-dialog.ini", QSettings::IniFormat);
    ///power
    settings.beginGroup("Power");

    auto canPowerOff = settings.value("can-poweroff");
    m_canPowerOff = canPowerOff.toBool();

    auto canReboot = settings.value("can-reboot");
    m_canReboot = canReboot.toBool();

    auto canSuspend = settings.value("can-suspend");
    m_canSuspend = canSuspend.toBool();
}

Prefs::~Prefs()
{
}

Prefs* Prefs::m_instance = nullptr;
void Prefs::globalInit()
{
    m_instance = new Prefs();
    m_instance->init();
}

bool Prefs::canPowerOff()
{
    return m_canPowerOff;
}

bool Prefs::canReboot()
{
    return m_canReboot;
}

bool Prefs::canSuspend()
{
    return m_canSuspend;
}

}  // namespace Locker
}  // namespace SessionGuard
}  // namespace Kiran